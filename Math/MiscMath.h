//////////////////////////////////////////////////////////////////////
// MiscMath.h - Assorted mathematical functions
//
// Changes Copyright David K. McAllister, 1998-2007, 2022.

#pragma once

#include "Math/BinaryRep.h"
#include "Util/ToolConfig.h"
#include "Util/Utils.h"

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

////////////////////////////////////////////////
// Scalar templatized min/max/clamp for any type

#define SCALARTMPLMINMAX(TEMPLATE, R, T, MIN, MAX) \
    TEMPLATE DMC_DECL R min(T a, T b)              \
    {                                              \
        return MIN;                                \
    }                                              \
    TEMPLATE DMC_DECL R max(T a, T b)              \
    {                                              \
        return MAX;                                \
    }                                              \
    TEMPLATE DMC_DECL R min(T a, T b, T c)         \
    {                                              \
        return min(min(a, b), c);                  \
    }                                              \
    TEMPLATE DMC_DECL R max(T a, T b, T c)         \
    {                                              \
        return max(max(a, b), c);                  \
    }                                              \
    TEMPLATE DMC_DECL R clamp(T v, T lo, T hi)     \
    {                                              \
        return min(max(v, lo), hi);                \
    }

// This impl ensures that it will only return NaN if the 'b' is NaN; taken from: http://www.cs.utah.edu/~thiago/papers/robustBVH-v2.pdf
SCALARTMPLMINMAX(template <class T>, T&, T&, (a < b) ? a : b, (a > b) ? a : b)
SCALARTMPLMINMAX(template <class T>, const T, const T&, (a < b) ? a : b, (a > b) ? a : b)

template <class T> DMC_DECL T sqr(const T& a) { return a * a; }
template <class T> DMC_DECL T rcp(const T& a) { return (a) ? (T)1 / a : (T)0; }

DMC_DECL float fastExp2(int a) { return uintAsFloat((uint32_t)(min(max(a + 127, 1), 254) << 23)); }
DMC_DECL float fastMin(float a, float b) { return (a + b - abs(a - b)) * 0.5f; }
DMC_DECL float fastMax(float a, float b) { return (a + b + abs(a - b)) * 0.5f; }

// Clamp a number to 0..1, i.e. a linear ramp on 0..1
template <class T> DMC_DECL T saturate(const T d) { return clamp<T>(d, 0, 1); }

// Cubic Hermite interpolation between minv and maxv or 0 if d<=minv or 1 if d>=maxv
template <class T> DMC_DECL T smoothStep(const T d, const T minv, const T maxv)
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

template <class T> DMC_DECL T linInterp(const T d1, const T d2, const T weight) { return (d2 - d1) * weight + d1; }

template <class T> DMC_DECL T reflect(const T toLight, const T nrmOut) { return nrmOut * 2.f * dot(toLight, nrmOut) - toLight; }

DMC_DECL double degToRad(const double d) { return d * static_cast<double>(M_PI) / static_cast<double>(180); }
DMC_DECL float degToRad(const float d) { return d * static_cast<float>(M_PI) / static_cast<float>(180); }

DMC_DECL double radToDeg(const double r) { return r * static_cast<double>(180) / static_cast<double>(M_PI); }
DMC_DECL float radToDeg(const float r) { return r * static_cast<float>(180) / static_cast<float>(M_PI); }
