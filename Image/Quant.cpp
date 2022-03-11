//////////////////////////////////////////////////////////////////////
// Quant.cpp - Quantize color images to MaxColors colors for saving
//
// Copyright David K. McAllister, Mar. 1999 and June 2007

#include "Image/Quant.h"

#include "Math/Random.h"
#include "Util/Assert.h"

#include <algorithm>
#include <functional>
#include <iostream>

// See if there are <= MaxColors unique colors. If so, fill in IndexImg and return true
template <class Pixel_T, class Index_T> bool Quantizer<Pixel_T, Index_T>::TrivialSolution()
{
    for (size_t y = 0; y < size && CMap.size() <= QP.maxColorPalette; y++) {
        bool FoundIt = false;
        for (size_t i = 0; i < CMap.size(); i++) {
            if (Pix[y] == CMap[i]) {
                FoundIt = true;
                IndexImg[y] = Index_T(i);
                break;
            }
        }

        if (!FoundIt) {
            if (CMap.size() >= QP.maxColorPalette) return false;

            // Add a new color map entry
            IndexImg[y] = (Index_T)CMap.size();
            CMap.C.push_back(Pix[y]);
        }
    }

    return true;
}

// Reduce the color map by computing the set of unique colors.
// Also overwrite IndexImg with the new indices.
template <class Pixel_T, class Index_T> void Quantizer<Pixel_T, Index_T>::ReduceColorMap()
{
    std::vector<bool> IsUsed(QP.maxColorPalette, false);
    std::vector<Index_T> pc2nc(QP.maxColorPalette, 0);
    ColorMap<Pixel_T> CMap2;

    // Scan image and flag which colormap entries were actually used
    for (size_t i = 0; i < size; i++) { IsUsed[IndexImg[i]] = true; }

    // Reduce color map by removing unused and duplicate colors
    for (size_t i = 0; i < CMap.size(); i++) {
        // See if color number i is already used
        size_t j;
        for (j = 0; j < i; j++) {
            if (CMap[i] == CMap[j]) break;
        }

        // If the color is unique and used, add it
        if (j == i && IsUsed[i]) { // Wasn't found
            pc2nc[i] = (Index_T)CMap2.size();
            CMap2.C.push_back(CMap[i]);
        } else
            pc2nc[i] = pc2nc[j];
    }

    // Remap index image pixels
    for (size_t i = 0; i < size; i++) { IndexImg[i] = pc2nc[IndexImg[i]]; }

    CMap = CMap2;
}

// Centroids is the sums created when looping over all the pixels and the count.
// Here we simply divide the centroid sums by the counts to make a color value in the MathPixType,
// then convert from the centroid's MathPixType to the image's and color map's Pixel_T.
template <class Pixel_T, class Index_T> void Quantizer<Pixel_T, Index_T>::CentroidsToColorMap(const std::vector<CountedPixel<MathPixType>>& Centroids)
{
    CMap.C.resize(0);
    for (size_t i = 0; i < Centroids.size(); i++) {
        if (Centroids[i].count > 0) { CMap.C.push_back(static_cast<Pixel_T>(Centroids[i].color / Centroids[i].count)); }
    }

    // If there were unused centroids, set them randomly or to the value of the worst pixel.
    for (size_t i = CMap.size(); i < QP.maxColorPalette; i++) {
#ifdef DMC_DEBUG
        std::cerr << "R";
#endif
        CMap.C.push_back(worstPixInd);
    }
}

