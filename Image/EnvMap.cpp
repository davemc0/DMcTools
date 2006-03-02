//////////////////////////////////////////////////////////////////////
// ImageAlgorithms.cpp - Image filtering, sampling, etc.
//
// Copyright David K. McAllister, 2001.

// Also:
// Quant, Filter, PullPush need to be parameterized properly.

// Want to make these be templated functions, not class members.
// How to handle precision for fixed?

#include <Image/tImage.h>

#include <vector>
#include <algorithm>

using namespace std;

// Use for 0 <= x <= 1
static inline float CubicFilterN(float x)
{
    float x2 = x * x;
    return (7.0f/6.0f * x - 2.0f) * x2 + 8.0f/9.0f;
}

// Use for 1 <= x <= 2
static inline float CubicFilterF(float x)
{
    return ((-7.0f/18.0f * x + 2.0f) * x - 10.0f/3.0f) * x + 16.0f/9.0f;
}

// This is basically just here as an example.
static inline float CubicFilter(float x)
{
    x = fabs(x);
    if(x<1) return CubicFilterN(x);
    else return CubicFilterF(x);
}

// Return true if all subsamples are inside image.
// Otherwise return false and don't modify res.
// XXX These will need instantiations before use.

// Bicubic sample the image.
template<class _ImgType, class _PixType>
bool sample4(_PixType &res, const _ImgType &Img, const float x, const float y)
// The following doesn't work on SGI:
// bool sample4(const _ImgType &Img, const float x, const float y, _ImgType::PixType &res)
{
    int xl = int(x) - 1;
    int xh = xl + 4;
    if(xl < 0 || xh > Img.w() || xl > xh)
        return false;
    
    int yl = int(y) - 1;
    int yh = yl + 4;
    if(yl < 0 || yh > Img.h() || yl > yh) // The last is for wraparound of huge floats.
        return false;
    
    res = _PixType(0);
    float Cdx0 = CubicFilterF(x - float(xl));
    float Cdx1 = CubicFilterN(x - float(xl+1));
    float Cdx2 = CubicFilterN(float(xl+2) - x);
    float Cdx3 = CubicFilterF(float(xl+3) - x);
    
    float Cdy;
    Cdy = CubicFilterF(y - float(yl));
    res += Cdx0 * Cdy * static_cast<_PixType>(Img(xl, yl));
    res += Cdx1 * Cdy * static_cast<_PixType>(Img(xl+1, yl));
    res += Cdx2 * Cdy * static_cast<_PixType>(Img(xl+2, yl));
    res += Cdx3 * Cdy * static_cast<_PixType>(Img(xl+3, yl));
    
    Cdy = CubicFilterN(y - float(yl+1));
    res += Cdx0 * Cdy * static_cast<_PixType>(Img(xl, yl+1));
    res += Cdx1 * Cdy * static_cast<_PixType>(Img(xl+1, yl+1));
    res += Cdx2 * Cdy * static_cast<_PixType>(Img(xl+2, yl+1));
    res += Cdx3 * Cdy * static_cast<_PixType>(Img(xl+3, yl+1));
    
    Cdy = CubicFilterN(float(yl+2) - y);
    res += Cdx0 * Cdy * static_cast<_PixType>(Img(xl, yl+2));
    res += Cdx1 * Cdy * static_cast<_PixType>(Img(xl+1, yl+2));
    res += Cdx2 * Cdy * static_cast<_PixType>(Img(xl+2, yl+2));
    res += Cdx3 * Cdy * static_cast<_PixType>(Img(xl+3, yl+2));
    
    Cdy = CubicFilterF(float(yl+3) - y);
    res += Cdx0 * Cdy * static_cast<_PixType>(Img(xl, yl+3));
    res += Cdx1 * Cdy * static_cast<_PixType>(Img(xl+1, yl+3));
    res += Cdx2 * Cdy * static_cast<_PixType>(Img(xl+2, yl+3));
    res += Cdx3 * Cdy * static_cast<_PixType>(Img(xl+3, yl+3));
    
    return true;
}

