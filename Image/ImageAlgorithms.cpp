//////////////////////////////////////////////////////////////////////
// ImageAlgorithms.cpp - Image filtering, sampling, etc.
//
// Copyright David K. McAllister, 2001-2015.

#include "Image/ImageAlgorithms.h"

#include "Math/Random.h"

#include <algorithm>
#include <vector>

using namespace std;

template <class OutImage_T, class InImage_T>
void ToneMapLinear(OutImage_T& Out, const InImage_T& Img, const typename InImage_T::PixType::ElType Scale, const typename InImage_T::PixType::ElType Bias)
{
    Out.SetSize(Img.w(), Img.h());

    for (int i = 0; i < Img.size(); i++) {
        typename InImage_T::PixType tmp = Scale * Img[i];
        tmp += Bias;
        Out[i] = static_cast<typename OutImage_T::PixType>(tmp);
    }
}

// This instantiation is touchy. Your code has to really give float args.
template void ToneMapLinear(uc1Image& Out, const f1Image& Img, const float Scale, const float Bias);
template void ToneMapLinear(uc3Image& Out, const f3Image& Img, const float Scale, const float Bias);
template void ToneMapLinear(uc1Image& Out, const f3Image& Img, const float Scale, const float Bias);

template <class OutImage_T, class InImage_T>
void ToneMapExtrema(OutImage_T& Out, const InImage_T& Img, const typename InImage_T::PixType& MinP, const typename InImage_T::PixType& MaxP)
{
    float minc = MinP.min_chan();
    float maxc = MaxP.max_chan();

    float Scale = 1.0f / (maxc - minc);
    float Bias = -(minc * Scale);

    ToneMapLinear(Out, Img, Scale, Bias);
}

template void ToneMapExtrema(uc1Image& Out, const f1Image& Img, const f1Pixel& MinP, const f1Pixel& MaxP);
template void ToneMapExtrema(uc3Image& Out, const f3Image& Img, const f3Pixel& MinP, const f3Pixel& MaxP);
template void ToneMapExtrema(uc1Image& Out, const f3Image& Img, const f3Pixel& MinP, const f3Pixel& MaxP);

template <class OutImage_T, class InImage_T> void ToneMapFindExtrema(OutImage_T& Out, const InImage_T& Img)
{
    typename InImage_T::PixType MinP, MaxP;
    Img.GetMinMax(MinP, MaxP);

    ToneMapExtrema(Out, Img, MinP, MaxP);
}

template void ToneMapFindExtrema(uc1Image& Out, const f1Image& Img);
template void ToneMapFindExtrema(uc3Image& Out, const f3Image& Img);
template void ToneMapFindExtrema(uc1Image& Out, const f3Image& Img);

// If a pixel is outside bounding box of all its neighbors then clamp it into the box
template <class Image_T> void DeSpeckle(Image_T& dstImg, const Image_T& srcImg)
{
    dstImg.SetSize(srcImg.w(), srcImg.h());

    for (int y = 1; y < srcImg.h() - 1; y++) {
        for (int x = 1; x < srcImg.w() - 1; x++) {
            // Channel-wise min and max make the bounding box of the neighbors
            typename Image_T::PixType MinP = srcImg(x - 1, y - 1), MaxP = srcImg(x - 1, y - 1);
            MinP = Min(MinP, srcImg(x, y - 1));
            MaxP = Max(MaxP, srcImg(x, y - 1));
            MinP = Min(MinP, srcImg(x + 1, y - 1));
            MaxP = Max(MaxP, srcImg(x + 1, y - 1));
            MinP = Min(MinP, srcImg(x - 1, y));
            MaxP = Max(MaxP, srcImg(x - 1, y));
            MinP = Min(MinP, srcImg(x + 1, y));
            MaxP = Max(MaxP, srcImg(x + 1, y));
            MinP = Min(MinP, srcImg(x - 1, y + 1));
            MaxP = Max(MaxP, srcImg(x - 1, y + 1));
            MinP = Min(MinP, srcImg(x, y + 1));
            MaxP = Max(MaxP, srcImg(x, y + 1));
            MinP = Min(MinP, srcImg(x + 1, y + 1));
            MaxP = Max(MaxP, srcImg(x + 1, y + 1));
            // Set the pixel to a point inside the bounding box. Should we do the box center or some such?
            dstImg(x, y) = Clamp(MinP, srcImg(x, y), MaxP);
        }
    }
}