// Map the TrueColor image to an Index_T image by assigning each pixel to the index of the closest color map entry. No dither.
// Output: Centroids is how many pixels chose each entry and the color sums for later computing the centroids.
// Returns amount of numerical error.
template <class Pixel_T, class Index_T>
typename Quantizer<Pixel_T, Index_T>::MathType Quantizer<Pixel_T, Index_T>::Image3to1(std::vector<CountedPixel<MathPixType>>& Centroids)
{
    MathType Error = 0;
    Centroids.clear();
    Centroids.resize(QP.maxColorPalette);

    worstPixInd = Pix[0]; // Value of the pixel that has the most error
    worstErr = 0;

    // Set each pixel to closest color in color map
    // TODO: Parallelize this loop; use atomics
    for (size_t y = 0; y < size; y++) {
        // TODO: Build BVH over color map entries and do log n search for closest
        MathType BestErr = std::numeric_limits<MathType>::max();
        Index_T BestC = 0;
        for (size_t i = 0; i < CMap.size(); i++) {
            MathType Err = DiffSqr(Pix[y], CMap[i]);
            if (Err < BestErr) {
                BestErr = Err;
                BestC = Index_T(i);
            }
        }

        IndexImg[y] = BestC;
        Error += BestErr;

        if (BestErr > worstErr) {
            worstErr = BestErr;
            worstPixInd = Pix[y];
        }

        if (QP.makeArtisticPalette) BestErr = 1; // Weight by distance, so farther ones don't get forgotten
        Centroids[BestC].count += BestErr;
        Centroids[BestC].color += MathPixType(Pix[y]) * BestErr;
    }

    ASSERT_R(CMap.size() - 1 <= (size_t)std::numeric_limits<Index_T>::max());

    return Error;
}

// Given a histogram (a list of all the quantized colors and their frequency),
// find the color map entry that is closest to each histogram entry.
// Output: Centroids is how many pixels chose each entry and the color sums for later computing the centroids.
// Returns amount of numerical error.
template <class Pixel_T, class Index_T>
typename Quantizer<Pixel_T, Index_T>::MathType Quantizer<Pixel_T, Index_T>::Image3to1Fast(const std::vector<CountedPixel<Pixel_T>>& CHist,
                                                                                          std::vector<CountedPixel<MathPixType>>& Centroids)
{
    MathType Error = 0;
    Centroids.clear();
    Centroids.resize(QP.maxColorPalette);

    worstPixInd = Pix[0]; // This is the index of the pixel that has the most error.
    worstErr = 0;

    // Set each histogram entry to the closest color in color map.
    for (size_t y = 0; y < CHist.size(); y++) {
        MathType BestErr = std::numeric_limits<MathType>::max();
        Index_T BestC = 0;
        for (size_t i = 0; i < CMap.size(); i++) {
            MathType Err = DiffSqr(CHist[y].color, CMap[i]);
            if (Err < BestErr) {
                BestErr = Err;
                BestC = static_cast<Index_T>(i);
            }
        }

        Error += BestErr * CHist[y].count;
        if (QP.makeArtisticPalette) {
            // Weight by distance, so farther ones don't get forgotten
            // To really make sure less common colors receive a color map entry,
            // don't weight the hist bucket's contribution to the centroid's color by the number of pixels in the hist bucket.
            // Then when the pixels have their final assignment to a centroid
            // Could also neglect any pixel whose value is a linear combination of two centroids since that's an AA boundary.
            Centroids[BestC].count += CHist[y].count * BestErr;
            MathType Wgt = CHist[y].count * BestErr;
            Centroids[BestC].color += static_cast<MathPixType>(CHist[y].color) * Wgt;
        } else {
            Centroids[BestC].count += CHist[y].count;
            MathType Wgt = CHist[y].count;
            Centroids[BestC].color += static_cast<MathPixType>(CHist[y].color) * Wgt;
        }

        if (BestErr > worstErr) {
            worstErr = BestErr;
            worstPixInd = CHist[y].color;
        }
    }

    return Error;
}

