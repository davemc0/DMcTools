//////////////////////////////////////////////////////////////////////
// ImageAlgorithms.cpp - Image filtering, sampling, etc.
//
// Copyright David K. McAllister, 2001-2015.

#include "Image/ImageAlgorithms.h"
#include "Math/Random.h"

#include <vector>
#include <algorithm>

using namespace std;

template <class Image_T>
void Downsample2x2(Image_T &Out, const Image_T &Img)
{
    typedef typename Image_T::PixType::MathPixType MPT;

    const int w1 = (1 + Img.w()) / 2; // Output image size
    const int h1 = (1 + Img.h()) / 2;

    Out.SetSize(w1, h1);

    const int ws = Img.w() / 2; // Number of times through loop.
    const int hs = Img.h() / 2;

    for (int y = 0; y < hs; y++) {
        for (int x = 0; x < ws; x++) {
            Out(x, y) = (
                static_cast<MPT>(Img((x << 1), (y << 1))) +
                static_cast<MPT>(Img((x << 1) + 1, (y << 1))) +
                static_cast<MPT>(Img((x << 1), (y << 1) + 1)) +
                static_cast<MPT>(Img((x << 1) + 1, (y << 1) + 1))
                ) / static_cast<typename Image_T::PixType::MathType>(4);
        }
        if (w1 != ws)
            Out(ws, y) = (
            static_cast<MPT>(Img((ws << 1), (y << 1))) +
            static_cast<MPT>(Img((ws << 1), (y << 1) + 1))
            ) / static_cast<typename Image_T::PixType::MathType>(2);
    }
    if (h1 != hs) {
        for (int x = 0; x < ws; x++) {
            Out(x, hs) = (
                static_cast<MPT>(Img((x << 1), (hs << 1))) +
                static_cast<MPT>(Img((x << 1) + 1, (hs << 1)))
                ) / static_cast<typename Image_T::PixType::MathType>(2);
        }
        if (w1 != ws)
            Out(ws, hs) = Img((ws << 1), (hs << 1));
    }
}

template void Downsample2x2(f1Image &Out, const f1Image &Img);
template void Downsample2x2(f3Image &Out, const f3Image &Img);
template void Downsample2x2(f4Image &Out, const f4Image &Img);

namespace {
    // Horizontal box filter for MIP level generation, etc.
    template <class Image_T>
    void Downsample2x1(Image_T &Out, const Image_T &Img)
    {
        const int w1 = (1 + Img.w()) / 2; // Output image size
        const int h1 = Img.h();

        Out.SetSize(w1, h1);

        const int ws = Img.w() / 2; // Number of times through loop.
        const int hs = Img.h();

        for (int y = 0; y < hs; y++) {
            for (int x = 0; x < ws; x++) {
                Out(x, y) = (
                    static_cast<typename Image_T::PixType::MathPixType>(Img((x << 1), y)) +
                    static_cast<typename Image_T::PixType::MathPixType>(Img((x << 1) + 1, y))
                    ) / static_cast<typename Image_T::PixType::MathType>(2);
            }
            if (w1 != ws)
                Out(ws, y) = Img((ws << 1), y);
        }
    }

    // Vertical box filter for MIP level generation, etc.
    template <class Image_T>
    void Downsample1x2(Image_T &Out, const Image_T &Img)
    {
        const int w1 = Img.w(); // Output image size
        const int h1 = (1 + Img.h()) / 2;

        Out.SetSize(w1, h1);

        const int ws = Img.w(); // Number of times through loop.
        const int hs = Img.h() / 2;

        for (int y = 0; y < hs; y++) {
            for (int x = 0; x < ws; x++) {
                Out(x, y) = (
                    static_cast<typename Image_T::PixType::MathPixType>(Img(x, (y << 1))) +
                    static_cast<typename Image_T::PixType::MathPixType>(Img(x, (y << 1) + 1))
                    ) / static_cast<typename Image_T::PixType::MathType>(2);
            }
        }
        if (h1 != hs) {
            for (int x = 0; x < ws; x++) {
                Out(x, hs) = Img(x, (hs << 1));
            }
        }
    }

    // Use for 0 <= x <= 1
    DMC_DECL float CubicFilterN(float x)
    {
        float x2 = x * x;
        return (7.0f / 6.0f * x - 2.0f) * x2 + 8.0f / 9.0f;
    }

    // Use for 1 <= x <= 2
    DMC_DECL float CubicFilterF(float x)
    {
        return ((-7.0f / 18.0f * x + 2.0f) * x - 10.0f / 3.0f) * x + 16.0f / 9.0f;
    }

