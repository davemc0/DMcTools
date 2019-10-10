//////////////////////////////////////////////////////////////////////
// VCD.cpp - Variable Conductance Diffusion
//
// Copyright David K. McAllister, 2008.

#include "Image/ImageAlgorithms.h"
#include "Math/Random.h"

#include <vector>
using namespace std;

namespace {
    // Functor to evaluate gaussian(sqrt(c)).
    template<class Gauss_T>
    struct GaussSqFunc_t
    {
        Gauss_T StDevSq;
        GaussSqFunc_t(const Gauss_T StDev) : StDevSq(StDev * StDev) {}
        Gauss_T operator()(const Gauss_T c) const { return static_cast<Gauss_T>(GaussianSq(static_cast<double>(c), StDevSq)); }
    };

    // Sample the image with the kernel. Divide by sum of covered weights.
    template<class Image_T, class KernelImage_T, class GaussSqFunctor_T>
    DMC_DECL typename Image_T::PixType sample_kernel_vcd_weighted(const Image_T &Img, const KernelImage_T &Kernel,
        const GaussSqFunctor_T &GSq, const int xc, const int yc)
    {
        const int N = Kernel.w();
        const int N2 = N/2;

        typename Image_T::PixType::MathPixType sum(0);
        typename KernelImage_T::PixType::MathType weight = 0;
        const typename Image_T::PixType Pc = Img(xc,yc);

        int xl=max(xc-N2,0), xh=min(xc+N2,Img.w()-1);
        int yl=max(yc-N2,0), yh=min(yc+N2,Img.h()-1);
        for(int y=yl; y<=yh; y++) {
            for(int x=xl; x<=xh; x++) {
                const typename KernelImage_T::PixType::ElType K = Kernel(xc - x + N2, yc - y + N2);
                const typename Image_T::PixType P = Img(x,y);
                const typename Image_T::PixType::MathType ColorDistSqr = DiffSqr(Pc, P); // Compute sum of squared difference of channels in color space
                const typename KernelImage_T::PixType::ElType G = GSq(ColorDistSqr); // Compute gaussian(sqrt(ColorDistSqr))
                const typename KernelImage_T::PixType::ElType W = K * G;
                sum += P * W;
                weight += W;
            }
        }

        ASSERT_D(weight != 0);

        return sum / weight;
    }

    // Sample the image with the kernel. Kernel is fully inside image.
    template<class Image_T, class KernelImage_T, class GaussSqFunctor_T>
    DMC_DECL typename Image_T::PixType sample_kernel_vcd_full(const Image_T &Img, const KernelImage_T &Kernel,
        const GaussSqFunctor_T &GSq, const int xc, const int yc)
    {
        const int N = Kernel.w();
        const int N2 = N/2;

        typename Image_T::PixType::MathPixType sum(0);
        typename KernelImage_T::PixType::MathType weight = 0;
        const typename Image_T::PixType Pc = Img(xc,yc);
        const typename KernelImage_T::PixType::ElType *Kp = (const typename KernelImage_T::PixType::ElType *)Kernel.pv();

        for(int yk=0; yk<N; yk++) {
            int y = yc - N2 + yk;
            for(int xk=0; xk<N; xk++) {
                int x = xc - N2 + xk;
                const typename KernelImage_T::PixType::ElType K = *Kp++;
                const typename Image_T::PixType P = Img(x,y);
                const typename Image_T::PixType::MathType ColorDistSqr = DiffSqr(Pc, P); // Compute sum of squared difference of channels in color space
                const typename KernelImage_T::PixType::ElType G = GSq(ColorDistSqr); // Compute gaussian(sqrt(ColorDistSqr))
                const typename KernelImage_T::PixType::ElType W = K * G;
                sum += P * W;
                weight += W;
            }
        }

        ASSERT_D(weight != 0);

        return sum / weight;
    }

    // Sample the image with the kernel. Kernel is fully inside image.
    // Kernel width, N, is known at compile time.
    template<class Image_T, class KernelImage_T, class GaussSqFunctor_T, int N>
    DMC_DECL typename Image_T::PixType sample_kernel_vcd_fullN(const Image_T &Img, const KernelImage_T &Kernel,
        const GaussSqFunctor_T &GSq, const int xc, const int yc)
    {
        const int N2 = N/2;

        typename Image_T::PixType::MathPixType sum(0);
        typename KernelImage_T::PixType::MathType weight = 0;
        const typename Image_T::PixType Pc = Img(xc,yc);
        const typename KernelImage_T::PixType::ElType *Kp = (const typename KernelImage_T::PixType::ElType *)Kernel.pv();

        for(int yk=0; yk<N; yk++) {
            int y = yc - N2 + yk;
            for(int xk=0; xk<N; xk++) {
                int x = xc - N2 + xk;
                const typename KernelImage_T::PixType::ElType K = *Kp++;
                const typename Image_T::PixType P = Img(x,y);
                const typename Image_T::PixType::MathType ColorDistSqr = DiffSqr(Pc, P); // Compute sum of squared difference of channels in color space
                const typename KernelImage_T::PixType::ElType G = GSq(ColorDistSqr); // Compute gaussian(sqrt(ColorDistSqr))
                const typename KernelImage_T::PixType::ElType W = K * G;
                sum += P * W;
                weight += W;
            }
        }

        ASSERT_D(weight != 0);

        return sum / weight;
    }