// Create and iteratively refine colormap to reduce its error
// If Fast is true, this operates on a histogram of the image, otherwise it operates on the actual pixels and ignores CHist.
template <class Pixel_T, class Index_T>
typename Quantizer<Pixel_T, Index_T>::MathType Quantizer<Pixel_T, Index_T>::RefineColorMap(const bool Fast, const std::vector<CountedPixel<Pixel_T>>& CHist)
{
    std::vector<CountedPixel<MathPixType>> Centroids;

    // Compute best fit to CMap.
    MathType OldError = std::numeric_limits<MathType>::max();
    MathType Error = OldError;
    int maxIt = Fast ? QP.maxItersFast : QP.maxIters;

    for (int iter = 0; iter < maxIt && (iter == 0 || (Error + QP.targetErr < OldError)); iter++) {
        OldError = Error;
        if (Fast)
            Error = Image3to1Fast(CHist, Centroids);
        else
            Error = Image3to1(Centroids);

        CentroidsToColorMap(Centroids);

        //#ifdef DMC_DEBUG
        fprintf(stderr, "%03d RefineColorMap Error = %f Fast=%d\n", iter, float(Error) / float(size), (int)Fast);
        //#endif
    }

    return Error;
}

//////////////////////////////////////////////////////////////////////
// Median Cut Color Quantization Algorithm
//////////////////////////////////////////////////////////////////////

// Returns a hash value for the pixel value (for integer pixel elements)
template <class Pixel_T> int PixelHash(const Pixel_T& pp, const int keepBits)
{
    const int total = std::numeric_limits<typename Pixel_T::ElType>::digits;
    const int killBits = total - keepBits;
    int ind = 0;
    for (int i = 0; i < Pixel_T::Chan; i++) ind = (ind << keepBits) | (pp[i] >> killBits);
    return ind;
}

// Returns a hash value for the pixel value (for float pixel elements)
template <> int PixelHash(const f3Pixel& pp, const int keepBits)
{
    ASSERT_R(0);             // XXX This isn't very good. Need better float hash.
    f3Pixel tmp = pp * 0.5f; // Shrink it down a bit and fit it into the range
    ui3Pixel ui(tmp);
    return PixelHash(ui, keepBits);
}

// Make an associative histogram of pixel colors by chopping the lsbs of the color values to map similar colors to a histogram entry
template <class Pixel_T, class Index_T> void Quantizer<Pixel_T, Index_T>::MakeHist(std::vector<CountedPixel<Pixel_T>>& CHist)
{
#ifdef DMC_DEBUG
    std::cerr << "Making histogram.\n";
#endif

    int keepBits = QP.makeArtisticPalette ? 4 : 5;

    CHist.clear();
    CHist.resize(1ull << (Chan * keepBits));

    // Go through the entire image, building a hash table of colors.
    for (size_t i = 0; i < size; i++) {
        int ind = PixelHash(Pix[i], keepBits);

        if (CHist[ind].count == 0) { CHist[ind].color = Pix[i]; }
        CHist[ind].count++;
    }

    // Partition the table to put empty elements at the end.
    // Linear time: Move a pointer up from 0 and when it finds a gap,
    // move a pointer down from the end until it can find one to fill the gap.
    size_t top = CHist.size() - 1;
    for (size_t i = 0; i < top; i++) {
        if (CHist[i].count == 0) {
            for (; top > i; top--) {
                if (CHist[top].count) {
                    CHist[i] = CHist[top];
                    CHist[top].count = 0;
                    break;
                }
            }
        }
    }

    CHist.resize(top); // Chop it down to only the ones that are used

    if (QP.makeArtisticPalette) {
        // We want all used colors to be of equal importance when making an artistic palette so that highlights come out right.
        for (size_t i = 0; i < CHist.size(); i++) CHist[i].count = 1;
    }

#ifdef DMC_DEBUG
    std::cerr << CHist.size() << " unique colors found.\n";
#endif
}

// A box points to a span of HistEntries and represents a box in color space.
struct Box {
    size_t index;    // Index of first histogram entry in this box
    size_t HECount;  // How many histogram entries are in this box
    size_t PixCount; // How many pixels are in this box
};

