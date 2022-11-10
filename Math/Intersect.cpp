//////////////////////////////////////////////////////////////////////
// Intersect.cpp - Implements ray-box and ray-triangle intersection
//
// Copyright David K. McAllister, 2022.

#include "Math/Intersect.h"

#include "Math/AABB.h"
#include "Math/Triangle.h"
#include "Math/Vector.h"

#include <cmath>

// True on hit; false on miss
template <class Vec_T, class Elem_T, class AABB_T>
DMC_DECL bool RBI(const Vec_T& orig, const Vec_T& dir, Elem_T tMin, Elem_T tMax, const AABB_T& box, Elem_T& tHit)
{
    // Spec says that if any part of ray is Nan we can make it a miss. Just test tMin, tMax; other components NaN will yield a miss anyway.
    if (orig.any_nan() || std::isnan(tMax) || std::isnan(tMin)) return false;

    if (box.lo().any_nan() || box.hi().any_nan()) return false;

    if (tMax < 0.f) return false;

    Vec_T iDir(1.f / dir.x, 1.f / dir.y, 1.f / dir.z);

    if (iDir.any_nan() || iDir.all_inf() || iDir.any_zero()) return false;

    Elem_T ulp = std::numeric_limits<Elem_T>::epsilon();
    Elem_T roundDown = 1.f - ulp * 3.f;
    Elem_T roundUp = 1.f + ulp * 3.f;

    int dirSign[3] = {iDir.x < 0.f ? 1 : 0, iDir.y < 0.f ? 1 : 0, iDir.z < 0.f ? 1 : 0};
    Vec_T tNear = {(box.v[dirSign[0]].x - orig.x) * iDir.x, (box.v[dirSign[1]].y - orig.y) * iDir.y, (box.v[dirSign[2]].z - orig.z) * iDir.z};
    Vec_T tFar = {(box.v[1 - dirSign[0]].x - orig.x) * iDir.x, (box.v[1 - dirSign[1]].y - orig.y) * iDir.y, (box.v[1 - dirSign[2]].z - orig.z) * iDir.z};

    // Replace NaN with Infinity to prevent it being chosen by min/max
    Elem_T inf = std::numeric_limits<Elem_T>::infinity();
    if (std::isnan(tNear.x)) tNear.x = -inf;
    if (std::isnan(tNear.y)) tNear.y = -inf;
    if (std::isnan(tNear.z)) tNear.z = -inf;
    if (std::isnan(tFar.x)) tFar.x = inf;
    if (std::isnan(tFar.y)) tFar.y = inf;
    if (std::isnan(tFar.z)) tFar.z = inf;

    // Adjust box hits conservatively
    Elem_T tNearMax = tNear.max() * roundDown;
    Elem_T tFarMin = tFar.min() * roundUp;

    tHit = max(tNearMax, tMin);

    return tHit <= min(tFarMin, tMax);
}
template DMC_DECL bool RBI(const f3vec& orig, const f3vec& dir, float tMin, float tMax, const tAABB<f3vec>& box, float& tHit);

