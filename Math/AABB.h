//////////////////////////////////////////////////////////////////////
// AABB.h - Implements an axis-aligned bounding box
//
// Copyright David K. McAllister, Aug. 1998, April 2016, Feb. 2022.

#pragma once

#include "Math/MiscMath.h"
#include "Math/Vector.h"

#include <iosfwd>

template <class Vec_T> struct tAABB {
    Vec_T v[2];

    typedef Vec_T VecType; // The type of a vector low or high corner

    Vec_T& lo() { return v[0]; }
    Vec_T& hi() { return v[1]; }
    const Vec_T& lo() const { return v[0]; }
    const Vec_T& hi() const { return v[1]; }

    tAABB()
    {
        lo() = DMC_MAXFLOAT;
        hi() = -DMC_MAXFLOAT;
    }
    tAABB(const Vec_T& v0, const Vec_T& v1)
    {
        lo() = v0;
        hi() = v1;
    }
    tAABB& operator=(const tAABB& a)
    {
        lo() = a.lo();
        hi() = a.hi();
        return *this;
    }

    bool operator!=(const tAABB& a) const { return !(*this == a); }
    bool operator==(const tAABB& a) const { return lo() == a.lo() && hi() == a.hi(); }
    bool valid() const { return lo().x <= hi().x && lo().y <= hi().y && lo().z <= hi().z; }
    bool contains(const tAABB& a) const
    {
        return (lo().x <= a.lo().x && lo().y <= a.lo().y && lo().z <= a.lo().z) && (hi().x >= a.hi().x && hi().y >= a.hi().y && hi().z >= a.hi().z);
    }
    bool contains(const Vec_T& a) const { return (lo().x <= a.x && lo().y <= a.y && lo().z <= a.z) && (hi().x >= a.x && hi().y >= a.y && hi().z >= a.z); }
    void grow(const Vec_T& v)
    {
        lo() = min(lo(), v);
        hi() = max(hi(), v);
    }

    void grow(const tAABB& a)
    {
        lo() = min(lo(), a.lo());
        hi() = max(hi(), a.hi());
    }
    void reset() { *this = tAABB<Vec_T>(); }
    Vec_T extent() const { return hi() - lo(); }
    Vec_T centroid() const { return (lo() + hi()) * (typename Vec_T::ElType)0.5; }
    float volume() const { return valid() ? ((hi().x - lo().x) * (hi().y - lo().y) * (hi().z - lo().z)) : (typename Vec_T::ElType)0; }
    float area() const
    {
        Vec_T e = extent();
        return valid() ? ((e.y * e.z + e.z * e.x + e.x * e.y) * (typename Vec_T::ElType)2) : (typename Vec_T::ElType)0;
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