// The median-cut colormap generator. This is based on Paul Heckbert's paper
// "Color Image Quantization for Frame Buffer Display", SIGGRAPH '82 Proceedings, page 297.
template <class Pixel_T, class Index_T> void Quantizer<Pixel_T, Index_T>::MedianCut(std::vector<CountedPixel<Pixel_T>>& CHist)
{
    std::vector<Box> Boxes; // The bounding boxes for the HECount that map to a given entry

    // Set up the initial box
    Box b;
    b.index = 0;
    b.HECount = CHist.size();
    b.PixCount = size; // Pixels in image
    Boxes.push_back(b);

    // Main loop: split boxes until we have enough
    while (Boxes.size() < QP.maxColorPalette) {
        // Find the first splittable Box
        size_t bi;
        for (bi = 0; bi < Boxes.size() && Boxes[bi].HECount < 2; bi++)
            ;
        if (bi == Boxes.size()) break; // No splittable boxes

        // Go through the Box finding the minimum and maximum of each component - the boundaries of the box
        size_t ind = Boxes[bi].index;
        size_t HECnt = Boxes[bi].HECount;
        size_t PixCnt = Boxes[bi].PixCount;

        Pixel_T MinP = CHist[ind].color;
        Pixel_T MaxP = CHist[ind].color;
        for (size_t i = 1; i < HECnt; i++) {
            MinP = Min(MinP, CHist[ind + i].color);
            MaxP = Max(MaxP, CHist[ind + i].color);
        }

        // Find the index of the largest dimension, and sort by that component.
        Pixel_T Dif = MaxP - MinP;

        int MaxChan = 0;
        ElType MaxV = Dif[0];
        for (int c = 1; c < Chan; c++) {
            if (Dif[c] > MaxV) {
                MaxChan = c;
                MaxV = Dif[c];
            }
        }

        auto PixCh_less = [&](const CountedPixel<Pixel_T>& a, const CountedPixel<Pixel_T>& b) {
            if (a.color[MaxChan] != b.color[MaxChan])
                return a.color[MaxChan] < b.color[MaxChan];
            else
                return a.count > b.count;
        };

        std::sort(&CHist[ind], (&CHist[ind + HECnt - 1]) + 1, PixCh_less);

        // Now find the median based on the counts, so that about half the PixCount (not HECount) is in each subdivision.
        size_t lowersum = (size_t)CHist[ind].count;
        size_t halfsum = PixCnt / 2;
        size_t i;
        for (i = 1; i < HECnt - 1; i++) {
            if (lowersum >= halfsum) break;
            lowersum += (size_t)CHist[ind + i].count;
        }

        // Split the Box, and sort all boxes to bring the biggest boxes to the top
        Boxes[bi].HECount = i;
        Boxes[bi].PixCount = lowersum;

        Box b;
        b.index = ind + i;
        b.HECount = HECnt - i;
        b.PixCount = PixCnt - lowersum;
        Boxes.push_back(b);

        // Sort the boxes by their PixCount
        auto PixCount_greater = [](const Box& p1, const Box& p2) { return p1.PixCount > p2.PixCount; };

        sort(Boxes.begin(), Boxes.end(), PixCount_greater);
    }

    // Ok, we have enough boxes. Now choose a representative color for each box. There are many ways to make this choice:
    // 1) Choose the center of the box; this ignores any structure within the boxes.
    // 2) Average all the hist entries in the box - this is the method specified in Heckbert's paper.
    // 3) Average all the pixels in the box. In other words, take a weighted average of the hist entries. This is what we do.
    std::vector<CountedPixel<MathPixType>> Centroids;
    Centroids.resize(QP.maxColorPalette);

    for (size_t bi = 0; bi < Boxes.size(); bi++) {
        size_t heind = Boxes[bi].index;
        for (size_t i = 0; i < Boxes[bi].HECount; i++) {
            MathPixType MP = static_cast<MathPixType>(CHist[heind + i].color);
            MathType C = static_cast<MathType>(CHist[heind + i].count);
            Centroids[bi].color += MP * C;
            Centroids[bi].count += C;
        }
    }

    CentroidsToColorMap(Centroids);

#ifdef DMC_DEBUG
    std::cerr << "Finished median cut.\n";
#endif
}