template bool sample4(f3Pixel &res, const f3Image &Img, const float x, const float y);
//template bool sample4(uc1Pixel &res, const uc1Image &Img, const float x, const float y);
template bool sample4(float &res, const uc1Image &Img, const float x, const float y);

// Bilinear sample the image.
template<class _ImgType, class _PixType>
bool sample2(_PixType &res, const _ImgType &Img, const float x, const float y)
// The following doesn't work on SGI:
// bool sample2(const _ImgType &Img, const float x, const float y, _ImgType::PixType &res)
{
    int x0 = int(x);
    int y0 = int(y);
    
    if(x0<0 || (x0+1)>=Img.w() || y0<0 || (y0+1)>=Img.h())
        return false;
    
    float xt = x - float(x0); 
    float yt = y - float(y0);
    
    _PixType b00 = Img(x0,y0);
    _PixType b01 = Img(x0+1,y0);
    _PixType b10 = Img(x0,y0+1);
    _PixType b11 = Img(x0+1,y0+1);
    
    _PixType b0 = xt * (b01 - b00) + b00;
    _PixType b1 = xt * (b11 - b10) + b10;
    res = yt * (b1 - b0) + b0;
    
    return true;
}

// Constant sample the image.
template<class _ImgType, class _PixType>
bool sample1(_PixType &res, const _ImgType &Img, const float x, const float y)
// The following doesn't work on SGI:
// bool sample1(const _ImgType &Img, const float x, const float y, _PixType &res)
{
    int x0 = int(x+0.5f);
    int y0 = int(y+0.5f);
    
    if(x0<0 || x0>=Img.w() || y0<0 || y0>=Img.h())
        return false;
    
    res = Img(x0,y0);
    
    return true;
}

template bool sample1(f3Pixel &res, const f3Image &Img, const float x, const float y);

// Map a float image to an unsigned char image using log scale.
template<class OutImgType, class InImgType, class ElType>
void ToneMapLog(OutImgType &Out, const InImgType &Img,
                const ElType Scale, const ElType Bias, const ElType LogBias)
{
    ASSERT0(typeid(InImgType::PixType::ElType) == typeid(ElType));
    
    Out.SetSize(Img.w(), Img.h());
    
    for(int i=0; i<Img.size(); i++) {
        //cerr << Img[i] << '\t';
        InImgType::PixType tmp = Scale * func(Img[i], logf);
        tmp += Bias;
        //cerr << tmp << '\t';
        Out[i] = static_cast<OutImgType::PixType>(tmp);
        //cerr << Out[i] << endl;
    }
}

template void ToneMapLog(uc1Image &Out, const f1Image &Img,
                         const float Scale, const float Bias, const float LogBias);
template void ToneMapLog(uc3Image &Out, const f3Image &Img,
                         const float Scale, const float Bias, const float LogBias);
template void ToneMapLog(uc1Image &Out, const f3Image &Img,
                         const float Scale, const float Bias, const float LogBias);

// Map a float image to an unsigned char image.
template<class OutImgType, class InImgType, class ElType>
void ToneMapLinear(OutImgType &Out, const InImgType &Img, const ElType Scale, const ElType Bias)
{
    ASSERT0(typeid(InImgType::PixType::ElType) == typeid(ElType));
    
    Out.SetSize(Img.w(), Img.h());
    
    for(int i=0; i<Img.size(); i++) {
        InImgType::PixType tmp = Scale * Img[i];
        tmp += Bias;
        Out[i] = static_cast<OutImgType::PixType>(tmp);
    }
}

// This instantiation is touchy. Your code has to really give float args.
template void ToneMapLinear(uc1Image &Out, const f1Image &Img,
                                const float Scale, const float Bias);
template void ToneMapLinear(uc3Image &Out, const f3Image &Img,
                                const float Scale, const float Bias);
template void ToneMapLinear(uc1Image &Out, const f3Image &Img,
                                const float Scale, const float Bias);