template void DeSpeckle(f1Image& dstImg, const f1Image& srcImg);
template void DeSpeckle(f3Image& dstImg, const f3Image& srcImg);
template void DeSpeckle(f4Image& dstImg, const f4Image& srcImg);
template void DeSpeckle(uc1Image& dstImg, const uc1Image& srcImg);
template void DeSpeckle(uc3Image& dstImg, const uc3Image& srcImg);
template void DeSpeckle(uc4Image& dstImg, const uc4Image& srcImg);

// If a pixel is brighter than at least N neighbors then set it to its brightest neighbor
template <class Image_T> void DeSpeckleN(Image_T& Img, int N)
{
    Image_T NImg(Img.w(), Img.h());
    NImg.fill(uc1Pixel(255)); // The 255 is a hack. The fill should be unnecessary.

    for (int y = 1; y < Img.h() - 1; y++) {
        for (int x = 1; x < Img.w() - 1; x++) {
            typename Image_T::PixType MinP = Img(x - 1, y - 1), MaxP = Img(x - 1, y - 1);
            MinP = Min(MinP, Img(x, y - 1));
            MaxP = Max(MaxP, Img(x, y - 1));
            MinP = Min(MinP, Img(x + 1, y - 1));
            MaxP = Max(MaxP, Img(x + 1, y - 1));
            MinP = Min(MinP, Img(x - 1, y));
            MaxP = Max(MaxP, Img(x - 1, y));
            MinP = Min(MinP, Img(x + 1, y));
            MaxP = Max(MaxP, Img(x + 1, y));
            MinP = Min(MinP, Img(x - 1, y + 1));
            MaxP = Max(MaxP, Img(x - 1, y + 1));
            MinP = Min(MinP, Img(x, y + 1));
            MaxP = Max(MaxP, Img(x, y + 1));
            MinP = Min(MinP, Img(x + 1, y + 1));
            MaxP = Max(MaxP, Img(x + 1, y + 1));

            typename Image_T::PixType Me = Img(x, y);
            int c = 0;
            c += Img(x - 1, y - 1) <= Me;
            c += Img(x, y - 1) <= Me;
            c += Img(x + 1, y - 1) <= Me;
            c += Img(x - 1, y) <= Me;
            c += Img(x + 1, y) <= Me;
            c += Img(x - 1, y + 1) <= Me;
            c += Img(x, y + 1) <= Me;
            c += Img(x + 1, y + 1) <= Me;
            NImg(x, y) = c >= N ? Me : MaxP;
        }
    }

    Img = NImg;
}

template void DeSpeckleN(uc1Image& Img, int N);

template <class Image_T> typename Image_T::PixType::ElType getApproxMedian(const Image_T& Img, const int NumSamples)
{
    typename Image_T::PixType::ElType* vals = new typename Image_T::PixType::ElType[NumSamples];

    for (int k = 0; k < NumSamples; k++) {
        int i = LRand() % Img.size();
        vals[k] = Img[i].luminance();
    }

    sort(&vals[0], &vals[NumSamples]);

    typename Image_T::PixType::ElType ret = vals[NumSamples / 2];
    delete[] vals;

    return ret;
}

template float getApproxMedian(const f3Image& Img, const int NumSamples);

template <class Pixel_T, class Image_T, class Elem_T>
vector<Pixel_T> getHistogram(const Image_T& Img, const int NumBuckets, const Elem_T minc, const Elem_T maxc)
{
    vector<Pixel_T> Hist(NumBuckets, Pixel_T(0));

    float scale = float(NumBuckets) / float(maxc - minc);
    for (int k = 0; k < Img.size(); k++) {
        for (int c = 0; c < Img.chan(); c++) {
            int bucket = int(float(Img[k][c] - minc) * scale);
            if (bucket >= 0 && bucket < NumBuckets) Hist[bucket][c]++;
        }
    }

    return Hist;
}