//////////////////////////////////////////////////////////////////////
// Stages of quantization:
// 0) Trivial finish if one-channel
// 1) Trivial finish if few enough unique colors
// 2) Make histogram and apply median cut to get seed colors
// 3) Refine quantization iteratively with weighted histogram from median cut
// 4) Refine quantization iteratively using actual pixels
//
// TODO: Its simpler and often faster to resize the image to something small first
// TODO: Use random pixels as initial color map
template <class Pixel_T, class Index_T> void Quantizer<Pixel_T, Index_T>::DoQuant()
{
    if (IndexImg == NULL) IndexImg = new Index_T[size];
    ASSERT_RM(IndexImg, "memory alloc failed");

    // 0) Trivial finish if one-channel
    if (isOneChan) {
#ifdef DMC_DEBUG
        std::cerr << "isOneChan\n";
#endif
        // For one channel, just copy it.
        memcpy(IndexImg, Pix, size);

        // Make a gray color map
        CMap.C.resize(QP.maxColorPalette);
        for (size_t i = 0; i < QP.maxColorPalette; i++) { CMap[i] = Pixel_T((ElType)i); }

        ReduceColorMap();

        return;
    }

    // 1) Trivial finish if few unique colors
    if (TrivialSolution()) return;

    // 2) Make histogram and apply median cut to get seed colors
    std::vector<CountedPixel<Pixel_T>> CHist;
    MakeHist(CHist);

    // Apply median-cut to histogram, making the new colormap
    MedianCut(CHist);

    // 3) Refine quantization iteratively with weighted histogram from median cut
    RefineColorMap(true, CHist);

    // 4) Refine quantization iteratively using actual pixels
    RefineColorMap(false, CHist);
}

// Allocate and return an IndexImg. Caller must delete IndexImg.
template <class Pixel_T, class Index_T> Index_T* Quantizer<Pixel_T, Index_T>::GetIndexImage()
{
    if (CMap.size() == 0) DoQuant();
    Index_T* TheIndexImg = IndexImg;
    IndexImg = NULL; // Swap it out
    return TheIndexImg;
}
template unsigned char* Quantizer<uc3Pixel, unsigned char>::GetIndexImage();

// Return the generated color map
template <class Pixel_T, class Index_T> ColorMap<Pixel_T>& Quantizer<Pixel_T, Index_T>::GetColorMap()
{
    if (CMap.size() == 0) DoQuant();
    return CMap;
}
template ColorMap<uc3Pixel>& Quantizer<uc3Pixel, unsigned char>::GetColorMap();

// Fill NewTrueColorImg based on CMap's colors and used IndexImg
template <class Pixel_T, class Index_T> void Quantizer<Pixel_T, Index_T>::GetQuantizedTrueColorImage(Pixel_T* NewTrueColorImg)
{
    std::cerr << CMap.size() << std::endl;
    if (CMap.size() == 0 || IndexImg == NULL) DoQuant();
    for (size_t i = 0; i < size; i++) {
        // std::cerr << i << " " << int(IndexImg[i]) << std::endl;
        NewTrueColorImg[i] = CMap[IndexImg[i]];
    }
}
template void Quantizer<uc3Pixel, unsigned char>::GetQuantizedTrueColorImage(uc3Pixel* NewTrueColorImg);

// Does MakeHist and returns the fraction of histogram boxes that had pixels
template <class Pixel_T, class Index_T> float Quantizer<Pixel_T, Index_T>::GetHistogramCount()
{
    std::vector<CountedPixel<Pixel_T>> CHist;
    MakeHist(CHist);

    int keepBits = QP.makeArtisticPalette ? 4 : 5;
    int maxHistEntries = 1 << (Chan * keepBits);
    return CHist.size() / float(maxHistEntries);
}
template float Quantizer<uc3Pixel, unsigned char>::GetHistogramCount();
