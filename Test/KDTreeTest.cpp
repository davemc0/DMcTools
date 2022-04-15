//////////////////////////////////////////////////////////////////////
// KDTreeTest.cpp - Test K-D Tree
//
// Copyright David K. McAllister, July 2000.

#include "Math/KDBVector.h"
#include "Math/KDBoxTree.h"
#include "Math/Vector.h"
#include "Math/VectorUtils.h"
#include "Util/Timer.h"
#include "Util/Utils.h"

#include <iostream>
#include <vector>

namespace {

const int LISTCOUNT = 500000;
const int SPDLOOPCOUNT = 1000000;
const int VERIFLOOPCOUNT = 1000;
const float CLOSE_THRESH = 75.0;

// Find the closest point to this one.
// Return the euclidean distance thereto.
f3vec::ElType FindClosestInList(const std::vector<f3vec>& List, const f3vec& Q, f3vec& Res)
{
    f3vec::ElType DSqr = DMC_MAXFLOAT;
    for (size_t i = 0; i < List.size(); i++) {
        f3vec::ElType d;
        if ((d = (List[i] - Q).lenSqr()) <= DSqr) {
            DSqr = d;
            Res = List[i];
        }
    }

    return sqrt(DSqr);
}

// Find the closest point to this one.
// Return the euclidean distance thereto.
bool FindSpecificInList(const std::vector<f3vec>& List, const f3vec& Q)
{
    for (size_t i = 0; i < List.size(); i++)
        if (List[i] == Q) return true;

    return false;
}

// Find the closest point to this one.
// Return the euclidean distance thereto.
f3vec::ElType FindCloseEnoughInList(const std::vector<f3vec>& List, const f3vec& Q, f3vec& Res, const f3vec::ElType Thresh)
{
    f3vec::ElType DSqr = Thresh * Thresh;
    bool found = false;
    for (size_t i = 0; i < List.size(); i++) {
        f3vec::ElType d;
        if ((d = (List[i] - Q).lenSqr()) < DSqr) {
            DSqr = d;
            Res = List[i];
            found = true;
        }
    }

    if (found)
        return sqrt(DSqr);
    else
        return -1;
}

// Generate random points and put them in the List.
void FillList(std::vector<f3vec>& List)
{
    std::cerr << "Generating List...\n";
    Timer Clock;
    for (int i = 0; i < LISTCOUNT; i++) {
        f3vec P = makeRand<f3vec>(-1000, 1000);
        List.push_back(P);
    }
    std::cerr << "Finished. " << LISTCOUNT << " Time: " << Clock.Read() << std::endl;
}

// Insert each specific item from the list
void Insert(const std::vector<f3vec>& List, KDBoxTree<KDBVector>& Tree)
{
    std::cerr << "Inserting into KDBoxTree...\n";
    Timer Clock;
    for (size_t i = 0; i < List.size(); i++) {
        const f3vec Q = List[i];
        Tree.insert(KDBVector(Q));
    }
    std::cerr << "Finished. " << List.size() << " Time: " << Clock.Read() << std::endl;
}

// Find each specific item from the list
void FindSpecific_Speed(const std::vector<f3vec>& List, KDBoxTree<KDBVector>& Tree)
{
    std::cerr << "Speed: Finding each list item in KDBoxTree...\n";
    Timer Clock;
    for (size_t i = 0; i < List.size(); i++) {
        // std::cerr << ".";
        const f3vec& Q = List[i];
        KDBVector Found;
        bool FoundIt = Tree.find(Q, Found);
    }
    std::cerr << "Finished. " << List.size() << " Time: " << Clock.Read() << std::endl;
}

// Find each specific item from the list
bool FindSpecific_Verify(const std::vector<f3vec>& List, KDBoxTree<KDBVector>& Tree)
{
    std::cerr << "Verify: Finding each list item in KDBoxTree...\n";
    bool fail = false;
    Timer Clock;
    for (size_t i = 0; i < List.size(); i++) {
        // cerr << ".";
        const f3vec& Q = List[i];
        KDBVector Found;
        bool FoundIt = Tree.find(Q, Found);
        if (!FoundIt || Found != Q) {
            std::cerr << "Exact item not found: " << Q << std::endl;
            fail = true;
        }
    }
    std::cerr << "Finished. " << List.size() << " Time: " << Clock.Read() << std::endl;
    return fail;
}

// Pick a bunch of random points and find the closest point in the tree.
// For verification, find the closest in the list and make sure they're the same.
void FindClosest_Speed(const std::vector<f3vec>& List, const KDBoxTree<KDBVector>& Tree)
{
    std::cerr << "Speed: Finding closest point to each of " << SPDLOOPCOUNT << " random points in KDBoxTree...\n";
    Timer Clock;
    for (size_t i = 0; i < SPDLOOPCOUNT; i++) {
        // cerr << ".";
        f3vec Q = makeRand<f3vec>(-2000, 2000);
        const KDBVector QB(Q);
        KDBVector ResultT;
        f3vec::ElType treedist = Tree.nearest(QB, ResultT);
    }
    std::cerr << "Finished. " << SPDLOOPCOUNT << " Time: " << Clock.Read() << std::endl;
}

// Pick a bunch of random points and find the closest point in the tree.
// For verification, find the closest in the list and make sure they're the same.
bool FindClosest_Verify(const std::vector<f3vec>& List, const KDBoxTree<KDBVector>& Tree)
{
    std::cerr << "Verify: Finding closest point to each of " << VERIFLOOPCOUNT << " random points in KDBoxTree...\n";
    bool fail = false;
    Timer Clock;
    for (size_t i = 0; i < VERIFLOOPCOUNT; i++) {
        std::cerr << ".";
        f3vec Q = makeRand<f3vec>(-2000, 2000);
        KDBVector ResultT;
        f3vec::ElType treedist = Tree.nearest(Q, ResultT);
        f3vec ResultL;
        f3vec::ElType listdist = FindClosestInList(List, Q, ResultL);

        if (ResultT != ResultL) {
            std::cerr << "Wrong Closest: " << Q << ": " << ResultT << " != " << ResultL << std::endl;
            std::cerr << "Distances: " << (ResultT - Q).length() << " or " << treedist << " " << listdist << std::endl;
            fail = true;
        }
    }
    std::cerr << "\nFinished. " << VERIFLOOPCOUNT << " Time: " << Clock.Read() << std::endl;
    return fail;
}

// Pick a bunch of random points and find the closest point in the tree.
// For verification, find the closest in the list and make sure they're the same.
void FindCloseEnough_Speed(const std::vector<f3vec>& List, KDBoxTree<KDBVector>& Tree)
{
    std::cerr << "Speed: Finding close enough for each of " << SPDLOOPCOUNT << " random points in KDBoxTree...\n";
    Timer Clock;
    for (int i = 0; i < SPDLOOPCOUNT; i++) {
        f3vec Q = makeRand<f3vec>(-2000, 2000);
        KDBVector ResultT;
        Tree.find(Q, ResultT, CLOSE_THRESH);
    }
    std::cerr << "Finished. " << SPDLOOPCOUNT << " Time: " << Clock.Read() << std::endl;
}

// Pick a bunch of random points and find the closest point in the tree.
// For verification, find the closest in the list and make sure they're the same.
bool FindCloseEnough_Verify(const std::vector<f3vec>& List, KDBoxTree<KDBVector>& Tree)
{
    std::cerr << "Verify: Finding close enough for each of " << VERIFLOOPCOUNT << " random points in KDBoxTree...\n";
    bool fail = false;
    Timer Clock;
    for (int i = 0; i < VERIFLOOPCOUNT; i++) {
        f3vec Q = makeRand<f3vec>(-2000, 2000);
        KDBVector ResultT;
        bool Found = Tree.find(Q, ResultT, CLOSE_THRESH);

        if (Found) {
            bool IsInList = FindSpecificInList(List, ResultT);

            if (IsInList) {
                f3vec::ElType d = (ResultT - Q).length();

                if (d > CLOSE_THRESH) {
                    std::cerr << "Found a point that was too far: Q=" << Q << ResultT << " " << d << std::endl;
                    fail = true;
                } else {
                    std::cerr << ".";
                }
            } else {
                std::cerr << "Returned a bogus point: " << ResultT << " for query " << Q << std::endl;
                fail = true;
            }
        } else {
            f3vec ResultL;
            f3vec::ElType d = FindCloseEnoughInList(List, Q, ResultL, CLOSE_THRESH);

            if (d >= 0) {
                std::cerr << "It couldn't find a close enough point, but there was one:\n" << Q << " ResultL = " << ResultL << d << std::endl;
                fail = true;
            } else {
                std::cerr << "o";
            }
        }
    }
    std::cerr << "\nFinished. " << VERIFLOOPCOUNT << " Time: " << Clock.Read() << std::endl;
    return fail;
}

void TestSphereAABBIntersection()
{
    Timer Clock;
    for (int i = 0; i < LISTCOUNT; i++) {
        f3vec b0 = makeRand<f3vec>(-1000, 1000);
        f3vec b1 = makeRand<f3vec>(-1000, 1000);
        Aabb box;
        box.grow(b0);
        box.grow(b1);

        f3vec sphC = makeRand<f3vec>(-1000, 1000);
        float sphR = frand(0.f, 100.f);

        f3vec near = box.nearest(sphC);
        float dist = length(near - sphC);

        bool isect = box.intersectsSphere(sphC, sphR);

        // if (isect) std::cerr << i << box << sphC << dist << " " << isect << '\n';
        ASSERT_R((isect && sphR >= dist) || !isect);
    }
    std::cerr << "\nFinished. " << LISTCOUNT << " Time: " << Clock.Read() << std::endl;
}
}; // namespace

// Return true on success
bool KDTreeTest(int argc, char** argv)
{
    std::cerr << "Starting KDTreeTest\n";

    TestSphereAABBIntersection();

    KDBoxTree<KDBVector> BTree;
    std::vector<f3vec> List;

    FillList(List);

    Insert(List, BTree);

    FindCloseEnough_Speed(List, BTree);

    FindSpecific_Speed(List, BTree);

    FindClosest_Speed(List, BTree);
    std::cerr << "Done with speed tests.\n\n";

    bool fail = false;

    fail = fail || FindCloseEnough_Verify(List, BTree);

    fail = fail || FindSpecific_Verify(List, BTree);

    fail = fail || FindClosest_Verify(List, BTree);

    std::cerr << "Ending KDTreeTest\n";

    return !fail;
}