// When calling, make sure the last two args are actual floats.
template vector<ui1Pixel> getHistogram(const uc1Image& Img, const int NumBuckets, const float minc, const float maxc);
template vector<ui3Pixel> getHistogram(const uc3Image& Img, const int NumBuckets, const float minc, const float maxc);
template vector<ui1Pixel> getHistogram(const us1Image& Img, const int NumBuckets, const float minc, const float maxc);
template vector<ui3Pixel> getHistogram(const us3Image& Img, const int NumBuckets, const float minc, const float maxc);
template vector<ui3Pixel> getHistogram(const f3Image& Img, const int NumBuckets, const float minc, const float maxc);

template <class DstPixel_T, class SrcPixel_T> void CopyChan(tImage<DstPixel_T>& DstIm, const int dst_ch, const tImage<SrcPixel_T>& SrcIm, const int src_ch)
{
    ASSERT_R(DstIm.w() == SrcIm.w() && DstIm.h() == SrcIm.h());
    ASSERT_R(src_ch < SrcIm.chan() && dst_ch < DstIm.chan());

    for (int i = 0; i < DstIm.size(); i++) basePixel::channel_cast(DstIm[i][dst_ch], SrcIm[i][src_ch]);
}

template void CopyChan(f1Image& DstIm, const int dst_ch, const f3Image& SrcIm, const int src_ch);

// TODO: Make the per-pixel operation be generic by passing in a lambda
template <class DstPixel_T, class SrcPixel_T>
void CopyRect(tImage<DstPixel_T>& DstIm, const tImage<SrcPixel_T>& SrcIm, const int srcx, const int srcy, const int dstx, const int dsty, const int bwid,
              const int bhgt, const int mode, SrcPixel_T Key, float alpha)
{
    if (bwid <= 0 || bhgt <= 0 || DstIm.size() <= 0 || SrcIm.size() <= 0) return;

    // Compute transformation from src to dst.
    int stodx = dstx - srcx;
    int stody = dsty - srcy;

    // Clip box against src.
    int bw = (bwid + srcx >= SrcIm.w()) ? (SrcIm.w() - srcx) : bwid;
    int bh = (bhgt + srcy >= SrcIm.h()) ? (SrcIm.h() - srcy) : bhgt;

    int bsx = srcx;
    if (bsx < 0) {
        bw += bsx;
        bsx = 0;
    }
    int bsy = srcy;
    if (bsy < 0) {
        bh += bsy;
        bsy = 0;
    }

    // Transform box to dst space.
    int bdx = bsx + stodx;
    int bdy = bsy + stody;

    // Clip box against dst.
    if (bw + dstx >= DstIm.w()) bw = DstIm.w() - dstx;
    if (bh + dsty >= DstIm.h()) bh = DstIm.h() - dsty;

    if (bdx < 0) {
        bw += bdx;
        bdx = 0;
    }
    if (bdy < 0) {
        bh += bdy;
        bdy = 0;
    }

    if (bw <= 0 || bh <= 0) return;

    // Transform box back to src space.
    bsx = bdx - stodx;
    bsy = bdy - stody;

    int srcxmax = bsx + bw;
    int srcymax = bsy + bh;

    int chan = Key[0];

    for (int sy = bsy, dy = bdy; sy < srcymax; sy++, dy++) {
        for (int sx = bsx, dx = bdx; sx < srcxmax; sx++, dx++) {
            if (mode == 0) // Pixel Copy
                DstIm(dx, dy) = static_cast<DstPixel_T>(SrcIm(sx, sy));
            else if (mode == 1) { // OVER Operator with per-pixel alpha
                typename DstPixel_T::FloatMathPixType srcpix(SrcIm(sx, sy));
                typename DstPixel_T::FloatMathType srcalpha(srcpix[3]);
                DstIm(dx, dy) = LinearInterp(DstIm(dx, dy), SrcIm(sx, sy), srcalpha);
            } else if (mode == 2) { // Color key BLIT
                // This one makes white and off-white be transparent. Should turn this into alpha.
                if (SrcIm(sx, sy) != Key) DstIm(dx, dy) = static_cast<DstPixel_T>(SrcIm(sx, sy));
                // if ((Key - SrcIm(sx, sy)).max_chan() > 25) DstIm(dx, dy) = static_cast<DstPixel_T>(SrcIm(sx, sy));
            } else if (mode == 3) { // Adobe Multiply mode (alpha lerp between dst and src*dst)
                typename DstPixel_T::FloatMathPixType srcpix(SrcIm(sx, sy));
                typename DstPixel_T::FloatMathPixType dstpix(DstIm(dx, dy));
                typename DstPixel_T::FloatMathType srcalpha(srcpix[3]);
                typename DstPixel_T::FloatMathPixType mp(srcpix * dstpix);
                DstIm(dx, dy) = LinearInterp(dstpix, mp, srcalpha);
            } else if (mode == 4) { // OVER Operator with constant alpha
                DstIm(dx, dy) = LinearInterp(DstIm(dx, dy), SrcIm(sx, sy), alpha);
            } else if (mode == 5) { // Adding pixels
                DstIm(dx, dy) = DstIm(dx, dy) + SrcIm(sx, sy);
            } else if (mode == 6) { // Copy one channel
                DstIm(dx, dy)[chan] = SrcIm(sx, sy)[chan];
            }
        }
    }
}

