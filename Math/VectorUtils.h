//////////////////////////////////////////////////////////////////////
// VectorUtils.h - Implements utility functions related to geometric vectors, esp. float three-vectors
//
// Copyright David K. McAllister, July 1999, Feb. 2022.
// Some functions patterned after Steve Parker's 1995 code, some others on Timo Aila's 2010 code

#pragma once

#ifdef _WIN32
#include <intrin.h>
#endif

#include "Math/AABB.h"
#include "Math/Random.h"
#include "Math/Vector.h"

DMC_DECL f4vec fromABGR(unsigned int abgr)
{
    return f4vec((float)(abgr & 0xFF) * (1.0f / 255.0f), (float)((abgr >> 8) & 0xFF) * (1.0f / 255.0f), (float)((abgr >> 16) & 0xFF) * (1.0f / 255.0f),
                 (float)(abgr >> 24) * (1.0f / 255.0f));
}

DMC_DECL unsigned int toABGR(const f4vec& v)
{
    // 72057594037927936.f is 2^56.
    return ((((unsigned int)(((uint64_t)(clamp(v.x, 0.0f, 1.0f) * 72057594037927936.f) * 255) >> 55) + 1) >> 1) << 0) |
        ((((unsigned int)(((uint64_t)(clamp(v.y, 0.0f, 1.0f) * 72057594037927936.f) * 255) >> 55) + 1) >> 1) << 8) |
        ((((unsigned int)(((uint64_t)(clamp(v.z, 0.0f, 1.0f) * 72057594037927936.f) * 255) >> 55) + 1) >> 1) << 16) |
        ((((unsigned int)(((uint64_t)(clamp(v.w, 0.0f, 1.0f) * 72057594037927936.f) * 255) >> 55) + 1) >> 1) << 24);
}

DMC_DECL f3vec latLonToCart(float lon, float lat) { return f3vec(cosf(lat) * cosf(lon), cosf(lat) * sinf(lon), sinf(lat)); }

// Fill in two vectors that, with this one, form an orthogonal frame
// this may not be normalized, but the other two are.
template <class Vec_T> void FindOrthogonal(const Vec_T& inp, Vec_T& v1, Vec_T& v2)
{
    Vec_T v0(Cross(inp, Vec_T(1, 0, 0)));
    if (v0.lenSqr() == (typename Vec_T::ElType)0) { v0 = Cross(inp, Vec_T(0, 1, 0)); }
    v1 = Cross(inp, v0);
    v1.normalize();
    v2 = Cross(inp, v1);
    v2.normalize();
}

// Find the point p in terms of the u,v,w basis.
template <class Vec_T> DMC_DECL Vec_T Solve(const Vec_T& p, const Vec_T& u, const Vec_T& v, const Vec_T& w)
{
    typename Vec_T::ElType det = (typename Vec_T::ElType)1 /
        (w.z * u.x * v.y - w.z * u.y * v.x - u.z * w.x * v.y - u.x * v.z * w.y + v.z * w.x * u.y + u.z * v.x * w.y);

    return Vec_T(((v.x * w.y - w.x * v.y) * p.z + (v.z * w.x - v.x * w.z) * p.y + (w.z * v.y - v.z * w.y) * p.x) * det,
                 -((u.x * w.y - w.x * u.y) * p.z + (u.z * w.x - u.x * w.z) * p.y + (u.y * w.z - u.z * w.y) * p.x) * det,
                 ((u.x * v.y - u.y * v.x) * p.z + (v.z * u.y - u.z * v.y) * p.x + (u.z * v.x - u.x * v.z) * p.y) * det);
}

template <class Vec_T> DMC_DECL Vec_T linInterp(const Vec_T& v1, const Vec_T& v2, typename Vec_T::ElType t)
{
    return Vec_T(v1.x + (v2.x - v1.x) * t, v1.y + (v2.y - v1.y) * t, v1.z + (v2.z - v1.z) * t);
}

