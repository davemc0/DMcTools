//////////////////////////////////////////////////////////////////////
// MiscMath.h - Assorted mathematical functions
//
// Changes Copyright David K. McAllister, 1998-2007.
// Originally written by Steven G. Parker, Mar. 1994.

#ifndef dmc_MiscMath_h
#define dmc_MiscMath_h

#define NOMINMAX

#include "toolconfig.h"
#include "Util/Utils.h"

#include <limits>
#include <cmath>
#include <cfloat>

#ifndef DMC_MAXFLOAT
#define DMC_MAXFLOAT FLT_MAX
#endif

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#ifndef M_PI_2
#define M_PI_2 1.5707963267948966192313216916398
#endif

#ifndef M_SQRT2
#define M_SQRT2 1.4142135623730950488016887242097
#endif

#ifndef M_SQRT1_2
#define M_SQRT1_2 0.70710678118654752440084436210485
#endif

DMC_INLINE int IsNaN(const double d)
{
#ifdef DMC_MACHINE_win
    return _isnan(d);
#else
    return isnan(d);
#endif
}

DMC_INLINE int Finite(const double d)
{
#ifdef DMC_MACHINE_win
    return _finite(d);
#else
    return finite(d);
#endif
}

DMC_INLINE double DtoR(const double d)
{
    return d*M_PI/180.;
}

DMC_INLINE double RtoD(const double r)
{
    return r*180./M_PI;
}

DMC_INLINE double Sqrt(const double &v) { return sqrt(v); }
DMC_INLINE float Sqrt(const float &v) { return sqrtf(v); }

DMC_INLINE double Cbrt(const double d)
{
#ifdef DMC_MACHINE_win
#define ONE_THIRD (1./3.)
    return (d<0) ? -pow(-d, ONE_THIRD) : pow(d, ONE_THIRD);
#else
    return cbrt(d);
#endif
}

DMC_INLINE float Cbrt(const float d)
{
#ifdef DMC_MACHINE_win
#define ONE_THIRDF (1.f/3.f)
    return (d<0) ? -powf(-d, ONE_THIRDF) : powf(d, ONE_THIRDF);
#else
    return cbrtf(d);
#endif
}

DMC_INLINE double Exp10(const double p){ return pow(10.0, p); }

DMC_INLINE double Sqr(const double x) { return x*x; }
DMC_INLINE float Sqr(const float x) { return x*x; }
DMC_INLINE int Sqr(const int x) { return x*x; }
DMC_INLINE unsigned int Sqr(const unsigned int x) { return x*x; }

DMC_INLINE double Cube(const double x) { return x*x*x; }
DMC_INLINE float Cube(const float x) { return x*x*x; }
DMC_INLINE int Cube(const int x) { return x*x*x; }
DMC_INLINE unsigned int Cube(const unsigned int x) { return x*x*x; }

// The sqrt of 2 pi.
#define SQRT2PI 2.506628274631000502415765284811045253006
#define ONEOVERSQRT2PI (1. / SQRT2PI)

// Compute the gaussian with sigma and mu at value x.
// exp(-0.5 * Sqr(((x-mu)/sigma))) / (SQRT2PI * sigma);
// Gaussian(x,mu,sigma) = exp(-0.5 * Sqr(x-mu) / Sqr(sigma)) / (sigma * sqrt(2*pi))
DMC_INLINE double Gaussian(const double x, const double sigma, const double mu=0)
{
    double oneOverSigma = 1. / sigma;
    return exp(-0.5 * Sqr(((x-mu) * oneOverSigma))) * ONEOVERSQRT2PI * oneOverSigma;
}

// Symmetric gaussian centered at origin.
// No covariance matrix. Give it X and Y.
DMC_INLINE double Gaussian2(const double x, const double y, const double sigma)
{
    double oneOverSigma = 1. / sigma;
    return exp(-0.5 * (Sqr(x) + Sqr(y)) * Sqr(oneOverSigma)) * ONEOVERSQRT2PI * oneOverSigma;
}

// Compute the gaussian at value x with given sigma and mu=0.
// Useful when we don't want to take the sqrt of x before calling.
// Gaussian(x,mu,sigma) = exp(-0.5 * Sqr(x-mu) / Sqr(sigma)) / (sigma * sqrt(2*pi))
DMC_INLINE double GaussianSq(const double xSq, const double sigma)
{
    double oneOverSigma = 1. / sigma;
    return exp(-0.5 * xSq * Sqr(oneOverSigma)) * ONEOVERSQRT2PI * oneOverSigma;
}

/// Return a random number with a normal distribution.
DMC_INLINE double NRand(const double sigma = 1.0)
{
    double x, y, r2;
    do {
        x = DRand(-1.0, 1.0);
        y = DRand(-1.0, 1.0);
        r2 = x*x+y*y;
    }
    while(r2 > 1.0 || r2 == 0.0);

    double m = sqrt(-2.0 * log(r2)/r2);

    double px = x*m*sigma;
    // double py = y*m*sigma;

    return px;
}

/// Return a random number with a normal distribution.
DMC_INLINE double NRandf(const float sigma = 1.0f)
{
    float x, y, r2;
    do {
        x = DRandf(-1.0f, 1.0f);
        y = DRandf(-1.0f, 1.0f);
        r2 = x*x+y*y;
    }
    while(r2 > 1.0f || r2 == 0.0f);

    float m = sqrtf(-2.0f * logf(r2)/r2);

    float px = x*m*sigma;
    // float py = y*m*sigma;

    return px;
}