template void CopyRect(f1Image& DstIm, const f1Image& SrcIm, const int srcx, const int srcy, const int dstx, const int dsty, const int bwid, const int bhgt,
                       const int mode, f1Image ::PixType, const float alpha);
template void CopyRect(f3Image& DstIm, const f3Image& SrcIm, const int srcx, const int srcy, const int dstx, const int dsty, const int bwid, const int bhgt,
                       const int mode, f3Image ::PixType, const float alpha);
template void CopyRect(f4Image& DstIm, const f4Image& SrcIm, const int srcx, const int srcy, const int dstx, const int dsty, const int bwid, const int bhgt,
                       const int mode, f4Image ::PixType, const float alpha);
template void CopyRect(uc1Image& DstIm, const uc1Image& SrcIm, const int srcx, const int srcy, const int dstx, const int dsty, const int bwid, const int bhgt,
                       const int mode, uc1Image ::PixType, const float alpha);
template void CopyRect(uc3Image& DstIm, const uc3Image& SrcIm, const int srcx, const int srcy, const int dstx, const int dsty, const int bwid, const int bhgt,
                       const int mode, uc3Image ::PixType, const float alpha);
template void CopyRect(uc4Image& DstIm, const uc4Image& SrcIm, const int srcx, const int srcy, const int dstx, const int dsty, const int bwid, const int bhgt,
                       const int mode, uc4Image ::PixType, const float alpha);

template <class KernelImage_T> KernelImage_T MakeGaussianKernel(const int N, const typename KernelImage_T::PixType::ElType sigma)
{
    KernelImage_T Kernel(N, N);
    ASSERT_R(Kernel.chan() == 1);

    const int N2 = N / 2;
    typename KernelImage_T::PixType::ElType Sum = 0;

    for (int y = -N2; y <= N2; y++) {
        for (int x = -N2; x <= N2; x++) {
            typename KernelImage_T::PixType::ElType G = Gaussian2(static_cast<typename KernelImage_T::PixType::ElType>(x),
                                                                  static_cast<typename KernelImage_T::PixType::ElType>(y), sigma);
            Kernel(x + N2, y + N2) = static_cast<typename KernelImage_T::PixType>(G);
            Sum += G;
        }
    }

    // Normalize the kernel.
    Kernel /= static_cast<typename KernelImage_T::PixType>(Sum);

    return Kernel;
}