template <class Vec_T> DMC_DECL Vec_T CubicInterp(const Vec_T& v1, const Vec_T& v2, typename Vec_T::ElType f)
{
    typename Vec_T::ElType t = f * f * (3 - 2 * f);
    return Vec_T(v1.x + (v2.x - v1.x) * t, v1.y + (v2.y - v1.y) * t, v1.z + (v2.z - v1.z) * t);
}

// Given three points, find the plane (N and D) that they lie on.
template <class Vec_T> DMC_DECL void ComputePlane(const Vec_T& V0, const Vec_T& V1, const Vec_T& V2, Vec_T& N, typename Vec_T::ElType& D)
{
    Vec_T E0 = V1 - V0;
    E0.normalize();
    Vec_T E1 = V2 - V0;
    E1.normalize();
    N = Cross(E0, E1);
    N.normalize();
    D = -Dot(V0, N);
}

// Return a uniformly distributed random vector on [0..1) for float or [0..2^31) for int
template <class Vec_T> DMC_DECL Vec_T makeRand()
{
    return Vec_T(tRand<typename Vec_T::ElType>(), tRand<typename Vec_T::ElType>(), tRand<typename Vec_T::ElType>());
}

// Return a uniformly distributed random vector on scalar [low..high) for float or int
template <class Vec_T> DMC_DECL Vec_T makeRand(const typename Vec_T::ElType low, const typename Vec_T::ElType high)
{
    return Vec_T(tRand<typename Vec_T::ElType>(low, high), tRand<typename Vec_T::ElType>(low, high), tRand<typename Vec_T::ElType>(low, high));
}

// Return a uniformly distributed random vector on vector [low..high) for float or int
template <class Vec_T> DMC_DECL Vec_T makeRand(const Vec_T low, const Vec_T high)
{
    return Vec_T(tRand<typename Vec_T::ElType>(low.x, high.x), tRand<typename Vec_T::ElType>(low.y, high.y), tRand<typename Vec_T::ElType>(low.z, high.z));
}

// Return a uniformly distributed random vector on AABB box for float or int
template <class Vec_T> DMC_DECL Vec_T makeRand(const tAABB<Vec_T>& box)
{
    return Vec_T(tRand<typename Vec_T::ElType>(box.lo().x, box.hi().x), tRand<typename Vec_T::ElType>(box.lo().y, box.hi().y),
                 tRand<typename Vec_T::ElType>(box.lo().z, box.hi().z));
}

// Return a normally distributed random point
template <class Vec_T> DMC_DECL Vec_T makeNRand(const typename Vec_T::ElType sigma = 1) { return Vec_T(ndrand(0, sigma), ndrand(0, sigma), ndrand(0, sigma)); }

// Return a uniformly distributed random point on a unit spherical shell
template <class Vec_T> Vec_T makeRandOnSphere()
{
    Vec_T RVec;
    do {
        RVec = makeRand<Vec_T>(-1, 1);
    } while (RVec.lenSqr() > static_cast<typename Vec_T::ElType>(1) || RVec.lenSqr() == static_cast<typename Vec_T::ElType>(0));

    RVec.normalize();

    return RVec;
}

// Given the X and Y of a unit normal compute the +Z and return the vector
template <class Vec_T> Vec_T norm2d(const typename Vec_T::ElType x, const typename Vec_T::ElType y)
{
    Vec_T RVec(x, y, sqrt(1 - x * x - y * y));

    return RVec;
}

DMC_DECL f4vec toHomogeneous(f3vec& t)
{
    const float* tp = t.data();
    f4vec r;
    float* rp = r.data();
    for (int i = 0; i < 3; i++) rp[i] = tp[i];
    rp[3] = (float)1;
    return r;
}

DMC_DECL f3vec toCartesian(f4vec& t)
{
    const float* tp = t.data();
    f3vec r;
    float* rp = r.data();
    float c = rcp(tp[3]);
    for (int i = 0; i < 3; i++) rp[i] = tp[i] * c;
    return r;
}
