// Split BVH builder
//
// Copyright 2022 by David K. McAllister

#pragma once

#include "Model/BVH.h"

class SBVH : public BVH {
public:
    SBVH(const std::vector<Triangle>& tris_) : BVH::BVH(tris_) {}

    void build();

private:
    void makeRefs();
};