namespace {
// Sample the image with the kernel. Divide by sum of covered weights.
template <class Image_T, class KernelImage_T>
DMC_DECL typename Image_T::PixType sample_kernel_weighted(const Image_T& Img, const KernelImage_T& Kernel, const int xc, const int yc)
{
    const int N = Kernel.w();
    const int N2 = N / 2;

    typename Image_T::PixType::MathPixType sum(0);
    typename KernelImage_T::PixType::MathType weight = 0;

    int xl = max(xc - N2, 0), xh = min(xc + N2, Img.w() - 1);
    int yl = max(yc - N2, 0), yh = min(yc + N2, Img.h() - 1);
    for (int y = yl; y <= yh; y++) {
        for (int x = xl; x <= xh; x++) {
            typename KernelImage_T::PixType::ElType K = Kernel(xc - x + N2, yc - y + N2);
            sum += Img(x, y) * K;
            weight += K;
        }
    }

    ASSERT_D(weight != 0);

    return sum / weight;
}

// Sample the image with the kernel. Kernel is fully inside image.
template <class Image_T, class KernelImage_T>
DMC_DECL typename Image_T::PixType sample_kernel_full(const Image_T& Img, const KernelImage_T& Kernel, const int xc, const int yc)
{
    const int N = Kernel.w();
    const int N2 = N / 2;

    typename Image_T::PixType::MathPixType sum(0);
    const typename KernelImage_T::PixType::ElType* Kp = (const typename KernelImage_T::PixType::ElType*)Kernel.pv();

    for (int yk = 0; yk < N; yk++) {
        int y = yc - N2 + yk;
        for (int xk = 0; xk < N; xk++) {
            int x = xc - N2 + xk;
            const typename KernelImage_T::PixType::ElType K = *Kp++;
            sum += Img(x, y) * K;
        }
    }

    return sum;
}

// Sample the image with the kernel. Kernel is fully inside image.
// Kernel width, N, is known at compile time.
template <class Image_T, class KernelImage_T, int N>
DMC_DECL typename Image_T::PixType sample_kernel_fullN(const Image_T& Img, const KernelImage_T& Kernel, const int xc, const int yc)
{
    const int N2 = N / 2;

    typename Image_T::PixType::MathPixType sum(0);
    const typename KernelImage_T::PixType::ElType* Kp = (const typename KernelImage_T::PixType::ElType*)Kernel.pv();

    for (int yk = 0; yk < N; yk++) {
        int y = yc - N2 + yk;
        for (int xk = 0; xk < N; xk++) {
            int x = xc - N2 + xk;
            const typename KernelImage_T::PixType::ElType K = *Kp++;
            sum += Img(x, y) * K;
        }
    }

    return sum;
}

// Convolve the parts of the image where the kernel is fully inside. This should be fast.
template <class Image_T, class KernelImage_T> void ConvolveMiddle(Image_T& Out, const Image_T& In, const KernelImage_T& Kernel)
{
    const int N = Kernel.w();
    const int N2 = N / 2;
    const int wid = In.w(), hgt = In.h();
    for (int y = N2; y < hgt - N2; y++) {
        for (int x = N2; x < wid - N2; x++) { Out(x, y) = sample_kernel_full(In, Kernel, x, y); }
    }
}

// Kernel width, N, is known at compile time.
template <class Image_T, class KernelImage_T, int N> void ConvolveMiddleN(Image_T& Out, const Image_T& In, const KernelImage_T& Kernel)
{
    const int N2 = N / 2;
    ASSERT_R((Kernel.w() == N));
    const int wid = In.w(), hgt = In.h();

    // Do center.
    for (int y = N2; y < hgt - N2; y++) {
        for (int x = N2; x < wid - N2; x++) { Out(x, y) = sample_kernel_fullN<Image_T, KernelImage_T, N>(In, Kernel, x, y); }
    }
}

// Specialization for f1Image, 5.
// Uses direct pixel access and loop unrolling for big speedup.
template <> void ConvolveMiddleN<f1Image, f1Image, 5>(f1Image& Out, const f1Image& In, const f1Image& Kernel)
{
    float* OutP = (float*)Out.pv();
    const float* InP = (const float*)In.pv();
    const float* KP = (const float*)Kernel.pv();

    const int wid = In.w(), hgt = In.h();
    for (int y = 2; y < hgt - 2; y++) {
        for (int x = 2; x < wid - 2; x++) {
            float sum = 0;

            sum += KP[0] * InP[(y - 2) * wid + x - 2];
            sum += KP[1] * InP[(y - 2) * wid + x - 1];
            sum += KP[2] * InP[(y - 2) * wid + x];
            sum += KP[3] * InP[(y - 2) * wid + x + 1];
            sum += KP[4] * InP[(y - 2) * wid + x + 2];

            sum += KP[5] * InP[(y - 1) * wid + x - 2];
            sum += KP[6] * InP[(y - 1) * wid + x - 1];
            sum += KP[7] * InP[(y - 1) * wid + x];
            sum += KP[8] * InP[(y - 1) * wid + x + 1];
            sum += KP[9] * InP[(y - 1) * wid + x + 2];

            sum += KP[10] * InP[(y)*wid + x - 2];
            sum += KP[11] * InP[(y)*wid + x - 1];
            sum += KP[12] * InP[(y)*wid + x];
            sum += KP[13] * InP[(y)*wid + x + 1];
            sum += KP[14] * InP[(y)*wid + x + 2];

            sum += KP[15] * InP[(y + 1) * wid + x - 2];
            sum += KP[16] * InP[(y + 1) * wid + x - 1];
            sum += KP[17] * InP[(y + 1) * wid + x];
            sum += KP[18] * InP[(y + 1) * wid + x + 1];
            sum += KP[19] * InP[(y + 1) * wid + x + 2];

            sum += KP[20] * InP[(y + 2) * wid + x - 2];
            sum += KP[21] * InP[(y + 2) * wid + x - 1];
            sum += KP[22] * InP[(y + 2) * wid + x];
            sum += KP[23] * InP[(y + 2) * wid + x + 1];
            sum += KP[24] * InP[(y + 2) * wid + x + 2];

            OutP[y * wid + x] = sum;
        }
    }
}
}; // namespace