// Map a float image to an unsigned char image, given the extrema that map to 0..255.
template<class OutImgType, class InImgType, class InPixType>
void ToneMapExtrema(OutImgType &Out, const InImgType &Img,
                    const InPixType &MinP, const InPixType &MaxP)
{
    ASSERT0(typeid(InImgType::PixType) == typeid(InPixType));
    
    float minc = MinP.min_chan();
    float maxc = MaxP.max_chan();
    
    float Scale = 1.0f / (maxc - minc);
    float Bias = -(minc * Scale);
    
    ToneMapLinear(Out, Img, Scale, Bias);
}

template void ToneMapExtrema(uc1Image &Out, const f1Image &Img,
                             const f1Pixel &MinP, const f1Pixel &MaxP);
template void ToneMapExtrema(uc3Image &Out, const f3Image &Img,
                             const f3Pixel &MinP, const f3Pixel &MaxP);
template void ToneMapExtrema(uc1Image &Out, const f3Image &Img,
                             const f3Pixel &MinP, const f3Pixel &MaxP);

// Map a float image to an unsigned char image, finding the extrema that map to 0..255.
template<class OutImgType, class InImgType>
void ToneMapFindExtrema(OutImgType &Out, const InImgType &Img)
{
    InImgType::PixType MinP, MaxP;
    Img.GetMinMax(MinP, MaxP);
    
    ToneMapExtrema(Out, Img, MinP, MaxP);
}

template void ToneMapFindExtrema(uc1Image &Out, const f1Image &Img);
template void ToneMapFindExtrema(uc3Image &Out, const f3Image &Img);
template void ToneMapFindExtrema(uc1Image &Out, const f3Image &Img);

// Clamp each pixel to the extrema of its neighbors.
template <class _ImgType>
void DeSpeckle(_ImgType &Img)
{
    for(int y=1; y<Img.h()-1; y++) {
        for(int x=1; x<Img.w()-1; x++) {
            _ImgType::PixType MinP = Img(x-1,y-1), MaxP = Img(x-1,y-1); 
            MinP = Min(MinP,Img(x,y-1)); MaxP = Max(MaxP,Img(x,y-1)); 
            MinP = Min(MinP,Img(x+1,y-1)); MaxP = Max(MaxP,Img(x+1,y-1)); 
            MinP = Min(MinP,Img(x-1,y)); MaxP = Max(MaxP,Img(x-1,y)); 
            MinP = Min(MinP,Img(x+1,y)); MaxP = Max(MaxP,Img(x+1,y)); 
            MinP = Min(MinP,Img(x-1,y+1)); MaxP = Max(MaxP,Img(x-1,y+1)); 
            MinP = Min(MinP,Img(x,y+1)); MaxP = Max(MaxP,Img(x,y+1)); 
            MinP = Min(MinP,Img(x+1,y+1)); MaxP = Max(MaxP,Img(x+1,y+1)); 
            Img(x,y) = Clamp(MinP, Img(x,y),MaxP);
        }
    }
}

// Instantiate it.
template void DeSpeckle(f1Image &Img);
template void DeSpeckle(uc1Image &Img);

// If N pixels are <= me, I keep my value, else I go to MaxP.
template <class _ImgType>
void DeSpeckleN(_ImgType &Img, int N)
{
    _ImgType NImg(Img.w(), Img.h()); // The 255 is a hack.
    NImg.fill(uc1Pixel(255));

    for(int y=1; y<Img.h()-1; y++) {
        for(int x=1; x<Img.w()-1; x++) {
            _ImgType::PixType MinP = Img(x-1,y-1), MaxP = Img(x-1,y-1); 
            MinP = Min(MinP,Img(x,y-1)); MaxP = Max(MaxP,Img(x,y-1)); 
            MinP = Min(MinP,Img(x+1,y-1)); MaxP = Max(MaxP,Img(x+1,y-1)); 
            MinP = Min(MinP,Img(x-1,y)); MaxP = Max(MaxP,Img(x-1,y)); 
            MinP = Min(MinP,Img(x+1,y)); MaxP = Max(MaxP,Img(x+1,y)); 
            MinP = Min(MinP,Img(x-1,y+1)); MaxP = Max(MaxP,Img(x-1,y+1)); 
            MinP = Min(MinP,Img(x,y+1)); MaxP = Max(MaxP,Img(x,y+1)); 
            MinP = Min(MinP,Img(x+1,y+1)); MaxP = Max(MaxP,Img(x+1,y+1)); 

            _ImgType::PixType Me = Img(x,y);
            int c=0;
            c += Img(x-1,y-1) <= Me; 
            c += Img(x,y-1) <= Me; 
            c += Img(x+1,y-1) <= Me; 
            c += Img(x-1,y) <= Me; 
            c += Img(x+1,y) <= Me; 
            c += Img(x-1,y+1) <= Me; 
            c += Img(x,y+1) <= Me; 
            c += Img(x+1,y+1) <= Me; 
            NImg(x,y) = c>=N?Me:MaxP;
        }
    }

    Img = NImg;
}

