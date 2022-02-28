//////////////////////////////////////////////////////////////////////
// Triangle.cpp - Implements a triangle, mainly for ray tracing
//
// Copyright David K. McAllister, 2022.

#include "Math/Triangle.h"

#include <iostream>

template <class Vec_T> DMC_DECL std::ostream& operator<<(std::ostream& os, const tTriangle<Vec_T>& b)
{
    os << '{' << b.v[0] << ", " << b.v[1] << ", " << b.v[2] << '}';

    return os;
}
template DMC_DECL std::ostream& operator<<(std::ostream& os, const tTriangle<f3vec>& b);

template <class Vec_T> DMC_DECL std::istream& operator>>(std::istream& is, tTriangle<Vec_T>& b)
{
    char st;
    is >> st >> b.v[0] >> st >> b.v[1] >> st >> b.v[2] >> st;

    return is;
}
template DMC_DECL std::istream& operator>>(std::istream& is, tTriangle<f3vec>& b);
