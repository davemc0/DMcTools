//////////////////////////////////////////////////////////////////////
// MiscMath.h - Assorted mathematical functions
//
// Changes Copyright David K. McAllister, Dec. 1998.
// Originally written by Steven G. Parker, Mar. 1994.

#ifndef _MiscMath_h
#define _MiscMath_h

#include "toolconfig.h"

#include <Util/Utils.h>

#include <limits>

#include <math.h>
#include <float.h>

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

inline int IsNaN(const double d)
{
#ifdef DMC_MACHINE_win
    return _isnan(d);
#else
    return isnan(d);
#endif
}

inline int Finite(const double d)
{
#ifdef DMC_MACHINE_win
    return _finite(d);
#else
    return finite(d);
#endif
}

inline double DtoR(const double d)
{
    return d*M_PI/180.;
}

inline double RtoD(const double r)
{
    return r*180./M_PI;
}

template<class T>
inline T Sqrt(const T &v)
{
    return static_cast<T>(sqrt(v));
}

template<class T>
inline float Sqrt(const float &v)
{
    return static_cast<float>(sqrtf(v));
}

inline double Cbrt(const double d)
{
#ifdef DMC_MACHINE_win
#define ONE_THIRD (1./3.)
    return (d<0) ? -pow(-d, ONE_THIRD) : pow(d, ONE_THIRD);
#else
    return cbrt(d);
#endif
}

inline double Exp10(const double p)
{
    return pow(10.0, p);
}

template<class T>
inline T Sqr(const T x)
{
    return x*x;
}

template<class T>
inline T Cube(const T x)
{
    return x*x*x;
}

// The sqrt of 2 pi.
#define SQRT2PI 2.506628274631000502415765284811045253006
#define ONEOVERSQRT2PI (1. / SQRT2PI)

// Compute the gaussian with sigma and mu at value x.
// exp(-0.5 * Sqr(((x-mu)/sigma))) / (SQRT2PI * sigma);
// Gaussian(x,mu,sigma) = exp(-0.5 * Sqr(x-mu) / Sqr(sigma)) / (sigma * sqrt(2*pi))
inline double Gaussian(const double x, const double sigma, const double mu=0)
{
    double oneOverSigma = 1. / sigma;
    return exp(-0.5 * Sqr(((x-mu) * oneOverSigma))) * ONEOVERSQRT2PI * oneOverSigma;
}

// Symmetric gaussian centered at origin.
// No covariance matrix. Give it X and Y.
inline double Gaussian2(const double x, const double y, const double sigma)
{
    double oneOverSigma = 1. / sigma;
    return exp(-0.5 * (Sqr(x) + Sqr(y)) * Sqr(oneOverSigma)) * ONEOVERSQRT2PI * oneOverSigma;
}

// Compute the gaussian at value x with given sigma and mu=0.
// Useful when we don't want to take the sqrt of x before calling.
// Gaussian(x,mu,sigma) = exp(-0.5 * Sqr(x-mu) / Sqr(sigma)) / (sigma * sqrt(2*pi))
inline double GaussianSq(const double xSq, const double sigma)
{
    double oneOverSigma = 1. / sigma;
    return exp(-0.5 * xSq * Sqr(oneOverSigma)) * ONEOVERSQRT2PI * oneOverSigma;
}

/// Return a random number with a normal distribution.
inline double NRand(const double sigma = 1.0)
{
    double x, y, r2;
    do {
        x = DRand()*2.0-1.0;
        y = DRand()*2.0-1.0;
        r2 = x*x+y*y;
    }
    while(r2 > 1.0 || r2 == 0.0);

    double m = sqrt(-2.0 * log(r2)/r2);

    double px = x*m*sigma;
    // double py = y*m*sigma;

    return px;
}

inline bool IsPow2(const int x)
{
    return (x & (x-1)) == 0;
}

// Absolute value
inline double Abs(const double d)
{
#ifdef DMC_MACHINE_win
    return fabs(d);
#else
    return d<0?-d:d;
#endif
}

inline int Abs(const int i)
{
#ifdef DMC_MACHINE_win
    return abs(i);
#else
    return i<0?-i:i;
#endif
}

// Signs
inline int Sign(const double d)
{
    return d<0?-1:1;
}

inline int Sign(const int i)
{
    return i<0?-1:1;
}

// Clamp a number to a specific range
template<class T>
inline T Clamp(const T minv, const T d, const T maxv=numeric_limits<T>::max())
{
    return d<=minv?minv:(d>=maxv?maxv:d);
}

// Two Values
template<class T>
inline T Min(const T d1, const T d2) {return d1<d2?d1:d2;}
template<class T>
inline T Max(const T d1, const T d2) {return d1>d2?d1:d2;}

// Three Values
template<class T>
inline T Min(const T d1, const T d2, const T d3) {return d1<d2?(d1<d3?d1:d3):(d2<d3?d2:d3);}
template<class T>
inline T Max(const T d1, const T d2, const T d3) {return d1>d2?(d1>d3?d1:d3):(d2>d3?d2:d3);}

// Cubic Hermite Interpolation
// Generate a step between minv and maxv.
// return: 0 - if d<=minv
// . . . . 1 - if d>=maxv
// hermite curve if d>minv && d<maxv
inline double CubicInterp(const double d, const double minv, const double maxv)
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

inline double SmoothStep(const double d, const double minv, const double maxv)
{
    return CubicInterp(d, minv, maxv);
}

// Linear Interpolation
inline double LinearInterp(const double d1, const double d2, const double weight)
{
    return d2*weight+d1*(1.0-weight);
}

// Linear Interpolation
inline double Interpolate(const double d1, const double d2, const double weight)
{
    return LinearInterp(d1, d2, weight);
}

// Integer/double conversions

inline int FloatToInt(const float f)
{
    return (int)f;
}

inline int DoubleToInt(const double d)
{
    return (int)d;
}

inline double Fraction(const double d)
{
    return (d>0) ? (d-DoubleToInt(d)) : (d-DoubleToInt(d+1));
}

inline int RoundDown(const double d)
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

inline int RoundUp(const double d)
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

inline int Round(const double d)
{
    return DoubleToInt(d+0.5);
}

inline int Floor(const double d)
{
    if(d>=0){
        return DoubleToInt(d);
    } else {
        return -DoubleToInt(-d);
    }
}

inline int Ceil(const double d)
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
