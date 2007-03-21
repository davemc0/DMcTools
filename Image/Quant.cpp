//////////////////////////////////////////////////////////////////////
// Quant.cpp - Quantize color images to MaxColors colors for saving.
//
// Copyright David K. McAllister, Mar. 1999.
// Taken from XV v. 3.10, which was taken from xgif and others 1987-1999.

#include <Image/Quant.h>
#include <Util/Assert.h>

#include <iostream>
using namespace std;

#include <memory.h>

static inline int DiffP(const pixel &a, const pixel &b)
{
    return (int(a.r)-int(b.r))*(int(a.r)-int(b.r)) +
        ((int(a.g)-int(b.g))*(int(a.g)-int(b.g))) +
        (int(a.b)-int(b.b))*(int(a.b)-int(b.b));
}

// See if there are <= MaxCols unique colors. If so, return pic8.
bool Quantizer::TrivialSolution(int MaxCols, byte *pic8)
{
    NumColors = 0;

    int y;
    for(y=0; y<size && NumColors <= MaxCols; y++)
    {
        pixel &p = Pix[y];
        bool FoundIt = false;
        for(int i=0; i<NumColors; i++)
        {
            if(p == cmap[i])
            {
                FoundIt = true;
                pic8[y] = i;
                break;
            }
        }
        if(!FoundIt)
        {
            if(NumColors >= MaxCols)
                break;

            // Add a new color map entry
            cmap[NumColors] = p;
            pic8[y] = NumColors++;
        }
    }

    return (y >= size);
}

// Reduce the color map by computing the number of unique colors.
// Also overwrite pic8 with the new indices.
void Quantizer::ReduceColorMap(byte *pic8)
{
    byte *cnt = new byte[MaxColors];
    pixel *cmap2 = new pixel[MaxColors];
    byte *pc2nc = new byte[MaxColors];

    memset(cnt, 0, MaxColors);
    memset(pc2nc, 0, MaxColors);
    memset(cmap2, 0, sizeof(pixel) * MaxColors);

    // Flag which colors were actually used.
    int i;
    for(i=0; i<size; i++)
    {
        cnt[pic8[i]] = 1;
    }

    // Reduce color map by removing unused and duplicate colors.
    int nc = 0;
    for(i=0; i<NumColors; i++)
    {
        // See if color #i is already used.
        int j;
        for(j=0; j<i; j++)
        {
            if(cmap[i] == cmap[j])
                break;
        }

        // If the color is unique and used, add it.
        if(j==i && cnt[i])
        {
            // Wasn't found.
            pc2nc[i] = nc;
            cmap2[nc] = cmap[i];
            nc++;
        }
        else
            pc2nc[i] = pc2nc[j];
    }

    // Replace the image with a new one.
    for(i=0; i<size; i++)
    {
        pic8[i] = pc2nc[pic8[i]];
    }

    NumColors = nc;

    memcpy(cmap, cmap2, sizeof(pixel) * MaxColors);

    delete [] cmap2;
    delete [] cnt;
    delete [] pc2nc;
}

// R, G, and B are the sums created when looping over all the pixels.
// Here we simply divide by the counts to make R,G,B be a color map.
void Quantizer::CentroidsToColorMap(int *Counts, int *R, int *G, int *B)
{
    if(NumColors < MaxColors)
        memset(&Counts[NumColors], 0, sizeof(int) * (MaxColors - NumColors));

    NumColors = MaxColors;
    for(int i=0; i<NumColors; i++)
    {
        if(Counts[i] > 0)
        {
            cmap[i].r = R[i] / Counts[i];
            cmap[i].g = G[i] / Counts[i];
            cmap[i].b = B[i] / Counts[i];
        }
        else
        {
            // There was an unused color, so set it randomly.
#ifdef DMC_DEBUG
            cerr << "R";
#endif
            cmap[i] = WorstErrPix;
        }
    }
}

// Map the 24-bit image to an 8-bit image by assigning each pixel
// to the index of the closest color map entry. No dither.
// Counts is how many of each pixel there are.
// Returns error.
DMCINT64 Quantizer::Image24to8(byte *pic8, int *Counts, int *R, int *G, int *B)
{
    DMCINT64 Error = 0;

    memset(Counts, 0, sizeof(int) * NumColors);
    memset(R, 0, sizeof(int) * NumColors);
    memset(G, 0, sizeof(int) * NumColors);
    memset(B, 0, sizeof(int) * NumColors);

    WorstErrPix = Pix[rand() % size]; // This is the index of the pixel that has the most error.
    WorstErr = 0;

    // Set each pixel to closest color.
    for(int y=0; y<size; y++)
    {
        int BestDist = 0x7fffffff, BestC = 0;
        for(int i=0; i<NumColors; i++)
        {
            int Dist = DiffP(Pix[y], cmap[i]);
            if(Dist<BestDist)
            {
                BestDist = Dist;
                BestC = i;
            }
        }

        pic8[y] = (byte) BestC;
        Error += BestDist;

        Counts[BestC]++;

        R[BestC] += Pix[y].r;
        G[BestC] += Pix[y].g;
        B[BestC] += Pix[y].b;

        if(BestDist > WorstErr) {
            WorstErr = BestDist;
            WorstErrPix = Pix[y];
        }
    }

    return Error;
}