DMC_INLINE bool IsPow2(const int x)
{
    return (x & (x-1)) == 0;
}

// Absolute value
DMC_INLINE double Abs(const double d)
{
#ifdef DMC_MACHINE_win
    return fabs(d);
#else
    return d<0?-d:d;
#endif
}

DMC_INLINE int Abs(const int i)
{
#ifdef DMC_MACHINE_win
    return abs(i);
#else
    return i<0?-i:i;
#endif
}

// Signs
DMC_INLINE int Sign(const double d)
{
    return d<0?-1:1;
}

DMC_INLINE int Sign(const int i)
{
    return i<0?-1:1;
}

// Clamp a number to a specific range
template<class T>
DMC_INLINE T Clamp(const T minv, const T d, const T maxv=std::numeric_limits<T>::max())
{
    return d<=minv?minv:(d>=maxv?maxv:d);
}

// Clamp a number to 0..1
template<class T>
DMC_INLINE T Saturate(const T d)
{
    return d<=0?0:(d>=1?1:d);
}

#if 0
// Two Values
// Replaced by std::min
DMC_INLINE float Min(const float d1, const float d2) {return d1<d2?d1:d2;}
DMC_INLINE float Max(const float d1, const float d2) {return d1>d2?d1:d2;}
DMC_INLINE double Min(const double d1, const double d2) {return d1<d2?d1:d2;}
DMC_INLINE double Max(const double d1, const double d2) {return d1>d2?d1:d2;}
DMC_INLINE size_t Min(const size_t d1, const size_t d2) {return d1<d2?d1:d2;}
DMC_INLINE size_t Max(const size_t d1, const size_t d2) {return d1>d2?d1:d2;}
DMC_INLINE int Min(const int d1, const int d2) {return d1<d2?d1:d2;}
DMC_INLINE int Max(const int d1, const int d2) {return d1>d2?d1:d2;}
//DMC_INLINE unsigned int Min(const unsigned int d1, const unsigned int d2) {return d1<d2?d1:d2;}
//DMC_INLINE unsigned int Max(const unsigned int d1, const unsigned int d2) {return d1>d2?d1:d2;}
DMC_INLINE unsigned char Min(const unsigned char d1, const unsigned char d2) {return d1<d2?d1:d2;}
DMC_INLINE unsigned char Max(const unsigned char d1, const unsigned char d2) {return d1>d2?d1:d2;}
#endif

// Three Values
template<class T>
DMC_INLINE T Min(const T d1, const T d2, const T d3) {return d1<d2?(d1<d3?d1:d3):(d2<d3?d2:d3);}
template<class T>
DMC_INLINE T Max(const T d1, const T d2, const T d3) {return d1>d2?(d1>d3?d1:d3):(d2>d3?d2:d3);}

// Cubic Hermite Interpolation
// Generate a step between minv and maxv.
// return: 0 - if d<=minv
// . . . . 1 - if d>=maxv
// hermite curve if d>minv && d<maxv
DMC_INLINE double CubicInterp(const double d, const double minv, const double maxv)
{
    double ret;
    if(d <= minv){
        ret=0.0;
    } else if(d >= maxv){
        ret=1.0;
    } else {
        double t=(d-minv)/(maxv-minv);
        ret=t*t*(3.0-2.0*t);
    }
    return ret;
}

DMC_INLINE double SmoothStep(const double d, const double minv, const double maxv)
{
    return CubicInterp(d, minv, maxv);
}

// Linear Interpolation
DMC_INLINE double LinearInterp(const double d1, const double d2, const double weight)
{
    return d2*weight+d1*(1.0-weight);
}

// Linear Interpolation
DMC_INLINE double Interpolate(const double d1, const double d2, const double weight)
{
    return LinearInterp(d1, d2, weight);
}

// Integer/double conversions

DMC_INLINE int FloatToInt(const float f)
{
    return (int)f;
}

DMC_INLINE int DoubleToInt(const double d)
{
    return (int)d;
}

DMC_INLINE double Fraction(const double d)
{
    return (d>0) ? (d-DoubleToInt(d)) : (d-DoubleToInt(d+1));
}

DMC_INLINE int RoundDown(const double d)
{
    if(d>=0){
        return DoubleToInt(d);
    } else {
        if(d==DoubleToInt(d)){
            return -DoubleToInt(-d);
        } else {
            return -DoubleToInt(-d)-1;
        }
    }
}

DMC_INLINE int RoundUp(const double d)
{
    if(d>=0){
        if((d-DoubleToInt(d)) == 0)
            return DoubleToInt(d);
        else
            return DoubleToInt(d+1);
    } else {
        return DoubleToInt(d);
    }
}

DMC_INLINE int Round(const double d)
{
    return DoubleToInt(d+0.5);
}

DMC_INLINE int Floor(const double d)
{
    if(d>=0){
        return DoubleToInt(d);
    } else {
        return -DoubleToInt(-d);
    }
}

DMC_INLINE int Ceil(const double d)
{
    if(d==DoubleToInt(d)){
        return DoubleToInt(d);
    } else {
        if(d>0){
            return 1+DoubleToInt(d);
        } else {
            return -(1+DoubleToInt(-d));
        }
    }
}

#endif
