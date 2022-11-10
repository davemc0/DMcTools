//////////////////////////////////////////////////////////////////////
// Intersect.h - Implements ray-box and ray-triangle intersection
//
// Copyright David K. McAllister, 2022.

#pragma once

#include "Math/AABB.h"
#include "Math/Triangle.h"

template <class Vec_T, class Elem_T, class AABB_T>
DMC_DECL bool RBI(const Vec_T& orig, const Vec_T& dir, Elem_T tMin, Elem_T tMax, const AABB_T& box, Elem_T& tHit);

template <class Vec_T, class Elem_T>
DMC_DECL bool RTI(const Vec_T& orig, const Vec_T& dir, Elem_T tMin, Elem_T tMax, const tTriangle<Vec_T>& tri, bool cullFront, bool cullBack, bool frontIsCCW,
                  Elem_T& tHit, Elem_T& bary0, Elem_T& bary1, bool& isFrontFacing);