// Given a histogram (a list of all the quantized colors and their frequency), and
// given a color map, find the color map entry that is closest to each histogram entry.
// Counts is how many of each pixel there are.
// R,G,B are the sums for later computing the centroids.
// Returns error.
DMCINT64 Quantizer::Image24to8Fast(color_count *CHist, int hsize, int *Counts,
                                   int *R, int *G, int *B)
{
    DMCINT64 Error = 0;

    memset(Counts, 0, sizeof(int) * NumColors);
    memset(R, 0, sizeof(int) * NumColors);
    memset(G, 0, sizeof(int) * NumColors);
    memset(B, 0, sizeof(int) * NumColors);

    WorstErrPix = Pix[rand() % size]; // This is the index of the pixel that has the most error.
    WorstErr = 0;

    // Set each color histogram entry to closest color in color map.
    for(int y=0; y<hsize; y++)
    {
        int BestDist = 0x7fffffff, BestC = 0;
        for(int i=0; i<NumColors; i++)
        {
            int Dist = DiffP(CHist[y].color, cmap[i]);
            if(Dist<BestDist)
            {
                BestDist = Dist;
                BestC = i;
            }
        }

        Error += BestDist * CHist[y].value;

        Counts[BestC] += CHist[y].value;

        R[BestC] += CHist[y].color.r * CHist[y].value;
        G[BestC] += CHist[y].color.g * CHist[y].value;
        B[BestC] += CHist[y].color.b * CHist[y].value;

        if(BestDist > WorstErr) {
            WorstErr = BestDist;
            WorstErrPix = CHist[y].color;
        }
    }

    return Error;
}

#define STOP_EARLY (size/2)

// Given an initial colormap, refine it to reduce error.
DMCINT64 Quantizer::RefineColorMap(byte *pic8)
{
    int *redBig = new int[MaxColors];
    int *greenBig = new int[MaxColors];
    int *blueBig = new int[MaxColors];
    ASSERT_RM(redBig && greenBig && blueBig, "memory alloc failed");

    // The number of pixels in each cluster.
    int *countBig = new int[MaxColors];
    ASSERT_RM(countBig, "memory alloc failed");

    // Compute best fit to that map.
    DMCINT64 OldError = 0x7fffffff;
    DMCINT64 Error = OldError - STOP_EARLY - 1;

    // XXX Not looping here speeds things up.
    //while(Error + STOP_EARLY < OldError)
    for(int iter = 0; iter < 3; iter++)
    {
        OldError = Error;
        Error = Image24to8(pic8, countBig, redBig, greenBig, blueBig);

        CentroidsToColorMap(countBig, redBig, greenBig, blueBig);

#ifdef DMC_DEBUG
        fprintf(stderr, "%03d Mapping Error = %f\n", iter, float(Error)/float(size));
#endif
    }

    delete [] redBig;
    delete [] greenBig;
    delete [] blueBig;
    delete [] countBig;

    return Error;
}

// Given an initial colormap, refine it to reduce error.
// This operates on a histogram of the image, rather than the actual pixels.
DMCINT64 Quantizer::RefineColorMapFast(color_count *CHist, int hsize)
{
    int *redBig = new int[MaxColors];
    int *greenBig = new int[MaxColors];
    int *blueBig = new int[MaxColors];
    ASSERT_RM(redBig && greenBig && blueBig, "memory alloc failed");

    // The number of pixels in each cluster.
    int *countBig = new int[MaxColors];
    ASSERT_RM(countBig, "memory alloc failed");

    // Compute best fit to that map.
    DMCINT64 OldError = 0x7fffffff;
    DMCINT64 Error = OldError - STOP_EARLY - 1;

    int iter = 0;
    while(Error + STOP_EARLY < OldError)
    {
        OldError = Error;
        Error = Image24to8Fast(CHist, hsize, countBig, redBig, greenBig, blueBig);

        CentroidsToColorMap(countBig, redBig, greenBig, blueBig);

        iter++;
#ifdef DMC_DEBUG
        fprintf(stderr, "%03d Fast Mapping Error = %f\n", iter, float(Error)/float(size));
#endif
    }

    delete [] redBig;
    delete [] greenBig;
    delete [] blueBig;
    delete [] countBig;

    return Error;
}