    // Helper function for sampleNweighted functions
    template<class Image_T, class AccPix_T, class AccEl_T>
    DMC_DECL void takeSample(AccPix_T& faccum, AccEl_T& fweight, const Image_T& Img, int x, int y, AccEl_T wgt)
    {
        if (x >= 0 && x < Img.w() && y >= 0 && y < Img.h()) {
            faccum += static_cast<AccPix_T>(Img(x, y)) * wgt;
            fweight += wgt;
        }
    }

    // Bicubic image sample with accumulated weight for edge case handling
    template<class Image_T>
    struct sample4 {
        DMC_DECL bool operator() (typename Image_T::PixType &res, const Image_T &Img, const float x, const float y)
        {
            typedef typename element_traits<typename Image_T::PixType::ElType>::FloatMathType AccEl_T;
            typedef Image_T::PixType Pixel_T;
            typedef tPixel<AccEl_T, Pixel_T::Chan> AccPix_T;

            int xl = int(x) - 1;
            int xh = xl + 4;
            int yl = int(y) - 1;
            int yh = yl + 4;

            AccEl_T Cdx0 = CubicFilterF(x - float(xl));
            AccEl_T Cdx1 = CubicFilterN(x - float(xl + 1));
            AccEl_T Cdx2 = CubicFilterN(float(xl + 2) - x);
            AccEl_T Cdx3 = CubicFilterF(float(xl + 3) - x);
            AccEl_T Cdy;

            AccPix_T faccum(0);
            AccEl_T fweight = 0;

            Cdy = CubicFilterF(y - float(yl));
            takeSample(faccum, fweight, Img, xl, yl, static_cast<AccEl_T>(Cdx0 * Cdy));
            takeSample(faccum, fweight, Img, xl + 1, yl, static_cast<AccEl_T>(Cdx1 * Cdy));
            takeSample(faccum, fweight, Img, xl + 2, yl, static_cast<AccEl_T>(Cdx2 * Cdy));
            takeSample(faccum, fweight, Img, xl + 3, yl, static_cast<AccEl_T>(Cdx3 * Cdy));

            Cdy = CubicFilterN(y - float(yl + 1));
            takeSample(faccum, fweight, Img, xl, yl + 1, static_cast<AccEl_T>(Cdx0 * Cdy));
            takeSample(faccum, fweight, Img, xl + 1, yl + 1, static_cast<AccEl_T>(Cdx1 * Cdy));
            takeSample(faccum, fweight, Img, xl + 2, yl + 1, static_cast<AccEl_T>(Cdx2 * Cdy));
            takeSample(faccum, fweight, Img, xl + 3, yl + 1, static_cast<AccEl_T>(Cdx3 * Cdy));

            Cdy = CubicFilterN(float(yl + 2) - y);
            takeSample(faccum, fweight, Img, xl, yl + 2, static_cast<AccEl_T>(Cdx0 * Cdy));
            takeSample(faccum, fweight, Img, xl + 1, yl + 2, static_cast<AccEl_T>(Cdx1 * Cdy));
            takeSample(faccum, fweight, Img, xl + 2, yl + 2, static_cast<AccEl_T>(Cdx2 * Cdy));
            takeSample(faccum, fweight, Img, xl + 3, yl + 2, static_cast<AccEl_T>(Cdx3 * Cdy));

            Cdy = CubicFilterF(float(yl + 3) - y);
            takeSample(faccum, fweight, Img, xl, yl + 3, static_cast<AccEl_T>(Cdx0 * Cdy));
            takeSample(faccum, fweight, Img, xl + 1, yl + 3, static_cast<AccEl_T>(Cdx1 * Cdy));
            takeSample(faccum, fweight, Img, xl + 2, yl + 3, static_cast<AccEl_T>(Cdx2 * Cdy));
            takeSample(faccum, fweight, Img, xl + 3, yl + 3, static_cast<AccEl_T>(Cdx3 * Cdy));

            if (fweight > 0) {
                res = static_cast<Pixel_T>(faccum / fweight);
                return true;
            }

            return false;
        }
    };