// Instantiate it.
template void DeSpeckleN(uc1Image &Img, int N);

// Return an approximation of the median luminance.
template <class _RetType, class _ImgType>
_RetType getApproxMedian(const _ImgType &Img, const int NumSamples)
{
    _ImgType::PixType::ElType *vals = new _ImgType::PixType::ElType[NumSamples];
    
    for(int k=0; k<NumSamples; k++) {
        int i = LRand()%Img.size();
        vals[k] = Img[i].Luminance();
    }
    
    sort(&vals[0], &vals[NumSamples]);
    
    _ImgType::PixType::ElType ret = vals[NumSamples/2];
    delete [] vals;
    
    return ret;
}

template float getApproxMedian(const f3Image &Img, const int NumSamples);

// Quantize all the pixels into buckets (separately for each channel)
// and return a histogram for each channel.
// Values beyond minc and maxc are not counted.
// You define a return type that is usually a uiPixel w/ as many channels as the image.
// For standard ucImage, use NumBuckets=256, minc=0, maxc=256.
template <class _PixType, class _ImgType, class _ElType>
vector<_PixType> getHistogram(const _ImgType &Img, const int NumBuckets,
                              const _ElType minc, const _ElType maxc)
{
    vector<_PixType> Hist(NumBuckets, _PixType(0));
 
    float scale = float(NumBuckets) / float(maxc - minc);
    for(int k=0; k<Img.size(); k++) {
        for(int c=0; c<Img.chan(); c++) {
            int bucket = int(float(Img[k][c] - minc) * scale);
            if(bucket >= 0 && bucket < NumBuckets)
                Hist[bucket][c]++;
        }
    }

    return Hist;
}

// When calling, make sure the last two args are actual floats.
template vector<ui3Pixel> getHistogram(const us3Image &Img, const int NumBuckets,
                            const float minc, const float maxc);

template vector<ui3Pixel> getHistogram(const f3Image &Img, const int NumBuckets,
                            const float minc, const float maxc);

template vector<ui1Pixel> getHistogram(const us1Image &Img, const int NumBuckets,
                            const float minc, const float maxc);

template vector<ui3Pixel> getHistogram(const uc3Image &Img, const int NumBuckets,
                            const float minc, const float maxc);

template vector<ui1Pixel> getHistogram(const uc1Image &Img, const int NumBuckets,
                            const float minc, const float maxc);

// Cheezy box filter for MIP level generation, etc.
template <class _ImgType>
void Downsample2(_ImgType &Out, const _ImgType &Img)
{
    int w1 = (1+Img.w())/2; // Output image size
    int h1 = (1+Img.h())/2;

    Out.SetSize(w1, h1);

    int ws = Img.w() / 2; // Number of times through simple loop.
    int hs = Img.h() / 2;
    
    for(int y=0; y<hs; y++) {
        for(int x=0; x<ws; x++) {
            Out(x,y) = 0.25f * (Img((x<<1),(y<<1)) + Img((x<<1)+1,(y<<1)) + Img((x<<1),(y<<1)+1) + Img((x<<1)+1,(y<<1)+1));
        }
        if(w1 != ws)
            Out(ws,y) = 0.5f * (Img((ws<<1),(y<<1)) + Img((ws<<1),(y<<1)+1));
    }
    if(h1 != hs) {
        for(int x=0; x<ws; x++) {
            Out(x,hs) = 0.5f * (Img((x<<1),(hs<<1)) + Img((x<<1)+1,(hs<<1)));
        }
        if(w1 != ws)
            Out(ws,hs) = Img((ws<<1),(hs<<1));
    }
}