    // Convolve the parts of the image where the kernel is fully inside. This should be fast.
    template <class Image_T, class KernelImage_T, class GaussSqFunctor_T>
    void ConvolveMiddleVCD(Image_T &Out, const Image_T &In, const KernelImage_T &Kernel, const GaussSqFunctor_T &GSq)
    {
        const int N = Kernel.w();
        const int N2 = N/2;
        const int wid = In.w(), hgt = In.h();
        for(int y=N2; y<hgt-N2; y++) {
            for(int x=N2; x<wid-N2; x++) {
                Out(x,y) = sample_kernel_vcd_full(In, Kernel, GSq, x, y);
            }
        }
    }

    // Kernel width, N, is known at compile time.
    template <class Image_T, class KernelImage_T, class GaussSqFunctor_T, int N>
    void ConvolveMiddleVCDN(Image_T &Out, const Image_T &In, const KernelImage_T &Kernel, const GaussSqFunctor_T &GSq)
    {
        const int N2 = N/2;
        ASSERT_R((Kernel.w() == N));
        const int wid = In.w(), hgt = In.h();

        // Do center.
        for(int y=N2; y<hgt-N2; y++) {
            for(int x=N2; x<wid-N2; x++) {
                Out(x,y) = sample_kernel_vcd_fullN<Image_T, KernelImage_T, GaussSqFunctor_T, N>(In, Kernel, GSq, x, y);
            }
        }
    }
};

template <class Image_T, class KernelImage_T, class GaussSqFunctor_T>
void ConvolveImageVCD(Image_T &Out, const Image_T &In, const KernelImage_T &Kernel, const GaussSqFunctor_T &GSq)
{
    const int N = Kernel.w();
    const int N2 = N/2;
    ASSERT_R((N & 1) && N >= 3); // Filter must be an odd width so it can center on a pixel.
    const int wid = In.w(), hgt = In.h();
    Out.SetSize(wid, hgt);

    // Do top and bottom edges.
    for(int y=0; y<N2; y++)
        for(int x=N2; x<wid-N2; x++)
            Out(x,y) = sample_kernel_vcd_weighted(In, Kernel, GSq, x, y);
    for(int y=hgt-N2; y<hgt; y++)
        for(int x=N2; x<wid-N2; x++)
            Out(x,y) = sample_kernel_vcd_weighted(In, Kernel, GSq, x, y);

    // Do left and right edges.
    for(int y=0; y<hgt; y++) {
        for(int x=0; x<N2; x++)
            Out(x,y) = sample_kernel_vcd_weighted(In, Kernel, GSq, x, y);
        for(int x=wid-N2; x<wid; x++)
            Out(x,y) = sample_kernel_vcd_weighted(In, Kernel, GSq, x, y);
    }

    if(N == 5)
        ConvolveMiddleVCDN<Image_T, KernelImage_T, GaussSqFunctor_T, 5>(Out, In, Kernel, GSq); // If kernel size is known at compile time.
    else if(N == 9)
        ConvolveMiddleVCDN<Image_T, KernelImage_T, GaussSqFunctor_T, 9>(Out, In, Kernel, GSq); // If kernel size is known at compile time.
    else
        ConvolveMiddleVCD(Out, In, Kernel, GSq); // If kernel size is not known at compile time.
}

// FiltWid x FiltWid variable conductance diffusion blur for any image type. Each pixel's contribution is
// modulated by the gaussian and the color space distance from the target pixel.
// FiltWid must be odd. Repeat iterations times.
template <class Image_T>
void VCD(Image_T &Out, const Image_T &In, const int FiltWid, const typename Image_T::PixType::MathType ImageStDev,
         const typename Image_T::PixType::MathType ColorStDev, const int iterations)
{
    ASSERT_R((FiltWid & 1) && FiltWid >= 3); // Filter must be an odd width so it can center on a pixel.
    ASSERT_R(ImageStDev > 0 && ColorStDev > 0);

    typedef tPixel<typename Image_T::PixType::ElType, 1> KernelPixelType;
    typedef tImage<KernelPixelType> KernelImageType;

    KernelImageType Kernel = MakeGaussianKernel<KernelImageType>(FiltWid, ImageStDev);
    GaussSqFunc_t<typename Image_T::PixType::ElType> GSF(ColorStDev);

    ConvolveImageVCD(Out, In, Kernel, GSF);

    for(int i=1; i<iterations; i++) {
        Image_T Tmp(Out);
        ConvolveImageVCD(Out, Tmp, Kernel, GSF);
    }
}

template void VCD<f1Image>(f1Image &Out, const f1Image &In, const int FiltWid, float ImageStDev, float ColorStDev, int);
template void VCD<f3Image>(f3Image &Out, const f3Image &In, const int FiltWid, float ImageStDev, float ColorStDev, int);
