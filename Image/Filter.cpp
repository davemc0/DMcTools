//////////////////////////////////////////////////////////////////////
// Filter.cpp - Stuff to filter images.
//
// This stuff is mostly deprecated by ImageAlgorithms.h.
//
// Copyright David K. McAllister, 1998.

#include <Util/Assert.h>
#include <Image/ucImage.h>
#include <Math/MiscMath.h>

// Shamelessly copied from Aly Ray Smith's Principles of Image Compositing
#define INT_MULT(a, b, t) ((t) = (a) * (b) + 0x80, ((((t)>>8) + (t))>>8))

inline int int_mult(unsigned char a, unsigned char b)
{
    int t = a * b + 0x80;
    return ((t>>8) + t);
}

// 3x3 blur with special kernel. Assumes single channel image.
void ucImage::FastBlur1()
{
    ASSERT_R(chan == 1);
    // cerr << "FastBlur1\n";
    int y;

    // Allocates space for image.
    unsigned char *P2 = new unsigned char[dsize];
    ASSERT_RM(P2, "memory alloc failed");

    // Do corners.
    {
        unsigned short C = Pix[0] << 2;
        C += Pix[1] << 2;
        C += Pix[wid] << 2;
        C += Pix[wid+1] << 1;
        C += Pix[wid+1];
        P2[0] = (unsigned char)((C + 16) / 15);

        C = Pix[wid-1] << 2;
        C += Pix[wid-2] << 2;
        C += Pix[wid+wid-1] << 2;
        C += Pix[wid+wid-2] << 1;
        C += Pix[wid+wid-2];
        P2[wid-1] = (unsigned char)((C + 16) / 15);

        int ib=(hgt-1)*wid;
        C = Pix[ib] << 2;
        C += Pix[ib+1] << 2;
        C += Pix[ib-wid] << 2;
        C += Pix[ib-wid+1] << 1;
        C += Pix[ib-wid+1];
        P2[ib] = (unsigned char)((C + 16) / 15);

        C = Pix[ib+wid-1] << 2;
        C += Pix[ib+wid-2] << 2;
        C += Pix[ib-1] << 2;
        C += Pix[ib-2] << 1;
        C += Pix[ib-2];
        P2[ib+wid-1] = (unsigned char)((C + 16) / 15);
    }

    // Do top and bottom edges.
    int it=1, ib=(hgt-1)*wid+1;
    for(; it<wid-1; ib++, it++)
    {
        // Top
        unsigned short C = Pix[it] << 2;
        C += Pix[it+1] << 2;
        C += Pix[it-1] << 2;
        C += Pix[it+wid] << 2;
        C += Pix[it+wid+1];
        C += Pix[it+wid+1] << 1;
        C += Pix[it+wid-1];
        C += Pix[it+wid-1] << 1;
        P2[it] = (unsigned char)((C + 16) / 22);

        // Bottom
        C = Pix[ib] << 2;
        C += Pix[ib+1] << 2;
        C += Pix[ib-1] << 2;
        C += Pix[ib-wid] << 2;
        C += Pix[ib-wid+1];
        C += Pix[ib-wid+1] << 1;
        C += Pix[ib-wid-1];
        C += Pix[ib-wid-1] << 1;
        P2[ib] = (unsigned char)((C + 16) / 22);
        //P2[ib] = 255;
    }

    for(y=1; y<hgt-1; y++)
    {
        int il = y*wid, ir = y*wid+wid-1;

        // Left side
        unsigned short C = Pix[il] << 2;
        C += Pix[il+1] << 2;
        C += Pix[il+wid] << 2;
        C += Pix[il-wid] << 2;
        C += Pix[il+wid+1];
        C += Pix[il+wid+1] << 1;
        C += Pix[il-wid+1];
        C += Pix[il-wid+1] << 1;
        P2[il] = (unsigned char)((C + 16) / 22);

        // Right side
        C = Pix[ir] << 2;
        C += Pix[ir-1] << 2;
        C += Pix[ir+wid] << 2;
        C += Pix[ir-wid] << 2;
        C += Pix[ir+wid-1];
        C += Pix[ir+wid-1] << 1;
        C += Pix[ir-wid-1];
        C += Pix[ir-wid-1] << 1;
        P2[ir] = (unsigned char)((C + 16) / 22);
        //P2[ir] = 255;
    }

#ifdef DMC_USE_MP
#pragma parallel
#pragma pfor schedtype(gss) local(y)
#endif
    for(y=1; y<hgt-1; y++)
    {
        int ind = y*wid+1;
        for(int x=1; x<wid-1; x++, ind++)
        {
            // Sum of weights: 343 444 343 = 32
            unsigned short C = Pix[ind] << 2;
            C += Pix[ind+1] << 2;
            C += Pix[ind-1] << 2;
            C += Pix[ind+wid] << 2;
            C += Pix[ind-wid] << 2;
            C += Pix[ind+wid+1];
            C += Pix[ind+wid+1] << 1;
            C += Pix[ind+wid-1];
            C += Pix[ind+wid-1] << 1;
            C += Pix[ind-wid+1];
            C += Pix[ind-wid+1] << 1;
            C += Pix[ind-wid-1];
            C += Pix[ind-wid-1] << 1;
            P2[ind] = (unsigned char)((C + 16) >> 5);
        }
    }

    // Hook the new image into this ucImage.
    delete[] Pix;
    Pix = P2;
}

