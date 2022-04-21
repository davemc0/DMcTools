// Linearized BVH builder
//
// Copyright 2022 by David K. McAllister

#include "Model/LBVH.h"

#include "Math/SpaceFillCurve.h"

#include <algorithm>
#include <execution>

//#define EXPOL std::execution::par_unseq
#define EXPOL std::execution::seq

void LBVH::build()
{
    makeRefs();
    linearize();
}

void LBVH::makeRefs()
{
    // Make array of references to input triangles and return world AABB
    Refs.resize(Tris.size());

    worldBox = std::transform_reduce(
        EXPOL, Tris.begin(), Tris.end(), Aabb(), [&](const Aabb& boxa, const Aabb& boxb) { return unioncsg(boxa, boxb); },
        [&](const Triangle& tri) {
            PrimRef ref;
            int ind = &tri - &(*Tris.begin());
            bool degen = tri.isDegenerate();
            ref.primId = degen ? -1 : ind;
            ref.box = tri.bbox();
            Refs[ind] = ref;
            return degen ? Aabb() : ref.box;
        });

    // Move invalid references to end of list and kill them
    std::vector<PrimRef>::iterator first_bad = std::partition(EXPOL, Refs.begin(), Refs.end(), [&](const PrimRef& a) { return a.primId >= 0; });
    Refs.resize(first_bad - Refs.begin());

    std::cerr << worldBox << " Refs.size=" << Refs.size() << '\n';
}

void LBVH::linearize()
{
    FixedPointifier<f3vec> Fixifier(worldBox, curveOrder<uint32_t>());

    // Make centroid
    // Get Morton code of centroid
    // Fixifier.floatToFixed();
}
