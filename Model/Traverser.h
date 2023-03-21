#pragma once

#include "Math/Triangle.h"
#include "Model/BVH.h"

#include <algorithm>

class Traverser {
public:
    Traverser(const BVH& _bvh) : m_bvh(_bvh) {}

    // Trace the ray against the BVH and return the hit
    template <class Vec_T, class Elem_T>
    std::tuple<bool, float, float, float, bool, int> traceRay(const Vec_T& orig, const Vec_T& dir, Elem_T tMin, Elem_T tMax);

private:
    template <class Vec_T, class Elem_T> auto rayNodeIntersect(const Vec_T& orig, const Vec_T& dir, Elem_T tMin, Elem_T tMax, const BVHNode& node);
    const BVH& m_bvh; // Ref to user's BVH
};