// N is the size of ONE DIMENSION of the kernel.
// Assumes an odd kernel size. Assumes single channel image.
void ucImage::Filter1(const int N, const KERTYPE *kernel)
{
    ASSERT_R(chan == 1);

    int N2 = N/2, y;

    // Allocates space for image.
    unsigned char *P2 = new unsigned char[dsize];
    ASSERT_RM(P2, "memory alloc failed");

    // Do top and bottom edges.
    {
        for(int x=N2; x<wid-N2; x++)
        {
            for(y=0; y<N2; y++)
                P2[y*wid+x] = SampleSlow1(x, y, N, kernel);
            for(y=hgt-N2; y<hgt; y++)
                P2[y*wid+x] = SampleSlow1(x, y, N, kernel);
        }
    }

    // Do left and right edges.
    for(y=0; y<hgt; y++)
    {
        int x;
        for(x=0; x<N2; x++)
            P2[y*wid+x] = SampleSlow1(x, y, N, kernel);
        for(x=wid-N2; x<wid; x++)
            P2[y*wid+x] = SampleSlow1(x, y, N, kernel);
    }

    for(y=N2; y<hgt-N2; y++)
    {
        int y0 = y-N2;
        int y1 = y+N2;
        for(int x=N2; x<wid-N2; x++)
        {
            // Add the pixels that contribute to this one.
            int x0 = x-N2;
            int x1 = x+N2;

            unsigned int C = 0;
            int ker = 0;
            for(int yy=y0; yy <= y1; yy++)
            {
                for(int xx=x0; xx <= x1; xx++, ker++)
                {
                    C += Pix[yy*wid+xx] * kernel[ker];
                }
            }
            P2[y*wid+x] = (unsigned char)((C + 0x8000) >> 16);
        }
    }

    // Hook the new image into this ucImage.
    delete[] Pix;
    Pix = P2;
}

void ucImage::Filter3(const int N, const KERTYPE *kernel)
{
    ASSERT_R(chan == 3);

    int N2 = N/2, x, y;

    // Allocates space for color image.
    unsigned char *P2 = new unsigned char[dsize];
    ASSERT_RM(P2, "memory alloc failed");

    Pixel *Pp2 = (Pixel *)P2;
    // Do top and bottom edges.
    for(x=N2; x<wid-N2; x++)
    {
        for(y=0; y<N2; y++)
            Pp2[y*wid+x] = SampleSlow3(x, y, N, kernel);
        for(y=hgt-N2; y<hgt; y++)
            Pp2[y*wid+x] = SampleSlow3(x, y, N, kernel);
    }

    // Do left and right edges.
    for(y=0; y<hgt; y++)
    {
        for(x=0; x<N2; x++)
            Pp2[y*wid+x] = SampleSlow3(x, y, N, kernel);
        for(x=wid-N2; x<wid; x++)
            Pp2[y*wid+x] = SampleSlow3(x, y, N, kernel);
    }

    for(y=N2; y<hgt-N2; y++)
    {
        int y0 = y-N2;
        int y1 = y+N2;
        for(x=N2; x<wid-N2; x++)
        {
            // Add the pixels that contribute to this one.
            int x0 = x-N2;
            int x1 = x+N2;

            unsigned int Cr = 0, Cg = 0, Cb = 0;
            int ker = 0;
            //KERTYPE SK = 0;
            for(int yy=y0; yy <= y1; yy++)
            {
                for(int xx=x0; xx <= x1; xx++, ker++)
                {
                    Cr += Pix[(yy*wid+xx)*3] * kernel[ker];
                    Cg += Pix[(yy*wid+xx)*3+1] * kernel[ker];
                    Cb += Pix[(yy*wid+xx)*3+2] * kernel[ker];
                }
            }
            P2[(y*wid+x)*3] = (unsigned char)((Cr + 0x8000) >> 16);
            P2[(y*wid+x)*3+1] = (unsigned char)((Cg + 0x8000) >> 16);
            P2[(y*wid+x)*3+2] = (unsigned char)((Cb + 0x8000) >> 16);
        }
    }

    // Hook the new image into this ucImage.
    delete[] Pix;
    Pix = P2;
}

// N is the size of ONE DIMENSION of the kernel.
// Assumes an odd kernel size.
void ucImage::Filter(const int N, const KERTYPE *kernel)
{
    if(chan == 1)
        Filter1(N, kernel);
    else if(chan == 3)
        Filter3(N, kernel);
    else
        std::cerr << "Filtering not supported on " << chan << " channel images.\n";
}

