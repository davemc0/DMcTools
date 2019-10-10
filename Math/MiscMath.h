//////////////////////////////////////////////////////////////////////
// MiscMath.h - Assorted mathematical functions
//
// Changes Copyright David K. McAllister, 1998-2007.

#pragma once

// For suppressing windows.h definitions of min and max.
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

namespace dmcm
{
    // Absolute value
    DMC_DECL double Abs(const double d)
    {
#ifdef DMC_MACHINE_win
        return fabs(d);
#else
        return d<0?-d:d;
#endif
    }

    DMC_DECL float Abs(const float d)
    {
#ifdef DMC_MACHINE_win
        return fabs(d);
#else
        return d<0?-d:d;
#endif
    }

    DMC_DECL int Abs(const int i)
    {
#ifdef DMC_MACHINE_win
        return abs(i);
#else
        return i<0?-i:i;
#endif
    }

    DMC_DECL float CopySign(float num, float sign)
    {
#if 0
        // If it can't find an intrinsic _copysign for single-precision.
        // This one is much faster than casting and using the double-precision implementation.
        union {
            float f;
            unsigned int i;
        } v1, v2, v3;

        v1.f = num;
        v2.f = sign;
        v3.i = (v1.i & 0x7fffffff) | (v2.i & 0x80000000);

        return v3.f;
#else
        return _copysignf(num, sign);
        // return static_cast<float>(_copysign(static_cast<float>(x), static_cast<float>(y)));
#endif
    }

    // Three Values
    template<class T>
    DMC_DECL T Min(const T d1, const T d2, const T d3) { return d1<d2 ? (d1<d3 ? d1 : d3) : (d2<d3 ? d2 : d3); }
    template<class T>
    DMC_DECL T Max(const T d1, const T d2, const T d3) { return d1>d2 ? (d1>d3 ? d1 : d3) : (d2>d3 ? d2 : d3); }

    // Clamp a number to a specific range
    template<class T>
    DMC_DECL T Clamp(const T minv, const T d, const T maxv = std::numeric_limits<T>::max())
    {
        return d <= minv ? minv : (d >= maxv ? maxv : d);
    }

    // Clamp a number to 0..1
    template<class T>
    DMC_DECL T Saturate(const T d)
    {
        return d <= 0 ? 0 : (d >= 1 ? 1 : d);
    }

    template<class T>
    DMC_DECL T Round(const T d)
    {
		// round appears to be missing in VS 2012 but is in 2013.
		return floor(d + static_cast<T>(0.5));
    }

    // Cubic Hermite Interpolation
    // Generate a step between minv and maxv.
    // return: 0 - if d<=minv
    // . . . . 1 - if d>=maxv
    // Hermite curve if d>minv && d<maxv
    template<class T>
    DMC_DECL T SmoothStep(const T d, const T minv, const T maxv)
    {
        T ret;
        if (d <= minv) {
            ret = 0;
        }
        else if (d >= maxv) {
            ret = 1;
        }
        else {
            T t = (d - minv) / (maxv - minv);
            ret = t*t*(3 - 2*t);
        }
        return ret;
    }

    // Linear Interpolation
    template<class T>
    DMC_DECL T LinearInterp(const T d1, const T d2, const T weight)
    {
        return (d2 - d1)*weight + d1;
    }

    template<class T>
    DMC_DECL bool isNaN(const T d)
    {
#ifdef DMC_MACHINE_win
        return _isnan(d)!=0;
#else
        return isnan(d)!=0;
#endif
    }

    template<class T>
    DMC_DECL bool isFinite(const T d)
    {
#ifdef DMC_MACHINE_win
        return _finite(d)!=0;
#else
        return finite(d)!=0;
#endif
    }

    DMC_DECL double Sqrt(const double &v) { return sqrt(v); }
    DMC_DECL float Sqrt(const float &v) { return sqrtf(v); }

    DMC_DECL double Cbrt(const double d)
    {
#ifdef DMC_MACHINE_win
#define ONE_THIRD (1./3.)
        return (d < 0) ? -pow(-d, ONE_THIRD) : pow(d, ONE_THIRD);
#else
        return cbrt(d);
#endif
    }

    DMC_DECL float Cbrt(const float d)
    {
#ifdef DMC_MACHINE_win
#define ONE_THIRDF (1.f/3.f)
        return (d < 0) ? -powf(-d, ONE_THIRDF) : powf(d, ONE_THIRDF);
#else
        return cbrtf(d);
#endif
    }

    DMC_DECL double Exp10(const double p){ return pow(10.0, p); }

    DMC_DECL double Sqr(const double x) { return x*x; }
    DMC_DECL float Sqr(const float x) { return x*x; }
    DMC_DECL int Sqr(const int x) { return x*x; }
    DMC_DECL unsigned int Sqr(const unsigned int x) { return x*x; }

    DMC_DECL double Cube(const double x) { return x*x*x; }
    DMC_DECL float Cube(const float x) { return x*x*x; }
    DMC_DECL int Cube(const int x) { return x*x*x; }
    DMC_DECL unsigned int Cube(const unsigned int x) { return x*x*x; }

    DMC_DECL bool IsPow2(const int x)
    {
        return (x & (x - 1)) == 0;
    }

    DMC_DECL double DtoR(const double d) { return d*static_cast<double>(M_PI) / static_cast<double>(180); }
    DMC_DECL float DtoR(const float d) { return d*static_cast<float>(M_PI) / static_cast<float>(180); }

    DMC_DECL double RtoD(const double r) { return r*static_cast<double>(180) / static_cast<double>(M_PI); }
    DMC_DECL float RtoD(const float r) { return r*static_cast<float>(180) / static_cast<float>(M_PI); }

}
