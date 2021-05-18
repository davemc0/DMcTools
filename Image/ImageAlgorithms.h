//////////////////////////////////////////////////////////////////////
// ImageAlgorithms.h - Image filtering, sampling, etc.
//
// Copyright David K. McAllister, 2001-2014.

#pragma once

#include "Image/tImage.h"

#include <vector>

// Box filter for MIP level generation, etc.
template <class Image_T> void Downsample2x2(Image_T& Out, const Image_T& Img);

// Arbitrary image resizing, both up and down, with any aspect ratio change.
// Should work with all image formats.
template <class Image_T> void Resample(Image_T& Out, const Image_T& Img, const int w1, const int h1);

// Map a float image to an unsigned char image.
template <class OutImage_T, class InImage_T>
void ToneMapLinear(OutImage_T& Out, const InImage_T& Img, const typename InImage_T::PixType::ElType Scale, const typename InImage_T::PixType::ElType Bias);

// Map a float image to an unsigned char image, given the extrema that map to 0..255.
template <class OutImage_T, class InImage_T>
void ToneMapExtrema(OutImage_T& Out, const InImage_T& Img, const typename InImage_T::PixType& MinP, const typename InImage_T::PixType& MaxP);

// Map a float image to an unsigned char image, finding the extrema that map to 0..255.
template <class OutImage_T, class InImage_T> void ToneMapFindExtrema(OutImage_T& Out, const InImage_T& Img);

// Clamp each pixel to the extrema of its neighbors.
template <class Image_T> void DeSpeckle(Image_T& dstImg, const Image_T& srcImg);

template <class Image_T> void DeSpeckleN(Image_T& Img, int N);

// Return an approximation of the median luminance.
template <class Image_T> typename Image_T::PixType::ElType getApproxMedian(const Image_T& Img, const int NumSamples);

// Quantize all the pixels into buckets (separately for each channel)
// and return a histogram for each channel.
// Values beyond minc and maxc are not counted.
// You define a return type that is usually a uiPixel w/ as many channels as the image.
template <class Pixel_T, class Image_T, class Elem_T>
std::vector<Pixel_T> getHistogram(const Image_T& Img, const int NumBuckets, const Elem_T minc, const Elem_T maxc);

// Copy channel number src_ch of image SrcIm to channel dest_ch of image DstIm.
// Things work fine if SrcIm is DstIm.
// Have to template it on the incoming pixel type so that it can be a different number of channels than the output image.
// Converts from src channel format to dest using channel_cast.
// Channel numbers start with 0.
template <class DstPixel_T, class SrcPixel_T> void CopyChan(tImage<DstPixel_T>& DstIm, const int dst_ch, const tImage<SrcPixel_T>& SrcIm, const int src_ch);

// Copies or composites a rectangle using mode: 0 = copy, 1 = alpha blend, 2 = keyed blit, 3 = Adobe multiply
// from SrcIm of size bwid x bhgt with upper-left corner srcx,srcy to upper-left corner dstx,dsty in DstIm.
// The two images may be different types.
// The images may be the same, but the result is undefined if the quads overlap.
// Doesn't resize the dest image to accommodate the quad.
template <class DstPixel_T, class SrcPixel_T>
void CopyRect(tImage<DstPixel_T>& DstIm, const tImage<SrcPixel_T>& SrcIm, const int srcx, const int srcy, const int dstx, const int dsty, const int bwid,
              const int bhgt, const int mode = 0, SrcPixel_T Key = SrcPixel_T(), float alpha = 0.0f);

// Create an NxN Gaussian blur kernel with standard deviation sigma.
// Usually represent the kernel as an f1Image
template <class KernelImage_T> KernelImage_T MakeGaussianKernel(const int N, const typename KernelImage_T::PixType::ElType sigma);

// Convolution for any image type.
// Kernel width must be odd. Kernel should be a one channel image of the pixel's MathType.
template <class Image_T, class KernelImage_T> void ConvolveImage(Image_T& Out, const Image_T& In, const KernelImage_T& Kernel);

// filtWid x filtWid gaussian blur for any image type.
// filtWid must be odd.
template <class Image_T> void GaussianBlur(Image_T& Out, const Image_T& In, const int filtWid, const typename Image_T::PixType::MathType stdev);

// filtWid x filtWid gaussian blur for any image type. Each pixel's contribution is further modulated by the color space distance from the target pixel
// filtWid must be odd. Repeat iterations times.
template <class Image_T>
void VCD(Image_T& Out, const Image_T& In, const int filtWid, const typename Image_T::PixType::MathType ImageStDev,
         const typename Image_T::PixType::MathType ColorStDev, const int iterations = 1);

// Any pixel in SrcIm not equal to Key gets copied to DstIm.
// Copies a rectangle from SrcIm of size bwid x bhgt with upper-right
// corner srcx,srcy to upper-right corner dstx,dsty in DstIm.
// The two images may be different types.
// The images may be the same, but the result is undefined if the quads overlap.
template <class DstPixel_T, class SrcPixel_T>
void KeyedBlit(tImage<DstPixel_T>& DstIm, const tImage<SrcPixel_T>& SrcIm, const int srcx, const int srcy, const int dstx, const int dsty, const int bwid,
               const int bhgt, SrcPixel_T Key);
