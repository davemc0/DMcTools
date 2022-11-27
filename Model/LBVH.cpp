// Linearized BVH builder
//
// Copyright 2022 by David K. McAllister

#include "Model/LBVH.h"

#include "Image/tImage.h"
#include "Math/Random.h"
#include "Math/SpaceFillCurve.h"

#include <algorithm>
#include <execution>

#define EXPOL std::execution::par_unseq
// #define EXPOL std::execution::seq

void LBVH::build()
{
    // Make and Filter Prim Refs
    makeRefs();

    // Order Prim Refs
    linearize();
    // randomize();
    // projectRefs();

    // Build Hierarchy
    Nodes.reserve(Refs.size() * 2); // TODO: Make smaller
    balancedTreeFromRefs(0, Refs.size());

    // Finalize Boxes
    auto [box, cost] = refitNodes(0);
    cost /= box.area();

    // std::cerr << box << box.area() << " SAH = " << cost << " Nodes.size = " << Nodes.size() << '\n';

    Refs.clear(); // Refs is only used while building. Only Nodes and Tris remain afterward.
}

void LBVH::makeRefs()
{
    // Make array of references to input triangles and return world AABB
    Refs.resize(Tris.size());

    m_worldBox = std::transform_reduce(
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

    // std::cerr << m_worldBox << " Refs.size = " << Refs.size() << '\n';
}

void LBVH::linearize()
{
    FixedPointifier<f3vec> Fixifier(m_worldBox, curveOrder<uint32_t>());

    // Make centroids and get Morton codes of centroids
    std::transform(EXPOL, Refs.begin(), Refs.end(), Refs.begin(), [&](auto ref) {
        i3vec icrds = Fixifier.floatToFixed(ref.box.centroid());
        ref.linCode = toMortonCode<uint32_t>(icrds);
        return ref;
    });

    // Sort refs by centroid code
    std::sort(EXPOL, Refs.begin(), Refs.end(), [&](auto a, auto b) { return a.linCode < b.linCode; });
}

void LBVH::randomize()
{
    // Make centroids and get Morton codes of centroids
    std::transform(EXPOL, Refs.begin(), Refs.end(), Refs.begin(), [&](auto ref) {
        ref.linCode = irand();
        return ref;
    });

    // Sort refs by centroid code
    std::sort(EXPOL, Refs.begin(), Refs.end(), [&](auto a, auto b) { return a.linCode < b.linCode; });
}

// Compute Morton code of min and max corners of AABB and scatter plot them
void LBVH::projectRefs()
{
    FixedPointifier<f3vec> Fixifier(m_worldBox, curveOrder<uint32_t>());

    uc1Image Scatter(1024, 1024, uc1Pixel(0));

    std::transform(EXPOL, Refs.begin(), Refs.end(), Refs.begin(), [&](auto ref) {
        i3vec locrds = Fixifier.floatToFixed(Tris[ref.primId].v[0]);
        i3vec hicrds = Fixifier.floatToFixed(Tris[ref.primId].v[1]);

        // uint32_t locode = ref.linCode = toMortonCode<uint32_t>(locrds);
        // uint32_t hicode = toMortonCode<uint32_t>(hicrds);
        // uint32_t locode = ref.linCode = toHilbertCode<uint32_t>(locrds);
        // uint32_t hicode = toHilbertCode<uint32_t>(hicrds);
        // uint32_t locode = ref.linCode = toRasterCode<uint32_t>(locrds);
        // uint32_t hicode = toRasterCode<uint32_t>(hicrds);
        uint32_t locode = ref.linCode = toBoustroCode<uint32_t>(locrds);
        uint32_t hicode = toBoustroCode<uint32_t>(hicrds);
        // uint32_t locode = ref.linCode = toTiled2Code<uint32_t>(locrds);
        // uint32_t hicode = toTiled2Code<uint32_t>(hicrds);

        int x = locode >> 20;
        int y = hicode >> 20;
        Scatter(x, y) = uc1Pixel(255);
        return ref;
    });

    Scatter.Save("BunnyTriBoustro.png");
}

// Turn this span of refs into Nodes
// Does not compute AABBs or SAH, just topology
// Returns root index of subtree made for span
// end is one past last
size_t LBVH::balancedTreeFromRefs(size_t begin, size_t end)
{
    ASSERT_R(Nodes.capacity() > 0); // Recommend preallocating

    size_t count = end - begin;
    size_t perChild = count / getMaxChildren();
    size_t remainder = count - perChild * getMaxChildren();

    size_t newNodeInd = Nodes.size();
    BVHNode& newNode = Nodes.emplace_back();

    if (count == 1) {
        // Make a leaf node
        newNode.isLeaf = true;
        newNode.primId = Refs[begin].primId;
    } else {
        // Make an internal node
        newNode.isLeaf = false;
        newNode.childCnt = perChild == 0 ? static_cast<int>(remainder) : getMaxChildren();
        ASSERT_D(newNode.childCnt > 1 && newNode.childCnt <= getMaxChildren());

        size_t curRef = begin;
        for (auto i = 0; i < newNode.childCnt; i++) {
            size_t toDo = perChild + ((i < remainder) ? 1 : 0);

            newNode.childInds[i] = static_cast<int>(balancedTreeFromRefs(curRef, curRef + toDo));
            curRef += toDo;
        }
    }

    return newNodeInd;
}

std::tuple<Aabb, float> LBVH::refitNodes(size_t ind, int level, bool refitAABBs)
{
    ASSERT_D(ind <= Nodes.size());

    Aabb nodeBox;
    float nodeCost = 0.f;

    if (Nodes[ind].isLeaf) {
        nodeBox = Tris[Nodes[ind].primId].bbox();
        // TODO: Handle triangle splitting
        nodeCost = nodeBox.area() * getBuildParams().leafCost;
        // std::cerr << "LEAF: " << ind << nodeBox << nodeBox.area() << " nodeCost = " << nodeCost << " " << Nodes[ind].primId << '\n';
    } else {
        for (int i = 0; i < Nodes[ind].childCnt; i++) {
            auto [box, cost] = refitNodes(Nodes[ind].childInds[i], level + 1, refitAABBs);
            ASSERT_D(box.valid());
            nodeCost += cost;
            nodeBox.grow(box);
            ASSERT_D(nodeBox.valid());
            if (refitAABBs) Nodes[ind].childAabbs[i] = box;
            // std::cerr << "INNR Child: " << i << " " << nodeBox << " area = " << nodeBox.area() << " nodeCost = " << nodeCost << " " << Nodes[ind].childCnt << '\n';
        }

        nodeCost += nodeBox.area() * getBuildParams().internalNodeCost; // Cost of the node itself
        // std::cerr << "INNR: " << ind << " " << nodeBox << " area = " << nodeBox.area() << " nodeCost = " << nodeCost << " " << Nodes[ind].childCnt << '\n';
    }

    return std::make_tuple(nodeBox, nodeCost);
}

// Compute SAH at each ref
// int LBVH::computeObjectSplitCosts(size_t begin, size_t end)
// {
//     // Grow partial AABB from left
//     std::inclusive_scan(EXPOL, Refs.begin() + begin, Refs.begin() + end, Refs.begin(), [&](const Aabb& boxa, const Aabb& boxb) { return unioncsg(boxa, boxb); });
//
//
// }
// Partition span of PrimRefs
// Spatial median
// Object median
// Primitive split
