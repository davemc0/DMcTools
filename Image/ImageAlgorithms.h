//////////////////////////////////////////////////////////////////////
// ImageAlgorithms.h - Image filtering, sampling, etc.
//
// Copyright David K. McAllister, 2001.

#ifndef dmc_imageAlgorithms_h
#define dmc_imageAlgorithms_h

#include "Image/tImage.h"

#include <vector>

// Cheezy box filter for MIP level generation, etc.
template <class Image_T>
void Downsample2x2(Image_T &Out, const Image_T &Img);

// Arbitrary downsampling.
// Handles small aspect ratio changes, but undersamples longer direction if too big of a ratio change.
template <class Image_T>
void Downsample(Image_T &Out, const Image_T &Img, const int w1, const int h1);

// Bi-cubic resampling to arbitrary size.
// Works well for upsampling and downsampling by a factor less than two.
// Doesn't work for downsampling by a factor bigger than two.
// Instead call Downsample().
template <class Image_T>
void Resample4(Image_T &Out, const Image_T &Img, const int w, const int h);

// Map a float image to an unsigned char image.
template<class OutImage_T, class InImage_T>
void ToneMapLinear(OutImage_T &Out, const InImage_T &Img, const typename InImage_T::PixType::ElType Scale, const typename InImage_T::PixType::ElType Bias);

// Map a float image to an unsigned char image, given the extrema that map to 0..255.
template<class OutImage_T, class InImage_T>
void ToneMapExtrema(OutImage_T &Out, const InImage_T &Img, const typename InImage_T::PixType &MinP, const typename InImage_T::PixType &MaxP);

// Map a float image to an unsigned char image, finding the extrema that map to 0..255.
template<class OutImage_T, class InImage_T>
void ToneMapFindExtrema(OutImage_T &Out, const InImage_T &Img);

// Clamp each pixel to the extrema of its neighbors.
template <class Image_T>
void DeSpeckle(Image_T &Img);

template <class Image_T>
void DeSpeckleN(Image_T &Img, int N);

// Return an approximation of the median luminance.
template <class Image_T>
typename Image_T::PixType::ElType getApproxMedian(const Image_T &Img, const int NumSamples);

// Quantize all the pixels into buckets (separately for each channel)
// and return a histogram for each channel.
// Values beyond minc and maxc are not counted.
// You define a return type that is usually a uiPixel w/ as many channels as the image.
template <class Pixel_T, class Image_T, class Elem_T>
std::vector<Pixel_T> getHistogram(const Image_T &Img, const int NumBuckets, const Elem_T minc, const Elem_T maxc);

// Copy channel number src_ch of image SrcIm to channel dest_ch of image DstIm.
// Things work fine if SrcIm is DstIm.
// Have to template it on the incoming pixel type so that it can be a different number of channels than the output image.
// Converts from src channel format to dest using channel_cast.
// Channel numbers start with 0.
template<class DstPixel_T, class SrcPixel_T>
void CopyChan(tImage<DstPixel_T> &DstIm, const int dst_ch, const tImage<SrcPixel_T> &SrcIm, const int src_ch);

// Copy a rectangle from SrcIm of size bwid x bhgt with upper-right
// corner srcx,srcy to upper-right corner dstx,dsty in DstIm.
// The two images may be different types.
// The images may be the same, but the result is undefined if the quads overlap.
// Doesn't resize the dest image to accomodate the quad.
template<class DstPixel_T, class SrcPixel_T>
void CopyRect(tImage<DstPixel_T> &DstIm, const tImage<SrcPixel_T> &SrcIm, const int srcx, const int srcy,
               const int dstx, const int dsty, const int bwid, const int bhgt);

// Create an NxN gaussian blur kernel with standard deviation sigma.
// Usually represent the kernel as an f1Image
template <class KernelImage_T>
KernelImage_T MakeGaussianKernel(const int N, const typename KernelImage_T::PixType::ElType sigma);

// Convolution for any image type.
// Kernel width must be odd. Kernel should be a one channel image of the pixel's MathType.
template <class Image_T, class KernelImage_T>
void ConvolveImage(Image_T &Out, const Image_T &In, const KernelImage_T &Kernel);

