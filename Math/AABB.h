//////////////////////////////////////////////////////////////////////
// AABB.h - An axis-aligned bounding box
//
// Changes Copyright David K. McAllister, April 2016.

#pragma once

#include "Math/Vector.h"

#include <string>

template <class Vec_T> class AABB {
public:
    Vec_T m_min, m_max;

    typedef Vec_T VecType; // The type of an element of the vector.

    bool intersects(const AABB<Vec_T>& b)
    {
        if (m_max.x < b.m_min.x) return false;
        if (m_min.x > b.m_max.x) return false;
        if (m_max.y < b.m_min.y) return false;
        if (m_min.y > b.m_max.y) return false;
        if (m_max.z < b.m_min.z) return false;
        if (m_min.z > b.m_max.z) return false;

        return true;
    }
};

/////////////////////////////////////////////////
// Utility operations

template <class Vec_T> AABB<Vec_T> operator+(const AABB<Vec_T>& b, const Vec_T& v)
{
    AABB<Vec_T> b2 = {b.m_min + v, b.m_max + v};
    return b2;
}

template <class Vec_T> DMC_DECL std::ostream& operator<<(std::ostream& os, const AABB<Vec_T>& B)
{
    os << '[' << B.m_min << ", " << B.m_max << ']';

    return os;
}

typedef AABB<f3Vector> Aabb;
