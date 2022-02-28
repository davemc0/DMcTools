//////////////////////////////////////////////////////////////////////
// AABB.cpp - Implements an axis-aligned bounding box
//
// Copyright David K. McAllister, Aug. 1998, April 2016, Feb. 2022.

#include "Math/AABB.h"

#include <iostream>

template <class Vec_T> DMC_DECL std::ostream& operator<<(std::ostream& os, const tAABB<Vec_T>& b)
{
    os << '{' << b.lo() << ", " << b.hi() << '}';

    return os;
}
template DMC_DECL std::ostream& operator<<(std::ostream& os, const tAABB<f3vec>& b);

template <class Vec_T> DMC_DECL std::istream& operator>>(std::istream& is, tAABB<Vec_T>& b)
{
    char st;
    is >> b.lo() >> st >> b.hi();

    return is;
}
template DMC_DECL std::istream& operator>>(std::istream& is, tAABB<f3vec>& b);