// FiltWid x FiltWid gaussian blur for any image type.
// FiltWid must be odd.
template <class Image_T>
void GaussianBlur(Image_T &Out, const Image_T &In, const int FiltWid, const typename Image_T::PixType::MathType stdev);

// FiltWid x FiltWid gaussian blur for any image type. Each pixel's contribution is further modulated by the color space distance from the target pixel
// FiltWid must be odd. Repeat iterations times.
template <class Image_T>
void VCD(Image_T &Out, const Image_T &In, const int FiltWid, const typename Image_T::PixType::MathType ImageStDev, const typename Image_T::PixType::MathType ColorStDev, const int iterations = 1);

// Any pixel in SrcIm not equal to Key gets copied to DstIm.
// Copies a rectangle from SrcIm of size bwid x bhgt with upper-right
// corner srcx,srcy to upper-right corner dstx,dsty in DstIm.
// The two images may be different types.
// The images may be the same, but the result is undefined if the quads overlap.
template<class DstPixel_T, class SrcPixel_T>
void KeyedBlit(tImage<DstPixel_T> &DstIm, const tImage<SrcPixel_T> &SrcIm, const int srcx, const int srcy,
			   const int dstx, const int dsty, const int bwid, const int bhgt, SrcPixel_T Key);

// Image sampling functions
namespace {
    // Use for 0 <= x <= 1
    inline float CubicFilterN(float x)
    {
        float x2 = x * x;
        return (7.0f/6.0f * x - 2.0f) * x2 + 8.0f/9.0f;
    }

    // Use for 1 <= x <= 2
    inline float CubicFilterF(float x)
    {
        return ((-7.0f/18.0f * x + 2.0f) * x - 10.0f/3.0f) * x + 16.0f/9.0f;
    }

    // This is basically just here as an example.
    inline float CubicFilter(float x)
    {
        x = fabs(x);
        if(x<1) return CubicFilterN(x);
        else return CubicFilterF(x);
    }

    // Return true if all subsamples are inside the image.
    // Otherwise return false and don't modify res.

    // Bicubic sample the image.
    // Does math in the specified pixel type. Could also use the image's pixel's MathType.
    // Doesn't work for int or unsigned int channels! All others should work and use native channel type for the math.
    // XXX Currently fails for non-float images!
    template<class Image_T, class Pixel_T>
    inline bool sample4(Pixel_T &res, const Image_T &Img, const float x, const float y)
    {
        int xl = int(x) - 1;
        int xh = xl + 4;
        if(xl < 0 || xh > Img.w()) // Not accounting for wraparound of huge ints.
            return false;

        int yl = int(y) - 1;
        int yh = yl + 4;
        if(yl < 0 || yh > Img.h()) // Not accounting for wraparound of huge ints.
            return false;

        res = Pixel_T(0);

        typename Pixel_T::ElType Cdx0, Cdx1, Cdx2, Cdx3;
        basePixel::channel_cast(Cdx0, CubicFilterF(x - float(xl)));
        basePixel::channel_cast(Cdx1, CubicFilterN(x - float(xl+1)));
        basePixel::channel_cast(Cdx2, CubicFilterN(float(xl+2) - x));
        basePixel::channel_cast(Cdx3, CubicFilterF(float(xl+3) - x));

        typename Pixel_T::ElType Cdy;
        basePixel::channel_cast(Cdy, CubicFilterF(y - float(yl)));
        res += Pixel_T(Cdx0 * Cdy) * static_cast<Pixel_T>(Img(xl, yl));
        res += Pixel_T(Cdx1 * Cdy) * static_cast<Pixel_T>(Img(xl+1, yl));
        res += Pixel_T(Cdx2 * Cdy) * static_cast<Pixel_T>(Img(xl+2, yl));
        res += Pixel_T(Cdx3 * Cdy) * static_cast<Pixel_T>(Img(xl+3, yl));

        basePixel::channel_cast(Cdy, CubicFilterN(y - float(yl+1)));
        res += Pixel_T(Cdx0 * Cdy) * static_cast<Pixel_T>(Img(xl, yl+1));
        res += Pixel_T(Cdx1 * Cdy) * static_cast<Pixel_T>(Img(xl+1, yl+1));
        res += Pixel_T(Cdx2 * Cdy) * static_cast<Pixel_T>(Img(xl+2, yl+1));
        res += Pixel_T(Cdx3 * Cdy) * static_cast<Pixel_T>(Img(xl+3, yl+1));

        basePixel::channel_cast(Cdy, CubicFilterN(float(yl+2) - y));
        res += Pixel_T(Cdx0 * Cdy) * static_cast<Pixel_T>(Img(xl, yl+2));
        res += Pixel_T(Cdx1 * Cdy) * static_cast<Pixel_T>(Img(xl+1, yl+2));
        res += Pixel_T(Cdx2 * Cdy) * static_cast<Pixel_T>(Img(xl+2, yl+2));
        res += Pixel_T(Cdx3 * Cdy) * static_cast<Pixel_T>(Img(xl+3, yl+2));

        basePixel::channel_cast(Cdy, CubicFilterF(float(yl+3) - y));
        res += Pixel_T(Cdx0 * Cdy) * static_cast<Pixel_T>(Img(xl, yl+3));
        res += Pixel_T(Cdx1 * Cdy) * static_cast<Pixel_T>(Img(xl+1, yl+3));
        res += Pixel_T(Cdx2 * Cdy) * static_cast<Pixel_T>(Img(xl+2, yl+3));
        res += Pixel_T(Cdx3 * Cdy) * static_cast<Pixel_T>(Img(xl+3, yl+3));

        return true;
    }

