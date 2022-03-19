//////////////////////////////////////////////////////////////////////
// MiscMath.h - Assorted mathematical functions
//
// Changes Copyright David K. McAllister, 1998-2007, 2022.

#pragma once

#include "Util/Utils.h"
#include "Util/toolconfig.h"

#include <cfloat>
#include <cmath>
#include <limits>

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

namespace dmcm {
// Absolute value
DMC_DECL double Abs(const double d)
{
#ifdef DMC_MACHINE_win
    return fabs(d);
#else
    return d < 0 ? -d : d;
#endif
}

DMC_DECL float Abs(const float d)
{
#ifdef DMC_MACHINE_win
    return fabs(d);
#else
    return d < 0 ? -d : d;
#endif
}

DMC_DECL int Abs(const int i)
{
#ifdef DMC_MACHINE_win
    return abs(i);
#else
    return i < 0 ? -i : i;
#endif
}

// Three Values
template <class T> DMC_DECL T Min(const T d1, const T d2, const T d3) { return d1 < d2 ? (d1 < d3 ? d1 : d3) : (d2 < d3 ? d2 : d3); }
template <class T> DMC_DECL T Max(const T d1, const T d2, const T d3) { return d1 > d2 ? (d1 > d3 ? d1 : d3) : (d2 > d3 ? d2 : d3); }

// DMC_DECL float fastExp2(int a) { return uintAsFloat((uint32_t)(std::min(std::max(a + 127, 1), 254) << 23)); }
DMC_DECL float fastMax(float a, float b) { return (a + b + abs(a - b)) * 0.5f; }
DMC_DECL float fastMin(float a, float b) { return (a + b - abs(a - b)) * 0.5f; }

template <class T> DMC_DECL T Round(const T d)
{
    // Round appears to be missing in VS 2012 but is in 2013.
    return floor(d + static_cast<T>(0.5));
}

// Clamp a number to a specific range
template <class T> DMC_DECL T Clamp(const T minv, const T d, const T maxv = std::numeric_limits<T>::max()) { return d <= minv ? minv : (d >= maxv ? maxv : d); }

// Clamp a number to 0..1, i.e. a linear ramp on 0..1
template <class T> DMC_DECL T Saturate(const T d) { return d <= 0 ? 0 : (d >= 1 ? 1 : d); }

// Cubic Hermite Interpolation
// Generate a step between minv and maxv.
// return: 0 - if d<=minv
// . . . . 1 - if d>=maxv
// Hermite curve if d>minv && d<maxv
template <class T> DMC_DECL T SmoothStep(const T d, const T minv, const T maxv)
{
    T ret;
    if (d <= minv) {
        ret = 0;
    } else if (d >= maxv) {
        ret = 1;
    } else {
        T t = (d - minv) / (maxv - minv);
        ret = t * t * (3 - 2 * t);
    }
    return ret;
}

// Linear Interpolation
template <class T> DMC_DECL T LinearInterp(const T d1, const T d2, const T weight) { return (d2 - d1) * weight + d1; }

DMC_DECL double Sqrt(const double& v) { return sqrt(v); }
DMC_DECL float Sqrt(const float& v) { return sqrtf(v); }

DMC_DECL double Cbrt(const double d)
{
#ifdef DMC_MACHINE_win
#define ONE_THIRD (1. / 3.)
    return (d < 0) ? -pow(-d, ONE_THIRD) : pow(d, ONE_THIRD);
#else
    return cbrt(d);
#endif
}

DMC_DECL float Cbrt(const float d)
{
#ifdef DMC_MACHINE_win
#define ONE_THIRDF (1.f / 3.f)
    return (d < 0) ? -powf(-d, ONE_THIRDF) : powf(d, ONE_THIRDF);
#else
    return cbrtf(d);
#endif
}

DMC_DECL double Sqr(const double x) { return x * x; }
DMC_DECL float Sqr(const float x) { return x * x; }
DMC_DECL int Sqr(const int x) { return x * x; }
DMC_DECL unsigned int Sqr(const unsigned int x) { return x * x; }

DMC_DECL double Cube(const double x) { return x * x * x; }
DMC_DECL float Cube(const float x) { return x * x * x; }
DMC_DECL int Cube(const int x) { return x * x * x; }
DMC_DECL unsigned int Cube(const unsigned int x) { return x * x * x; }

DMC_DECL double DtoR(const double d) { return d * static_cast<double>(M_PI) / static_cast<double>(180); }
DMC_DECL float DtoR(const float d) { return d * static_cast<float>(M_PI) / static_cast<float>(180); }

DMC_DECL double RtoD(const double r) { return r * static_cast<double>(180) / static_cast<double>(M_PI); }
DMC_DECL float RtoD(const float r) { return r * static_cast<float>(180) / static_cast<float>(M_PI); }
} // namespace dmcm
