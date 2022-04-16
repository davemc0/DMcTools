// Split BVH builder
//
// Copyright 2022 by David K. McAllister

#include "Model/SBVH.h"

#include <algorithm>
#include <execution>

#define EXPOL std::execution::par_unseq

void SBVH::makeRefs()
{
    // Make array of references to input triangles
    std::for_each(EXPOL, Tris.begin(), Tris.end(), [&](const Triangle& tri) {
        PrimRef ref;
        int ind = &tri - &(*Tris.begin());
        bool degen = tri.isDegenerate();
        ref.primId = degen ? -1 : ind;
        ref.box = tri.bbox();

        Refs.emplace_back(ref);
    });

    // Move invalid references to end of list, where they can easily be killed
    std::partition(EXPOL, Refs.begin(), Refs.end(), [&](auto a) { return a.primId >= 0; });
}