template void Downsample2(f3Image &Out, const f3Image &Img);

// Bi-cubic upsampling to arbitrary size.
template <class _ImgType>
void Upsample4(_ImgType &Out, const _ImgType &Img, const int w1, const int h1)
{
    Out.SetSize(w1, h1);

    float xs = (Img.w() - 1) / float(w1-1);
    float ys = (Img.h() - 1) / float(h1-1);

    float yf = 0;
    for(int y=0; y<h1; y++) {
        float xf = 0;
        for(int x=0; x<w1; x++) {
            bool good = sample4(Out(x,y), Img, xf, yf);
            if(!good)
                sample1(Out(x,y), Img, floorf(xf), floorf(yf));
            xf += xs;
        }
        yf += ys;
    }
}

template void Upsample4(f3Image &Out, const f3Image &Img, const int w1, const int h1);

#if 0
template <class _ImgType>
void Blur(_ImgType &Img, const _ImgType::PixType::MathType stdev)
{
    double *Kernel = MakeBlurKernel(5, stdev);
    float *F = FPix();
    
    float *N = new float[size];
    ASSERTERR(N, "memory alloc failed");
    
    // Just don't blur the edges.
    memcpy(N, F, wid * sizeof(float));
    memcpy(&N[wid], &F[wid], wid * sizeof(float));
    memcpy(&N[(hgt-2)*wid], &F[(hgt-2)*wid], wid * sizeof(float));
    memcpy(&N[(hgt-1)*wid], &F[(hgt-1)*wid], wid * sizeof(float));
    
    for(int y=2; y<hgt-2; y++)
    {
        N[y*wid] = F[y*wid];
        N[y*wid+1] = F[y*wid+1];
        N[(y+1)*wid-1] = F[(y+1)*wid-1];
        N[(y+1)*wid-2] = F[(y+1)*wid-2];
        
        for(int x=2; x<wid-2; x++)
        {
            double G = 0;
            
            G += Kernel[0] * F[(y-2)*wid+x-2];
            G += Kernel[1] * F[(y-2)*wid+x-1];
            G += Kernel[2] * F[(y-2)*wid+x];
            G += Kernel[3] * F[(y-2)*wid+x+1];
            G += Kernel[4] * F[(y-2)*wid+x+2];
            
            G += Kernel[5] * F[(y-1)*wid+x-2];
            G += Kernel[6] * F[(y-1)*wid+x-1];
            G += Kernel[7] * F[(y-1)*wid+x];
            G += Kernel[8] * F[(y-1)*wid+x+1];
            G += Kernel[9] * F[(y-1)*wid+x+2];
            
            G += Kernel[10] * F[(y)*wid+x-2];
            G += Kernel[11] * F[(y)*wid+x-1];
            G += Kernel[12] * F[(y)*wid+x];
            G += Kernel[13] * F[(y)*wid+x+1];
            G += Kernel[14] * F[(y)*wid+x+2];
            
            G += Kernel[15] * F[(y+1)*wid+x-2];
            G += Kernel[16] * F[(y+1)*wid+x-1];
            G += Kernel[17] * F[(y+1)*wid+x];
            G += Kernel[18] * F[(y+1)*wid+x+1];
            G += Kernel[19] * F[(y+1)*wid+x+2];
            
            G += Kernel[20] * F[(y+2)*wid+x-2];
            G += Kernel[21] * F[(y+2)*wid+x-1];
            G += Kernel[22] * F[(y+2)*wid+x];
            G += Kernel[23] * F[(y+2)*wid+x+1];
            G += Kernel[24] * F[(y+2)*wid+x+2];
            
            N[y*wid+x] = G;
        }
    }
    
    delete [] Pix;
    Pix = (unsigned char *) N;
    
    delete [] Kernel;
}
#endif