template <class Image_T, class KernelImage_T> void ConvolveImage(Image_T& Out, const Image_T& In, const KernelImage_T& Kernel)
{
    const int N = Kernel.w();
    const int N2 = N / 2;
    ASSERT_R((N & 1) && N >= 3); // Filter must be an odd width so it can center on a pixel.
    const int wid = In.w(), hgt = In.h();
    Out.SetSize(wid, hgt);

    // Do top and bottom edges.
    for (int y = 0; y < N2; y++)
        for (int x = N2; x < wid - N2; x++) Out(x, y) = sample_kernel_weighted(In, Kernel, x, y);
    for (int y = hgt - N2; y < hgt; y++)
        for (int x = N2; x < wid - N2; x++) Out(x, y) = sample_kernel_weighted(In, Kernel, x, y);

    // Do left and right edges.
    for (int y = 0; y < hgt; y++) {
        for (int x = 0; x < N2; x++) Out(x, y) = sample_kernel_weighted(In, Kernel, x, y);
        for (int x = wid - N2; x < wid; x++) Out(x, y) = sample_kernel_weighted(In, Kernel, x, y);
    }

    if (N == 5)
        ConvolveMiddleN<Image_T, KernelImage_T, 5>(Out, In, Kernel); // If kernel size is known at compile time.
    else if (N == 9)
        ConvolveMiddleN<Image_T, KernelImage_T, 9>(Out, In, Kernel); // If kernel size is known at compile time.
    else
        ConvolveMiddle(Out, In, Kernel); // If kernel size is not known at compile time.
}

// FiltWid x filtWid gaussian blur for any image type.
// filtWid must be odd.
template <class Image_T> void GaussianBlur(Image_T& Out, const Image_T& In, const int filtWid, const typename Image_T::PixType::MathType stdev)
{
    ASSERT_R((filtWid & 1) && filtWid >= 3); // Filter must be an odd width so it can center on a pixel.

    typedef tPixel<typename Image_T::PixType::ElType, 1> KernelPixelType;
    typedef tImage<KernelPixelType> KernelImageType;

    KernelImageType Kernel = MakeGaussianKernel<KernelImageType>(filtWid, stdev);

    ConvolveImage(Out, In, Kernel);
}

template void GaussianBlur<f1Image>(f1Image& Out, const f1Image& In, const int filtWid, float stdev);
template void GaussianBlur<f3Image>(f3Image& Out, const f3Image& In, const int filtWid, float stdev);
template void GaussianBlur<f4Image>(f4Image& Out, const f4Image& In, const int filtWid, float stdev);
