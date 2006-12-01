//////////////////////////////////////////////////////////////////////
// MiscMath.h - Assorted mathematical functions
//
// Changes Copyright David K. McAllister, Dec. 1998.
// Originally written by Steven G. Parker, Mar. 1994.

#ifndef _MiscMath_h
#define _MiscMath_h

#include "toolconfig.h"

#include <Util/Utils.h>

#include <math.h>

#include <float.h>

#ifndef DMC_MAXFLOAT
#define DMC_MAXFLOAT FLT_MAX
// MAXFLOAT
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

inline double Pow(const double d, const double p)
{
	return pow(d,p);
}

inline int Sqrt(const int i)
{
	return (int)sqrt((double)i);
}

inline double Sqrt(const double d)
{
	return sqrt(d);
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

inline double Exp(const double d)
{
	return exp(d);
}

inline double Exp10(const double p)
{
	return pow(10.0, p);
}

inline double Hypot(const double x, const double y)
{
	return hypot(x, y);
}

inline double Sqr(const double x)
{
	return x*x;
}

inline int Sqr(const int x)
{
	return x*x;
}

inline double Cube(const double x)
{
	return x*x*x;
}

inline int Cube(const int x)
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

// Return a random number with a normal distribution.
inline double NRand(const double sigma = 1.0)
{
    if(sigma <= 0)
        return 0;

#define ONE_OVER_SIGMA_EXP (1.0f / 0.7975f)
	
	double y;
	do
	{
		y = -log(DRand());
	}
	while(DRand() > exp(-Sqr(y-1)*0.5));
	
	if(LRand() & 0x1)
		return y * sigma * ONE_OVER_SIGMA_EXP;
	else
		return -y * sigma * ONE_OVER_SIGMA_EXP;
}

inline bool IsPow2(const int x)
{
	int ones = 0;
	int xx=x;
	while(xx) {
		ones += xx & 1;
		xx = xx >> 1;
	}
	return ones < 2;
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
#include <limits>
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
inline double Clamp(const double minv, const double d, const double maxv=DMC_MAXFLOAT)
{
	return d<=minv?minv:d>=maxv?maxv:d;
}

inline int Clamp(const int minv, const int i, const int maxv=0x7fffffff)
{
	return i<minv?minv:i>maxv?maxv:i;
}

// Two Values
inline char Min(const char d1, const char d2) {return d1<d2?d1:d2;}
inline char Max(const char d1, const char d2) {return d1>d2?d1:d2;}
inline short Min(const short d1, const short d2) {return d1<d2?d1:d2;}
inline short Max(const short d1, const short d2) {return d1>d2?d1:d2;}
inline int Min(const int d1, const int d2) {return d1<d2?d1:d2;}
inline int Max(const int d1, const int d2) {return d1>d2?d1:d2;}
inline unsigned char Min(const unsigned char d1, const unsigned char d2) {return d1<d2?d1:d2;}
inline unsigned char Max(const unsigned char d1, const unsigned char d2) {return d1>d2?d1:d2;}
inline unsigned short Min(const unsigned short d1, const unsigned short d2) {return d1<d2?d1:d2;}
inline unsigned short Max(const unsigned short d1, const unsigned short d2) {return d1>d2?d1:d2;}
inline unsigned int Min(const unsigned int d1, const unsigned int d2) {return d1<d2?d1:d2;}
inline unsigned int Max(const unsigned int d1, const unsigned int d2) {return d1>d2?d1:d2;}
inline float Min(const float d1, const float d2) {return d1<d2?d1:d2;}
inline float Max(const float d1, const float d2) {return d1>d2?d1:d2;}
inline double Min(const double d1, const double d2) {return d1<d2?d1:d2;}
inline double Max(const double d1, const double d2) {return d1>d2?d1:d2;}

// Three Values
inline char Min(const char d1, const char d2, const char d3) {return d1<d2?(d1<d3?d1:d3):(d2<d3?d2:d3);}
inline char Max(const char d1, const char d2, const char d3) {return d1>d2?(d1>d3?d1:d3):(d2>d3?d2:d3);}
inline short Min(const short d1, const short d2, const short d3) {return d1<d2?(d1<d3?d1:d3):(d2<d3?d2:d3);}
inline short Max(const short d1, const short d2, const short d3) {return d1>d2?(d1>d3?d1:d3):(d2>d3?d2:d3);}
inline int Min(const int d1, const int d2, const int d3) {return d1<d2?(d1<d3?d1:d3):(d2<d3?d2:d3);}
inline int Max(const int d1, const int d2, const int d3) {return d1>d2?(d1>d3?d1:d3):(d2>d3?d2:d3);}
inline unsigned char Min(const unsigned char d1, const unsigned char d2, const unsigned char d3) {return d1<d2?(d1<d3?d1:d3):(d2<d3?d2:d3);}
inline unsigned char Max(const unsigned char d1, const unsigned char d2, const unsigned char d3) {return d1>d2?(d1>d3?d1:d3):(d2>d3?d2:d3);}
inline unsigned short Min(const unsigned short d1, const unsigned short d2, const unsigned short d3) {return d1<d2?(d1<d3?d1:d3):(d2<d3?d2:d3);}
inline unsigned short Max(const unsigned short d1, const unsigned short d2, const unsigned short d3) {return d1>d2?(d1>d3?d1:d3):(d2>d3?d2:d3);}
inline unsigned int Min(const unsigned int d1, const unsigned int d2, const unsigned int d3) {return d1<d2?(d1<d3?d1:d3):(d2<d3?d2:d3);}
inline unsigned int Max(const unsigned int d1, const unsigned int d2, const unsigned int d3) {return d1>d2?(d1>d3?d1:d3):(d2>d3?d2:d3);}
inline float Min(const float d1, const float d2, const float d3) {return d1<d2?(d1<d3?d1:d3):(d2<d3?d2:d3);}
inline float Max(const float d1, const float d2, const float d3) {return d1>d2?(d1>d3?d1:d3):(d2>d3?d2:d3);}
inline double Min(const double d1, const double d2, const double d3) {return d1<d2?(d1<d3?d1:d3):(d2<d3?d2:d3);}
inline double Max(const double d1, const double d2, const double d3) {return d1>d2?(d1>d3?d1:d3):(d2>d3?d2:d3);}

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
inline double Fraction(const double d)
{
	return (d>0) ? (d-(int)d) : (d-(int)d+1);
}

inline int RoundDown(const double d)
{
	if(d>=0){
		return (int)d;
	} else {
		if(d==(int)d){
			return -(int)(-d);
		} else {
			return -(int)(-d)-1;
		}
	}
}

inline int RoundUp(const double d)
{
	if(d>=0){
		if((d-(int)d) == 0)
			return (int)d;
		else
			return (int)(d+1);
	} else {
		return (int)d;
	}
}

inline int Round(const double d)
{
	return (int)(d+0.5);
}


inline int Floor(const double d)
{
	if(d>=0){
		return (int)d;
	} else {
		return -(int)(-d);
	}
}

inline int Ceil(const double d)
{
	if(d==(int)d){
		return (int)d;
	} else {
		if(d>0){
			return 1+(int)d;
		} else {
			return -(1+(int)(-d));
		}
	}
}

#if 0
inline int Tile(const int tile, const int tf)
{
	if(tf<0){
		// Tile in negative direction
		if(tile>tf && tile<=0)return 1;
		else return 0;
	} else if(tf>0){
		// Tile in positive direction
		if(tile<tf && tile>=0)return 1;
		else return 0;
	} else {
		return 1;
	}
}
#endif

#endif
