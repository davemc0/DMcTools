// Linearized BVH builder
//
// Copyright 2022 by David K. McAllister

#include "Model/LBVH.h"

#include "Math/SpaceFillCurve.h"

#include <algorithm>
#include <execution>

#define EXPOL std::execution::par_unseq

void LBVH::build()
{
    makeRefs();
    linearize();
}

void LBVH::makeRefs()
{
    // Make array of references to input triangles
    std::for_each(EXPOL, Tris.begin(), Tris.end(), [&](const Triangle& tri) {
        PrimRef ref;
        int ind = &tri - &(*Tris.begin());
        bool degen = tri.isDegenerate();
        ref.primId = degen ? -1 : ind;
        ref.box = tri.bbox();

        if (!degen) worldBox.grow(ref.box); // XXX: Can I do this safely in parallel?
        Refs.emplace_back(ref);
    });

    // Move invalid references to end of list, where they can easily be killed
    std::partition(EXPOL, Refs.begin(), Refs.end(), [&](auto a) { return a.primId >= 0; });
}

void LBVH::linearize()
{
    FixedPointifier<f3vec> Fixifier(worldBox, curveOrder<uint32_t>());

    // Make centroid
    // Get Morton code of centroid
    // Fixifier.floatToFixed();
}
