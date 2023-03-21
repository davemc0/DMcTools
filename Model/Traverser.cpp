#include "Model/Traverser.h"

#include "Math/Intersect.h"
#include "Util/Counters.h"

#include <algorithm>

template <class Vec_T, class Elem_T> auto Traverser::rayNodeIntersect(const Vec_T& orig, const Vec_T& dir, Elem_T tMin, Elem_T tMax, const BVHNode& node) {}

template <class Vec_T, class Elem_T>
std::tuple<bool, float, float, float, bool, int> Traverser::traceRay(const Vec_T& orig, const Vec_T& dir, Elem_T tMin, Elem_T tMax)
{
    typedef std::tuple<float, int> StackEntry;
    std::vector<StackEntry> TravStack;

    bool isHit = false, isFrontFacing = false;
    float tHit = DMC_MAXFLOAT, bary0 = -1.f, bary1 = -1.f;
    int primIdHit = -1;

    TravStack.push_back({DMC_MAXFLOAT, 0}); // Push the root

    do {
        StackEntry stkTop = TravStack.back();
        TravStack.pop_back();

        const BVHNode& node = m_bvh.Nodes[std::get<1>(stkTop)];

        // CINC("TRAV_STEP");

        if (node.isLeaf) {
            const Triangle& tri = m_bvh.Tris[node.primId];

            float tHitCand = -1.f, bary0Cand = -1.f, bary1Cand = -1.f;
            bool isFrontFacingCand = false;
            bool isHitCand = RTI(orig, dir, tMin, tMax, tri,                         // Ray & Tri
                                 false, false, false,                                // Flags
                                 tHitCand, bary0Cand, bary1Cand, isFrontFacingCand); // Results

            if (isHitCand && tHitCand < tHit) { // Commit hit?
                tHit = tHitCand;
                bary0 = bary0Cand;
                bary1 = bary1Cand;
                isFrontFacing = isFrontFacingCand;
                isHit = isHitCand;
                primIdHit = node.primId;
            }
        } else {
            for (int i = 0; i < node.childCnt; i++) {
                float tHitBox = -1.f;
                bool isHitBox = RBI(orig, dir, tMin, tMax, node.childAabbs[i], // Ray & Box
                                    tHitBox);                                  // Results
                if (isHitBox) {
                    StackEntry& stkNew = TravStack.emplace_back();
                    stkNew = {tHitBox, node.childInds[i]};
                }
            }
        }
    } while (TravStack.size());

    return {isHit, tHit, bary0, bary1, isFrontFacing, primIdHit};
}
template std::tuple<bool, float, float, float, bool, int> Traverser::traceRay(const f3vec& orig, const f3vec& dir, float tMin, float tMax);