// True on hit; false on miss
template <class Vec_T, class Elem_T>
DMC_DECL bool RTI(const Vec_T& orig, const Vec_T& dir, Elem_T tMin, Elem_T tMax, const tTriangle<Vec_T>& tri, bool cullFront, bool cullBack, bool frontIsCCW,
                  Elem_T& tHit, Elem_T& bary0, Elem_T& bary1, bool& isFrontFacing)
{
    // Spec says that if any part of ray is Nan we can make it a miss. Just test tMin, tMax; other components NaN will yield a miss anyway.
    if (std::isnan(tMax) || std::isnan(tMin)) return false;

    // If any part of triangle is Nan it's a miss.
    for (int k = 0; k < 3; k++)
        if (tri.v[k].any_nan()) return false;

    if (tMax < 0.f) return false;

    // Max dir dimension will be Z. X and Y follow by right-hand rule.
    int kz = abs(dir).maxDim();
    int kx = (kz + 1) % 3;
    int ky = (kx + 1) % 3;

    // Preserve winding by swapping X and Y if Z is negative
    if (dir[kz] < 0.f) std::swap(kx, ky);

    // Shear constants
    Elem_T Sx = dir[kx] / dir[kz];
    Elem_T Sy = dir[ky] / dir[kz];
    Elem_T Sz = 1.0f / dir[kz];

    // Translate triangle so ray origin is at space origin
    Vec_T A = tri.v[0] - orig;
    Vec_T B = tri.v[1] - orig;
    Vec_T C = tri.v[2] - orig;

    // Shear and scale the X,Y vertex coordinates
    Elem_T Ax = A[kx] - Sx * A[kz];
    Elem_T Ay = A[ky] - Sy * A[kz];
    Elem_T Bx = B[kx] - Sx * B[kz];
    Elem_T By = B[ky] - Sy * B[kz];
    Elem_T Cx = C[kx] - Sx * C[kz];
    Elem_T Cy = C[ky] - Sy * C[kz];

    // Scale Z coordinates of vertices
    Elem_T Az = Sz * A[kz];
    Elem_T Bz = Sz * B[kz];
    Elem_T Cz = Sz * C[kz];

    // Scaled barycentric coordinates
    Elem_T U = Cx * By - Cy * Bx;
    Elem_T V = Ax * Cy - Ay * Cx;
    Elem_T W = Bx * Ay - By * Ax;

    // Edge tests
    if (cullBack) {
        if (U < 0.f || V < 0.f || W < 0.f) return false;
    } else if (cullFront) {
        if (U > 0.f || V > 0.f || W > 0.f) return false;
    } else {
        if ((U > 0.f || V > 0.f || W > 0.f) && (U < 0.f || V < 0.f || W < 0.f)) return false;
    }

    // Determinant
    Elem_T det = U + V + W;

    if (det == 0.f) return false;

    // Hit distance
    Elem_T T = U * Az + V * Bz + W * Cz;

    Elem_T iDet = 1.f / det;
    tHit = T * iDet;
    bary0 = V * iDet;
    bary1 = W * iDet;

    // For testing purposes only: bary0 = bary1 = 0;

    // Miss if tHit is Inf or NaN
    if (std::isinf(tHit) || std::isnan(tHit)) return false;

    // Apply T range
    if (tMin >= tHit || tMax <= tHit) return false;

    isFrontFacing = det > 0.f;
    if (frontIsCCW) isFrontFacing = !isFrontFacing;

    // Handle watertightness by addressing cases where a barycentric == 0, meaning that the ray hits an edge
    // Use the top-left rule, just like in rasterization: https://fgiesen.wordpress.com/2013/02/08/triangle-rasterization-in-practice
    bool edgeHit = false, vertHit = false;

    // For vertex hits set up v0 as the hit vertex and v1, v2 as the others
    // For edge cases set up v0, v1 for analyzing top-left rule
    Elem_T v0x = 0, v0y = 0, v1x = 0, v1y = 0, v2x = 0, v2y = 0;

    if (U == (Elem_T)0) {
        if (V == (Elem_T)0) {
            if (W == (Elem_T)0) {
                // Can't have all three == 0
            } else {
                vertHit = true;
                v0x = Cx;
                v0y = Cy;
                v1x = Ax;
                v1y = Ay;
                v2x = Bx;
                v2y = By;
            }
        } else {
            if (W == (Elem_T)0) {
                vertHit = true;
                v0x = Bx;
                v0y = By;
                v1x = Cx;
                v1y = Cy;
                v2x = Ax;
                v2y = Ay;
            } else {
                edgeHit = true;
                v0x = Bx;
                v0y = By;
                v1x = Cx;
                v1y = Cy;
            }
        }
    } else {
        if (V == (Elem_T)0) {
            if (W == (Elem_T)0) {
                vertHit = true;
                v0x = Ax;
                v0y = Ay;
                v1x = Bx;
                v1y = By;
                v2x = Cx;
                v2y = Cy;
            } else {
                edgeHit = true;
                v0x = Cx;
                v0y = Cy;
                v1x = Ax;
                v1y = Ay;
            }
        } else {
            if (W == (Elem_T)0) {
                edgeHit = true;
                v0x = Ax;
                v0y = Ay;
                v1x = Bx;
                v1y = By;
            } else {
                // Hit inside
                return true;
            }
        }
    }

    // XXX: For testing only bary0 = edgeHit; bary1 = vertHit;

    // Handle vertex watertightness with rule that if +X is inside triangle, vertex is in
    if (vertHit) {
        if (isFrontFacing) {
            if (v0y == v1y && v0x < v1x) return true; // Edge is horizontal and goes to the right so it's on +X.
            if (v0y < v1y && v0y > v2y) return true;  // +X axis is inside triangle.
        } else {
            if (v0y == v2y && v0x < v2x) return true; // Edge is horizontal and goes to the right so it's on +X.
            if (v0y < v2y && v0y > v1y) return true;  // +X axis is inside triangle.
        }
    }

    // Handle edge watertightness with top-left rule
    if (edgeHit) {
        if (isFrontFacing) {
            if (v0y == v1y && v0x < v1x) return true; // Edge is horizontal and goes to the right so it's a top edge.
            if (v0y < v1y) return true;               // Edge goes up so it's a left edge.
        } else {
            if (v0y == v1y && v0x > v1x) return true; // Edge is horizontal and goes to the left so it's a top edge.
            if (v0y > v1y) return true;               // Edge goes down so it's a left edge.
        }
    }

    return false;
}
template DMC_DECL bool RTI(const f3vec& orig, const f3vec& dir, float tMin, float tMax, const tTriangle<f3vec>& tri, bool cullFront, bool cullBack,
                           bool frontIsCCW, float& tHit, float& bary0, float& bary1, bool& isFrontFacing);
