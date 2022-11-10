// Linearized BVH builder
//
// Copyright 2022 by David K. McAllister

#pragma once

#include "Model/BVH.h"

class LBVH : public BVH {
public:
    LBVH(const std::vector<Triangle>& tris_, BuildParams& buildParams) : BVH::BVH(tris_, buildParams) {}

    void build();

private:
    void makeRefs();
    void linearize();
    void randomize();
    void projectRefs();

    void computeObjectSplitCosts(size_t begin, size_t end);

    // Functions that fill in the Nodes array
    size_t balancedTreeFromRefs(size_t begin, size_t end);

    // Traverse all nodes, applying provided function
    // void postTraverseNodes(take a lambda visit function)

    // Traverse all nodes, recomputing AABBs and SAH and returning rolled-up child AABB and SAH
    // Set refitAABBs to false to just compute AABBs and not modify Nodes
    std::tuple<Aabb, float> refitNodes(size_t ind, int level = 0, bool refitAABBs = true);
};
