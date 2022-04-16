//////////////////////////////////////////////////////////////////////
// AABB.h - Implements an axis-aligned bounding box
//
// Copyright David K. McAllister, Aug. 1998, April 2016, Feb. 2022.

#pragma once

#include "Math/MiscMath.h"
#include "Math/Vector.h"
#include "Util/Assert.h"

#include <iosfwd>

template <class Vec_T> struct tAABB {
    Vec_T v[2];

    typedef Vec_T VecType; // The type of a vector low or high corner

    DMC_DECL Vec_T& lo() { return v[0]; }
    DMC_DECL Vec_T& hi() { return v[1]; }
    DMC_DECL const Vec_T& lo() const { return v[0]; }
    DMC_DECL const Vec_T& hi() const { return v[1]; }

    DMC_DECL tAABB()
    {
        lo() = DMC_MAXFLOAT;
        hi() = -DMC_MAXFLOAT;
    }
    DMC_DECL tAABB(const Vec_T& v0)
    {
        lo() = v0;
        hi() = v0;
    }
    DMC_DECL tAABB(const Vec_T& v0, const Vec_T& v1)
    {
        lo() = v0;
        hi() = v1;
    }
    DMC_DECL tAABB& operator=(const tAABB& a)
    {
        lo() = a.lo();
        hi() = a.hi();
        return *this;
    }

    DMC_DECL bool operator!=(const tAABB& a) const { return !(*this == a); }
    DMC_DECL bool operator==(const tAABB& a) const { return lo() == a.lo() && hi() == a.hi(); }
    DMC_DECL bool valid() const { return lo().x <= hi().x && lo().y <= hi().y && lo().z <= hi().z; }
    DMC_DECL bool contains(const tAABB& a) const
    {
        return (lo().x <= a.lo().x && lo().y <= a.lo().y && lo().z <= a.lo().z) && (hi().x >= a.hi().x && hi().y >= a.hi().y && hi().z >= a.hi().z);
    }
    DMC_DECL bool contains(const Vec_T& a) const
    {
        return (lo().x <= a.x && lo().y <= a.y && lo().z <= a.z) && (hi().x >= a.x && hi().y >= a.y && hi().z >= a.z);
    }
    DMC_DECL void grow(const Vec_T& p)
    {
        lo() = min(lo(), p);
        hi() = max(hi(), p);
    }

    DMC_DECL void grow(const tAABB& a)
    {
        lo() = min(lo(), a.lo());
        hi() = max(hi(), a.hi());
    }
    DMC_DECL void reset() { *this = tAABB<Vec_T>(); }
    DMC_DECL Vec_T extent() const { return hi() - lo(); }
    DMC_DECL Vec_T centroid() const { return (lo() + hi()) * (typename Vec_T::ElType)0.5; }
    DMC_DECL float volume() const { return valid() ? ((hi().x - lo().x) * (hi().y - lo().y) * (hi().z - lo().z)) : (typename Vec_T::ElType)0; }
    DMC_DECL float area() const
    {
        Vec_T e = extent();
        return valid() ? ((e.y * e.z + e.z * e.x + e.x * e.y) * (typename Vec_T::ElType)2) : (typename Vec_T::ElType)0;
    }

    // True if the sphere and this AABB intersect
    DMC_DECL bool intersectsSphere(const Vec_T& P, const typename Vec_T::ElType r) const
    {
        ASSERT_D(r >= (typename Vec_T::ElType)0);

        typename Vec_T::ElType disSq = 0;
        for (int c = 0; c < 3; c++)
            if (P[c] < lo()[c])
                disSq += sqr(P[c] - lo()[c]);
            else if (P[c] > hi()[c])
                disSq += sqr(P[c] - hi()[c]);

        return disSq <= r * r;
    }

    // Returns nearest point to P in or on this AABB
    DMC_DECL Vec_T nearest(const Vec_T& P) const { return clamp(P, lo(), hi()); }

    // Returns nearest point to P on but not in this AABB, assuming P is inside this AABB
    DMC_DECL Vec_T nearestOnSurface(const Vec_T& P) const
    {
        Vec_T NP(P);
        float dm = DMC_MAXFLOAT;
        for (int c = 0; c < 3; c++) {
            float dl = P[c] - lo()[c];
            if (dl < dm) {
                NP = P;
                NP[c] = lo()[c];
                dm = dl;
            }
            float dh = hi()[c] - P[c];
            if (dh < dm) {
                NP = P;
                NP[c] = hi()[c];
                dm = dh;
            }
        }

        return NP;
    }

    // Construct the requested one of the eight corners of the AABB
    DMC_DECL Vec_T corner(int cornerCode) const
    {
        Vec_T v;
        for (int c = 0; c < 3; c++) v[c] = (cornerCode & (1 << c)) ? hi()[c] : lo()[c];

        return v;
    }
};

/////////////////////////////////////////////////
// Utility operations

// True if a and b intersect
template <class Vec_T> DMC_DECL bool isIntersecting(const tAABB<Vec_T>& a, const tAABB<Vec_T>& b)
{
    if (a.hi().x < b.lo().x) return false;
    if (a.lo().x > b.hi().x) return false;
    if (a.hi().y < b.lo().y) return false;
    if (a.lo().y > b.hi().y) return false;
    if (a.hi().z < b.lo().z) return false;
    if (a.lo().z > b.hi().z) return false;

    return true;
}

// Return AABB that is intersection of these two
template <class Vec_T> DMC_DECL tAABB<Vec_T> isectcsg(const tAABB<Vec_T>& a, const tAABB<Vec_T>& b)
{
    tAABB<Vec_T> c;
    c.lo() = max(a.lo(), b.lo());
    c.hi() = min(a.hi(), b.hi());

    return c;
}

// Return AABB that contains these two
template <class Vec_T> DMC_DECL tAABB<Vec_T> unioncsg(const tAABB<Vec_T>& a, const tAABB<Vec_T>& b)
{
    tAABB<Vec_T> c(a);
    c.grow(b);
    return c;
}

// Translate AABB by vector
template <class Vec_T> DMC_DECL tAABB<Vec_T> operator+(const tAABB<Vec_T>& b, const Vec_T& v)
{
    tAABB<Vec_T> b2 = {b.lo() + v, b.hi() + v};
    return b2;
}

template <class Vec_T> DMC_DECL std::ostream& operator<<(std::ostream& os, const tAABB<Vec_T>& b);
template <class Vec_T> DMC_DECL std::istream& operator>>(std::istream& is, tAABB<Vec_T>& b);

typedef tAABB<f3vec> Aabb;
