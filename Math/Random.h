//////////////////////////////////////////////////////////////////////
// Random.h - Handles random numbers
//
// Copyright David K. McAllister, 2014.

#pragma once

#include "Math/MiscMath.h"

// The sqrt of 2 pi.
#define SQRT2PI 2.506628274631000502415765284811045253006
#define ONEOVERSQRT2PI (1. / SQRT2PI)

// A random number on 0.0 to 1.0 for float and 0 to 2^31 for int
template <class Elem_T> DMC_DECL Elem_T tRand() { return static_cast<Elem_T>((rand() << 15) | rand()) / static_cast<Elem_T>(RAND_MAX * RAND_MAX); }
template <> DMC_DECL int tRand() { return abs((rand() ^ (rand() << 15) ^ (rand() << 30))); }

template <class Elem_T> DMC_DECL Elem_T tRand(const Elem_T low, const Elem_T high) { return low + tRand<Elem_T>() * (high - low); }
template <> DMC_DECL int tRand(const int low, const int high) { return low + tRand<int>() % (high - low); }

// A random float or double on low to high
DMC_DECL float frand() { return tRand<float>(); }
DMC_DECL double drand() { return tRand<double>(); }
DMC_DECL float frand(const float low, const float high) { return low + tRand<float>() * (high - low); }
DMC_DECL double drand(const double low, const double high) { return low + tRand<double>() * (high - low); }

// A random integer
DMC_DECL int irand() { return tRand<int>(); }
DMC_DECL int irand(const int low, const int high) { return low + (irand() % (high - low)); }
DMC_DECL int irand(const int high) { return irand() % high; }

// True approximately n out of out_of times
DMC_DECL bool chance(int n, int out_of) { return (out_of > 0) ? (irand(out_of) < n) : false; }

// A random number on [0 .. out_of)
DMC_DECL int randn(int out_of) { return (out_of > 0) ? irand(out_of) : 0; }

// Compute the gaussian with sigma and mu at value x.
// exp(-0.5 * Sqr(((x-mu)/sigma))) / (SQRT2PI * sigma);
// Gaussian(x,mu,sigma) = exp(-0.5 * Sqr(x-mu) / Sqr(sigma)) / (sigma * sqrt(2*pi))
DMC_DECL double Gaussian(const double x, const double sigma, const double mu = 0)
{
    double oneOverSigma = 1. / sigma;
    return exp(-0.5 * sqr(((x - mu) * oneOverSigma))) * ONEOVERSQRT2PI * oneOverSigma;
}

// Symmetric gaussian centered at origin.
// No covariance matrix. Give it X and Y.
template <class T> DMC_DECL T Gaussian2(const T x, const T y, const T sigma)
{
    T oneOverSigma = static_cast<T>(1) / sigma;
    return exp(static_cast<T>(-0.5) * (sqr(x) + sqr(y)) * sqr(oneOverSigma)) * static_cast<T>(ONEOVERSQRT2PI) * oneOverSigma;
}

// Compute the gaussian at value x with given sigma and mu=0.
// Useful when we don't want to take the sqrt of x before calling.
// Gaussian(x,mu,sigma) = exp(-0.5 * Sqr(x-mu) / Sqr(sigma)) / (sigma * sqrt(2*pi))
DMC_DECL double GaussianSq(const double xSq, const double sigma)
{
    double oneOverSigma = 1. / sigma;
    return exp(-0.5 * xSq * sqr(oneOverSigma)) * ONEOVERSQRT2PI * oneOverSigma;
}

// Return a random number with a normal distribution
template <class Elem_T> DMC_DECL Elem_T TNRand(const Elem_T sigma = 1)
{
    Elem_T x, y, r2;
    do {
        x = tRand<Elem_T>(-1.0, 1.0);
        y = tRand<Elem_T>(-1.0, 1.0);
        r2 = x * x + y * y;
    } while (r2 > 1.0 || r2 == 0.0);

    Elem_T m = sqrt(static_cast<Elem_T>(-2) * log(r2) / r2);

    Elem_T px = x * m * sigma;
    // Elem_T py = y*m*sigma;

    return px;
}

// Return a random number with a normal distribution
DMC_DECL double NRand(const double mu, const double sigma) { return TNRand<double>(sigma) + mu; }

// Return a random number with a normal distribution
DMC_DECL float NRandf(const float mu, const float sigma) { return TNRand<float>(sigma) + mu; }

// True approximately frac percent of the time
DMC_DECL bool chance(float frac) { return frac > frand(); }

// Seeds the random number generator based on time and/or process ID, if seed is zero (default).
// Definition of SRand() is in Utils.cpp.
extern unsigned int SRand(unsigned int seed = 0);