//////////////////////////////////////////////////////////////////////
// Median Cut Color Quantization Algorithm
//////////////////////////////////////////////////////////////////////

#define KEEP_BITS 5
#define HASH_SIZE (1<<(3*KEEP_BITS))
#define KILL_BITS (8-KEEP_BITS)

// Make a 3D historgram of pixel colors by chopping the LSBs of the color values
// to map similar colors to a histogram entry.
color_count *Quantizer::MakeHist(int &num_colors_in_hist)
{
    num_colors_in_hist = 0;
    color_count *CHist = new color_count[HASH_SIZE];
    ASSERT_RM(CHist, "memory alloc failed");

    memset(CHist, 0, sizeof(color_count) * HASH_SIZE);

    // Go through the entire image, building a hash table of colors.
    int i;
    for(i=0; i<size; i++)
    {
        pixel pp = Pix[i];

        int ind = ((pp.r>>KILL_BITS) << (2*KEEP_BITS)) | ((pp.g>>KILL_BITS) << (1*KEEP_BITS)) | (pp.b>>KILL_BITS);

        if(CHist[ind].value == 0) {
            CHist[ind].color = pp;
            num_colors_in_hist++;
        }
        CHist[ind].value++;
    }

    // Sort the table to put empty elements at the end.
    // Linear time: Move a pointer up from the bottom and when it finds a gap,
    // move a pointer down from the top until it can fill the gap.
    int top = HASH_SIZE - 1;
    for(i=0; i<top; i++)
    {
        if(CHist[i].value == 0)
        {
            for( ; top > i; top--)
            {
                if(CHist[top].value)
                {
                    CHist[i] = CHist[top];
                    CHist[top].value = 0;
                    break;
                }
            }
        }
    }

    return CHist;
}

//////////////////////////////////////////////////////////////////////
// Sort the histogram entries along any dimension
static int redcompare(const void *p1, const void *p2)
{
    return (int) ((color_count*)p1)->color.r - (int) ((color_count*)p2)->color.r;
}

static int greencompare(const void *p1, const void *p2)
{
    return (int) ((color_count*)p1)->color.g - (int) ((color_count*)p2)->color.g;
}

static int bluecompare(const void *p1, const void *p2)
{
    return (int) ((color_count*)p1)->color.b - (int) ((color_count*)p2)->color.b;
}

struct box
{
    int index;
    int colors;
    int sum;
};

// Sort the boxes by their sum
static int sumcompare(const void *p1, const void *p2)
{
    return ((box *) p2)->sum - ((box *) p1)->sum;
}

