// Linearized BVH builder
//
// Copyright 2022 by David K. McAllister

#pragma once

#include "Model/BVH.h"

class LBVH : public BVH {
public:
    LBVH(const std::vector<Triangle>& tris_) : BVH::BVH(tris_) {}

    void build();

private:
    void makeRefs();
    void linearize();

    Aabb worldBox;
};