    // Bilinear image sample with accumulated weight for edge case handling
    // XXX In release mode this returns 254 when sampling values of 255.
    template<class Image_T>
    struct sample2 {
        DMC_DECL bool operator() (typename Image_T::PixType &res, const Image_T &Img, const float x, const float y)
        {
            typedef typename element_traits<typename Image_T::PixType::ElType>::FloatMathType AccEl_T;
            typedef Image_T::PixType Pixel_T;
            typedef tPixel<AccEl_T, Pixel_T::Chan> AccPix_T;

            int x0 = int(x);
            int y0 = int(y);

            AccEl_T xs = x - float(x0);
            AccEl_T ys = y - float(y0);
            AccEl_T omxs = AccEl_T(1) - xs;
            AccEl_T omys = AccEl_T(1) - ys;

            AccPix_T faccum(0);
            AccEl_T fweight = 0;

            takeSample(faccum, fweight, Img, x0, y0, omxs * omys);
            takeSample(faccum, fweight, Img, x0 + 1, y0, xs * omys);
            takeSample(faccum, fweight, Img, x0, y0 + 1, omxs * ys);
            takeSample(faccum, fweight, Img, x0 + 1, y0 + 1, xs * ys);

            if (fweight > 0) {
                res = static_cast<Pixel_T>(faccum / fweight);
                return true;
            }

            return false;
        }
    };

    // Nearest image sample with accumulated weight for edge case handling
    template<class Image_T>
    struct sample1 {
        DMC_DECL bool operator() (typename Image_T::PixType &res, const Image_T &Img, const float x, const float y)
        {
            typedef typename element_traits<typename Image_T::PixType::ElType>::FloatMathType AccEl_T;
            typedef Image_T::PixType Pixel_T;
            typedef tPixel<AccEl_T, Pixel_T::Chan> AccPix_T;

            int x0 = int(x + 0.5f);
            int y0 = int(y + 0.5f);

            if (x0 < 0 || x0 >= Img.w() || y0 < 0 || y0 >= Img.h())
                return false;

            res = Img(x0, y0);

            return true;
        }
    };

};

// For each pixel in output image, sample input image using SampF
template <class Image_T, class Sampler_F>
void ResampleNtap(Sampler_F SampF, Image_T &Out, const Image_T &Img, const int w1, const int h1)
{
    Out.SetSize(w1, h1);

    if (Img.w() == w1 && Img.h() == h1) { // Short-circuit copy without resampling
        Out = Img;
        return;
    }

    const float xstep = (Img.w() - 1) / float(w1 - 1);
    const float ystep = (Img.h() - 1) / float(h1 - 1);

    float yf = 0;
    for (int y = 0; y < h1; y++) {
        float xf = 0;
        for (int x = 0; x < w1; x++) {
            SampF(Out(x, y), Img, xf, yf);
            xf += xstep;
        }
        yf += ystep;
    }
}

template <class Image_T>
void Resample(Image_T &Out, const Image_T &Img, const int w1, const int h1)
{
    ASSERT_R(w1 > 0 && h1 > 0)

    Image_T *OpImg = (Image_T *)&Img;
    while (OpImg->w() / 2 >= w1 && OpImg->h() / 2 >= h1) {
        Image_T *Smaller = new Image_T;
        Downsample2x2(*Smaller, *OpImg); // Downsample by two and replace OpImg.
        if (OpImg != &Img) delete OpImg;
        OpImg = Smaller;
    }

    while (OpImg->w() / 2 >= w1) {
        Image_T *Smaller = new Image_T;
        Downsample2x1(*Smaller, *OpImg); // Downsample by two and replace OpImg.
        if (OpImg != &Img) delete OpImg;
        OpImg = Smaller;
    }

    while (OpImg->h() / 2 >= h1) {
        Image_T *Smaller = new Image_T;
        Downsample1x2(*Smaller, *OpImg); // Downsample by two and replace OpImg.
        if (OpImg != &Img) delete OpImg;
        OpImg = Smaller;
    }

    // Image is less than twice the target size in both dimensions.

    // Use bicubic for upsampling but bilinear for downsampling.
    if (Out.w() > Img.w() || Out.h() > Img.h()) {
        ResampleNtap(sample4<Image_T>(), Out, *OpImg, w1, h1);
    }
    else {
        ResampleNtap(sample2<Image_T>(), Out, *OpImg, w1, h1);
    }

    if (OpImg != &Img) delete OpImg;
}

template void Resample(f1Image &Out, const f1Image &Img, const int w1, const int h1);
template void Resample(f3Image &Out, const f3Image &Img, const int w1, const int h1); // Tested Feb. 2015
template void Resample(f4Image &Out, const f4Image &Img, const int w1, const int h1);
template void Resample(uc1Image &Out, const uc1Image &Img, const int w1, const int h1); // Tested Apr. 2016
template void Resample(uc3Image &Out, const uc3Image &Img, const int w1, const int h1);
template void Resample(uc4Image &Out, const uc4Image &Img, const int w1, const int h1); // Tested Dec. 2014