//////////////////////////////////////////////////////////////////////
// Here is the fun part, the median-cut colormap generator. This is based
// on Paul Heckbert's paper "Color Image Quantization for Frame Buffer
// Display", SIGGRAPH '82 Proceedings, page 297.
void Quantizer::MedianCut(color_count* CHist, int num_colors_in_hist,
                          int sum, int maxval, int MaxColors)
{
    int bi, i;

    box *bv = new box[MaxColors]; // The bounding boxes for the colors that map to a given entry
    ASSERT_RM(bv, "memory alloc failed");

    memset(cmap, 0, MaxColors * sizeof(pixel));

    // Set up the initial box.
    bv[0].index = 0;
    bv[0].colors = num_colors_in_hist;
    bv[0].sum = sum;
    int boxes = 1;

    // Main loop: split boxes until we have enough.
    while (boxes < MaxColors)
    {
        int v;

        // Find the first splittable box.
        for(bi=0; bv[bi].colors<2 && bi<boxes; bi++)
            ;
        if(bi == boxes)
            break; // No splittable boxes.

        int indx = bv[bi].index;
        int clrs = bv[bi].colors;
        int sm = bv[bi].sum;

        // Go through the box finding the minimum and maximum of each
        // component - the boundaries of the box.
        int maxr, maxg, maxb;
        int minr = maxr = CHist[indx].color.r;
        int ming = maxg = CHist[indx].color.g;
        int minb = maxb = CHist[indx].color.b;

        for(i=1; i<clrs; i++) {
            v = CHist[indx + i].color.r;
            if(v < minr) minr = v;
            if(v > maxr) maxr = v;

            v = CHist[indx + i].color.g;
            if(v < ming) ming = v;
            if(v > maxg) maxg = v;

            v = CHist[indx + i].color.b;
            if(v < minb) minb = v;
            if(v > maxb) maxb = v;
        }

        // Find the largest dimension, and sort by that component.
        int rl = maxr - minr;
        int gl = maxg - ming;
        int bl = maxb - minb;

        if(rl >= gl && rl >= bl)
            qsort((char*) &(CHist[indx]), (size_t) clrs, sizeof(color_count), redcompare);
        else if(gl >= bl)
            qsort((char*) &(CHist[indx]), (size_t) clrs, sizeof(color_count), greencompare);
        else
            qsort((char*) &(CHist[indx]), (size_t) clrs, sizeof(color_count), bluecompare);

        // Now find the median based on the counts, so that about half the
        // pixels (not colors but pixels) are in each subdivision.
        int lowersum = CHist[indx].value;
        int halfsum = sm / 2;
        for(i=1; i<clrs-1; i++) {
            if(lowersum >= halfsum) break;
            lowersum += CHist[indx + i].value;
        }

        // Split the box, and sort all boxes to bring the biggest boxes to the top.
        bv[bi].colors = i;
        bv[bi].sum = lowersum;
        bv[boxes].index = indx + i;
        bv[boxes].colors = clrs - i;
        bv[boxes].sum = sm - lowersum;
        ++boxes;
        qsort((char*) bv, (size_t) boxes, sizeof(struct box), sumcompare);
    }

    /*
    ** Ok, we've got enough boxes. Now choose a representative color for
    ** each box. There are a number of possible ways to make this choice.
    ** One would be to choose the center of the box; this ignores any structure
    ** within the boxes. Another method would be to average all the colors in
    ** the box - this is the method specified in Heckbert's paper. A third
    ** method is to average all the pixels in the box. In other words, take a
    ** weighted average of the colors. This is what we do.
    */
    for(bi=0; bi<boxes; bi++) {
        int indx = bv[bi].index;
        int clrs = bv[bi].colors;
        DMCINT64 r = 0, g = 0, b = 0, sum = 0;

        for(i=0; i<clrs; i++) {
            r += CHist[indx + i].color.r * CHist[indx + i].value;
            g += CHist[indx + i].color.g * CHist[indx + i].value;
            b += CHist[indx + i].color.b * CHist[indx + i].value;
            sum += CHist[indx + i].value;
        }

        r = r / sum; if(r>maxval) r = maxval; /* avoid math errors */
        g = g / sum; if(g>maxval) g = maxval;
        b = b / sum; if(b>maxval) b = maxval;

        cmap[bi].r = byte(r);
        cmap[bi].g = byte(g);
        cmap[bi].b = byte(b);
    }

    delete [] bv;
}

//////////////////////////////////////////////////////////////////////
// There are four stages to the quantization.
// 1) Trivial finish if gray or few unique colors.
// 2) Median cut to get some good seed colors.
// 3) Refine quantization iteratively with weighted histogram from median cut.
// 4) Refine quantization iteratively using actual pixels.
byte *Quantizer::Quant(byte *pic24, int sz, int MaxCols, bool IsGray)
{
    MaxColors = MaxCols;
    size = sz;

    byte *pic8 = new byte[size];
    ASSERT_RM(pic8, "memory alloc failed");

    if(IsGray)
    {
        // 1) Trivial finish if gray or few unique colors.
#ifdef DMC_DEBUG
        cerr << "Gray.\n";
#endif
        // For gray scale, just do it.
        memcpy(pic8, pic24, size);
        NumColors = MaxColors;

        for(int i=0; i<NumColors; i++)
        {
            cmap[i].r = cmap[i].g = cmap[i].b = i;
        }

        ReduceColorMap(pic8);

        return pic8;
    }

    Pix = (pixel *) pic24;

    // 1) Trivial finish if gray or few unique colors.
    if(TrivialSolution(MaxColors, pic8))
        return pic8;

    // 2) Median cut to get some good seed colors.
#ifdef DMC_DEBUG
    cerr << "Making histogram.\n";
#endif

    int num_colors_in_hist;
    color_count *CHist = MakeHist(num_colors_in_hist);

#ifdef DMC_DEBUG
    cerr << num_colors_in_hist << " unique colors found; choosing " << MaxColors << " colors\n";
#endif

    // Apply median-cut to histogram, making the new colormap.
    byte maxval = 255;
    MedianCut(CHist, num_colors_in_hist, size, maxval, MaxColors);

#ifdef DMC_DEBUG
    cerr << "Finished median cut.\n";
#endif

    NumColors = MaxColors;
    // ReduceColorMap(pic8);

    // 3) Refine quantization iteratively with weighted histogram from median cut.
    RefineColorMapFast(CHist, num_colors_in_hist);

    // 4) Refine quantization iteratively using actual pixels.
    RefineColorMap(pic8);

    delete [] CHist;

    return pic8;
}

void Quantizer::ReplaceImage(const byte *pix8)
{
    for(int i=0; i<size; i++) {
        Pix[i] = cmap[pix8[i]];
    }
}
