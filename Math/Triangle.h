//////////////////////////////////////////////////////////////////////
// Triangle.h - Implements a triangle, mainly for ray tracing
//
// Copyright David K. McAllister, 2022.

// TODO: Merge this with triangles used in Model code

#pragma once

#include "Math/AABB.h"
#include "Math/Vector.h"

template <class Vec_T> struct tTriangle {
    Vec_T v[3];

    DMC_DECL Vec_T::ElType area()
    {
        Vec_T E1 = v[1] - v[0];
        Vec_T E2 = v[2] - v[1];

        return fabs(Cross(E1, E2).length() * (typename Vec_T::ElType)0.5);
    }

    DMC_DECL Vec_T centroid() { return (v[0] + v[1] + v[2]) * (typename Vec_T::ElType)0.3333333333; }

    DMC_DECL tAABB<Vec_T> bbox()
    {
        tAABB<Vec_T> b(v[0]);
        b.grow(v[1]);
        b.grow(v[2]);

        return b;
    }
};

template <class Vec_T> DMC_DECL std::ostream& operator<<(std::ostream& os, const tTriangle<Vec_T>& b);
template <class Vec_T> DMC_DECL std::istream& operator>>(std::istream& is, tTriangle<Vec_T>& b);

typedef tTriangle<f3vec> Triangle;
