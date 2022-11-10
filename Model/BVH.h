// Base class for BVH builders
//
// Copyright 2022 by David K. McAllister

#pragma once

#include "Math/Triangle.h"

#include <vector>

const int CHILD_ALLOC = 2;

// Stores either an internal or a leaf node
// Leaf nodes contain one prim.
struct BVHNode {
    Aabb childAabbs[CHILD_ALLOC]; // For internal nodes
    int childInds[CHILD_ALLOC];   // Child indices for internal nodes
    int childCnt;                 // For internal nodes
    int primId;                   // For leaf nodes
    bool isLeaf;
};

struct PrimRef {
    Aabb box;
    Aabb leftBox;     // Scratch space to store left inclusive scan AABB for some range
    int primId;       // Index into user's triangle array; primId < 0 means reference is invalid
    uint32_t linCode; // Morton code of primitive reference
};

struct BuildParams {
    float internalNodeCost = 1.f;
    float leafCost = 1.f;
    int maxChildren = CHILD_ALLOC;
    int maxPrimsPerLeaf = 1;
};

class BVH {
public:
    const std::vector<Triangle>& Tris; // The user's triangle array (may have degenerates, can't change order, etc.) Keep this around for tracing.
    std::vector<BVHNode> Nodes;
    std::vector<PrimRef> Refs; // Intention is to delete refs once BVH is built

    BVH(const std::vector<Triangle>& tris_, BuildParams& buildParams) : Tris(tris_), m_buildParams(buildParams) {}
    virtual void build() = 0;

    int getMaxChildren() { return m_buildParams.maxChildren; }
    auto getBuildParams() { return m_buildParams; }
    auto getWorldBox() { return m_worldBox; }

protected:
    void prepass(); // Get root AABB, remove degenerates, etc.
    Aabb m_worldBox;

private:
    BuildParams m_buildParams;
    int m_maxChildren; // Branching factor / arity of internal nodes (must be <= CHILD_ALLOC)
};