double *MakeBlurKernel(const int N, const double sigma)
{
    double *kernel = new double[N*N];
    ASSERT_RM(kernel, "memory alloc failed");

    int N2 = N/2, x, y;

    double S = 0;
    for(y= -N2; y<=N2; y++)
    {
        for(x= -N2; x<=N2; x++)
        {
            double G = Gaussian2(x, y, sigma);
            kernel[(y + N2)*N + (x+N2)] = G;

            S += G;
        }
    }

    // normalize the kernel.
    for(y = 0; y<N; y++)
        for(x = 0; x<N; x++)
            kernel[y*N + x] /= S;

        return kernel;
}

// Make a fixed point kernel from a double kernel.
// Does NOT delete the old kernel.
KERTYPE *DoubleKernelToFixed(const int N, double *dkernel)
{
    KERTYPE *ckernel = new KERTYPE[N*N];
    ASSERT_RM(ckernel, "memory alloc failed");

    // I should multiply by 256 and clamp to 255 but I won't.
    double SD = 0;
    double SC = 0;
    for(int i=0; i<N*N; i++)
    {
        ckernel[i] = (KERTYPE)(65535.0 * dkernel[i] + 0.5);
        SD += dkernel[i];
        SC += ckernel[i];
    }

    //cerr << "Double kernel weight = " << SD << endl;
    //cerr << "Byte kernel weight = " << int(SC) << endl;

    return ckernel;
}

// Blur this image with a kernel of size N and st. dev. of sigma.
// sigma = 1 seems to work well for different N.
// Makes a gaussian of st. dev. sigma, samples it at the places on
// the kernel (1 pixel is 1 unit), and then normalizes the kernel
// to have unit mass.
void ucImage::Blur(const int N, const double sig)
{
    if(chan == 1 && sig == 0)
    {
        FastBlur1();
        return;
    }

    double sigma = sig;
    if(sig < 0)
        sigma = double(N) / 3.0;

    double *dkernel = MakeBlurKernel(N, sigma);
    KERTYPE *ckernel = DoubleKernelToFixed(N, dkernel);
    delete [] dkernel;

    Filter(N, ckernel);

    delete [] ckernel;
}

// Upsample linearly.
void ucImage::HorizFiltLinear(unsigned char *Ld, int wd,
                            unsigned char *Ls, int ws)
{
    int xs = 0, xsw = 0, xdw = 0;
    for(int xd=0; xd<wd; xd++, xdw += wd)
    {
        // Sample from xs and xs+1 into xd.
        for(int c=0; c<chan; c++)
        {
            Ld[xd] = (Ls[xs] * (xdw-xsw)) / ws +
                (Ls[xs+1] * (xsw+ws-xdw)) / ws;
        }

        // Maybe step xs.
        if(xsw+ws < xdw)
        {
            xs++;
            xsw += ws;
        }
    }
}

// Rescales the image to the given size.
void ucImage::Resize(const int w, const int h)
{
    wid = w;
    hgt = h;
    size = wid * hgt;
    dsize = wid * hgt * chan;
}

static inline float CubicFilter(float x)
{
    x = fabs(x);

    /*
    Was:

    const float b = 1.0/3.0;
    const float c = 1.0/3.0;

    // Heinously inefficient code!  I hope the compiler does constant folding.

    if (x < 1)
        return ((12 - 9*b - 6*c)*pow(x,3) +
        (-18 + 12*b + 6*c)*pow(x,2) +
        6 - 2*b) / 6;
    else
        return ((-b - 6*c)*pow(x,3) +
        (6*b + 30*c)*pow(x,2) +
        (-12*b - 48*c)*x + 8*b +
        24*c) / 6;
    */

    float x2 = x * x;
    float x3 = x2 * x;

    if (x < 1)
        return 7.0/6.0 * x3 - 2.0 * x2 + 8.0/9.0;
    else
        return -7.0/18.0 * x3 + 2.0 * x2 - 10.0/3.0 * x + 16.0/9.0;
}

bool ucImage::sample(float x, float y, float &res) const
{
    int il, ih, jl, jh;

    // Hard-coded for cubic right now.

    il = (int) ceil(x) - 2;
    ih = Min(il + 4, wid);
    il = Max(il, 0);
    jl = (int) ceil(y) - 2;
    jh = Min(jl + 4, hgt);
    jl = Max(jl, 0);

    if (il >= ih || jl >= jh)
        return false;

    res = 0;
    int iO, jO;
    for (jO = jl; jO < jh; jO++) {
        for (iO = il; iO < ih; iO++) {
            float dx, dy;
            dx = (float) iO - x;
            dy = (float) jO - y;
            res += CubicFilter(dx) * CubicFilter(dy) * ch(iO, jO);
        }
    }

    return true;
}

