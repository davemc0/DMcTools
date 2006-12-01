//////////////////////////////////////////////////////////////////////
// ImageAlgorithms.h - Image filtering, sampling, etc.
//
// Copyright David K. McAllister, 2001.

#ifndef _ImageAlgorithms_h
#define _ImageAlgorithms_h

#include <Image/tImage.h>

#include <vector>


// Shamelessly copied from Alvy Ray Smith's Principles of Image Compositing
// Returns an 8.8 fixed.
inline int int_mult(unsigned char a, unsigned char b)
{
    int t = int(a) * int(b) + 0x80;
    return ((t>>8) + t);
}

// Return true if all subsamples are inside image.
// Otherwise return false and don't modify res.

// Bicubic sample the image.
template<class _ImgType, class _PixType>
bool sample4(_PixType &res, const _ImgType &Img, const float x, const float y);

// Bilinear sample the image.
template<class _ImgType, class _PixType>
bool sample2(_PixType &res, const _ImgType &Img, const float x, const float y);

// Constant sample the image.
template<class _ImgType, class _PixType>
bool sample1(_PixType &res, const _ImgType &Img, const float x, const float y);

// Map a float image to an unsigned char image using log scale.
template<class OutImgType, class InImgType, class ElType>
void ToneMapLog(OutImgType &Out, const InImgType &Img,
                const ElType Scale, const ElType Bias, const ElType LogBias);

// Map a float image to an unsigned char image.
template<class OutImgType, class InImgType, class ElType>
void ToneMapLinear(OutImgType &Out, const InImgType &Img,
                   const ElType Scale, const ElType Bias);

// Map a float image to an unsigned char image, given the extrema that map to 0..255.
template<class OutImgType, class InImgType, class InPixType>
void ToneMapExtrema(OutImgType &Out, const InImgType &Img,
                    const InPixType &MinP, const InPixType &MaxP);

// Map a float image to an unsigned char image, finding the extrema that map to 0..255.
template<class OutImgType, class InImgType>
void ToneMapFindExtrema(OutImgType &Out, const InImgType &Img);

// Clamp each pixel to the extrema of its neighbors.
template <class _ImgType>
void DeSpeckle(_ImgType &Img);

template <class _ImgType>
void DeSpeckleN(_ImgType &Img, int N);

// Return an approximation of the median luminance.
template <class _RetType, class _ImgType>
_RetType getApproxMedian(const _ImgType &Img, const int NumSamples);

// Quantize all the pixels into buckets (separately for each channel)
// and return a histogram for each channel.
// Values beyond minc and maxc are not counted.
// You define a return type that is usually a uiPixel w/ as many channels as the image.
template <class _PixType, class _ImgType, class _ElType>
std::vector<_PixType> getHistogram(const _ImgType &Img, const int NumBuckets,
                              const _ElType minc, const _ElType maxc);

// Cheezy box filter for MIP level generation, etc.
template <class _ImgType>
void Downsample2(_ImgType &Out, const _ImgType &Img);

// Bi-cubic upsampling to arbitrary size.
template <class _ImgType>
void Upsample4(_ImgType &Out, const _ImgType &Img, const int w, const int h);

#if 0
template <class _ImgType>
void Blur(_ImgType &Img, const _ImgType::PixType::MathType stdev)
#endif

#endif
