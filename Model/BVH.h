// Base class for BVH builders
//
// Copyright 2022 by David K. McAllister

#pragma once

#include "Math/Triangle.h"

#include <vector>

class BVHNode {
    Aabb l, r;
    int lc, rc; // Child indices
    bool isLeaf;
};

struct PrimRef {
    Aabb box;
    Aabb leftBox;     // Scratch space to store left inclusive scan AABB for some range
    int primId;       // Index into user's triangle array; primId < 0 means reference is invalid
    uint32_t linCode; // Morton code of primitive reference
};

class BVH {
public:
    const std::vector<Triangle>& Tris; // The user's triangle array (may have degenerates, can't change order, etc.)
    std::vector<BVHNode> Nodes;
    std::vector<PrimRef> Refs;

    BVH(const std::vector<Triangle>& tris_) : Tris(tris_) {}
    virtual void build() = 0;

protected:
    void prepass(); // Get root AABB, remove degenerates, etc.
};
