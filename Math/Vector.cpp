//////////////////////////////////////////////////////////////////////
// Vector.cpp - Implements geometric vectors, esp. float three-vectors
//
// Copyright David K. McAllister, July 1999, Feb. 2022.

#include "Math/Vector.h"

#include <iostream>

template <class T, int L, class S> DMC_DECL std::ostream& operator<<(std::ostream& os, const tVector<T, L, S>& b)
{
    os << "{";
    const T* bp = b.getPtr();
    for (int i = 0; i < L; i++) {
        os << bp[i];
        os << ((i < L - 1) ? ", " : "}");
    }
    return os;
}
template std::ostream& operator<<(std::ostream& os, const tVector<int, 3, i3vec>& b);
template std::ostream& operator<<(std::ostream& os, const tVector<float, 3, f3vec>& b);
template std::ostream& operator<<(std::ostream& os, const tVector<double, 3, d3vec>& b);
template std::ostream& operator<<(std::ostream& os, const tVector<float, 4, f4vec>& b);

template <class T, int L, class S> DMC_DECL std::istream& operator>>(std::istream& is, tVector<T, L, S>& b)
{
    char st;
    is >> st;
    T* bp = b.getPtr();
    for (int i = 0; i < L; i++) {
        is >> bp[i];
        is >> st;
    }
    return is;
}
template DMC_DECL std::istream& operator>>(std::istream& is, tVector<int, 3, i3vec>& b);
template DMC_DECL std::istream& operator>>(std::istream& is, tVector<float, 3, f3vec>& b);
template DMC_DECL std::istream& operator>>(std::istream& is, tVector<double, 3, d3vec>& b);