    // template bool sample4(f3Pixel &res, const f3Image &Img, const float x, const float y);
    // template bool sample4(uc1Pixel &res, const uc1Image &Img, const float x, const float y);
    // template bool sample4(float &res, const uc1Image &Img, const float x, const float y);
    // template bool sample4(float &res, const f1Image &Img, const float x, const float y);

    // Bilinear sample the image.
    // Doesn't work for int or unsigned int channels! All others should work and use native channel type for the math.
    // XXX Currently fails for non-float images!
    template<class Image_T, class Pixel_T>
    inline bool sample2(Pixel_T &res, const Image_T &Img, const float x, const float y)
    {
        int x0 = int(x);
        int y0 = int(y);

        if(x0<0 || (x0+1)>=Img.w() || y0<0 || (y0+1)>=Img.h())
            return false;

        typename Pixel_T::ElType xs;
        basePixel::channel_cast(xs, x - float(x0));
        typename Pixel_T::ElType ys;
        basePixel::channel_cast(ys, y - float(y0));
        typename Pixel_T::ElType omxs = element_traits<typename Pixel_T::ElType>::one() - xs;
        typename Pixel_T::ElType omys = element_traits<typename Pixel_T::ElType>::one() - ys;

        Pixel_T b00 = Img(x0,y0);
        Pixel_T b01 = Img(x0+1,y0);
        Pixel_T b10 = Img(x0,y0+1);
        Pixel_T b11 = Img(x0+1,y0+1);

        Pixel_T b0 = Pixel_T(xs * b01) + Pixel_T(omxs * b00);
        Pixel_T b1 = Pixel_T(xs * b11) + Pixel_T(omxs * b10);
        res = Pixel_T(ys * b1) + Pixel_T(omys * b0);

        return true;
    }

    // template bool sample2(f1Pixel &res, const f1Image &Img, const float x, const float y);
    // template bool sample2(f3Pixel &res, const uc3Image &Img, const float x, const float y);
    // template bool sample2(f3Pixel &res, const f3Image &Img, const float x, const float y);

    // Constant sample the image.
    template<class Image_T, class Pixel_T>
    inline bool sample1(Pixel_T &res, const Image_T &Img, const float x, const float y)
    {
        int x0 = int(x+0.5f);
        int y0 = int(y+0.5f);

        if(x0<0 || x0>=Img.w() || y0<0 || y0>=Img.h())
            return false;

        res = Img(x0,y0);

        return true;
    }

    // template bool sample1(f3Pixel &res, const f3Image &Img, const float x, const float y);
};

#endif
