//////////////////////////////////////////////////////////////////////
// KDTreeTest.cpp - Test both implementations of the K-D Tree.
//
// Copyright David K. McAllister, July 2000.

#include "Util/Utils.h"
#include "Util/Timer.h"
#include "Math/Vector.h"

#include "Math/KDIVector.h"
#include "Math/KDItemTree.h"

#include "Math/KDBVector.h"
#include "Math/KDBoxTree.h"

#include <vector>
#include <iostream>
using namespace std;

namespace {

#define LISTCOUNT 60000
#define LOOPCOUNT 100000
#define VLOOPCOUNT 1000
#define CLOSE_THRESH 75.0

    Timer Clock;

    // Find the closest point to this one.
    // Return the euclidean distance thereto.
    double FindClosestInList(const vector<Vector> &List, const Vector &Q, Vector &Res)
    {
        double DSqr = DMC_MAXFLOAT;
        for(size_t i=0; i<List.size(); i++)
        {
            double d;
            if((d = (List[i] - Q).length2()) <= DSqr)
            {
                DSqr = d;
                Res = List[i];
            }
        }

        return sqrt(DSqr);
    }

    // Find the closest point to this one.
    // Return the euclidean distance thereto.
    bool FindSpecificInList(const vector<Vector> &List, const Vector &Q)
    {
        for(size_t i=0; i<List.size(); i++)
            if(List[i] == Q)
                return true;

        return false;
    }

    // Find the closest point to this one.
    // Return the euclidean distance thereto.
    double FindCloseEnoughInList(const vector<Vector> &List, const Vector &Q, Vector &Res, const double Thresh)
    {
        double DSqr = Thresh * Thresh;
        bool found = false;
        for(size_t i=0; i<List.size(); i++) {
            double d;
            if((d = (List[i] - Q).length2()) < DSqr)
            {
                DSqr = d;
                Res = List[i];
                found = true;
            }
        }

        if(found)
            return sqrt(DSqr);
        else
            return -1;
    }


    // Generate random points and put them in the List.
    void FillList(vector<Vector> &List)
    {
        cerr << "Generating List...\n";
        Clock.Reset();
        Clock.Start();
        int i;
        for(i=0; i<LISTCOUNT; i++)
        {
            Vector P(DRand(-1000, 1000), DRand(-1000, 1000), DRand(-1000, 1000));
            List.push_back(P);
        }
        cerr << "Finished. " << i << " Time: " << Clock.Read() << endl;
    }

    // Insert each specific item from the list
    void Insert(const vector<Vector> &List, KDItemTree<Vector> &Tree)
    {
        cerr << "Inserting into KDItemTree...\n";
        Clock.Reset();
        Clock.Start();
        for(size_t i=0; i<List.size(); i++) {
            Tree.insert(List[i]);
        }
        cerr << "Finished. " << List.size() << " Time: " << Clock.Read() << endl;
    }

    // Insert each specific item from the list
    void Insert(const vector<Vector> &List, KDBoxTree<KDBVector> &Tree)
    {
        cerr << "Inserting into KDBoxTree...\n";
        Clock.Reset();
        Clock.Start();
        for(size_t i=0; i<List.size(); i++) {
            const Vector Q = List[i];
            Tree.insert(KDBVector(Q));
        }
        cerr << "Finished. " << List.size() << " Time: " << Clock.Read() << endl;
    }

    // Find each specific item from the list
    void FindSpecific_Speed(const vector<Vector> &List, const KDItemTree<Vector> &Tree)
    {
        cerr << "Speed: Finding each list item in KDItemTree...\n";
        Clock.Reset();
        Clock.Start();
        for(size_t i=0; i<List.size(); i++) {
            // cerr << ".";
            const Vector &Q = List[i];
            const Vector *Found = Tree.findv(Q);
        }
        cerr << "Finished. " << List.size() << " Time: " << Clock.Read() << endl;
    }

    // Find each specific item from the list
    void FindSpecific_Speed(const vector<Vector> &List, KDBoxTree<KDBVector> &Tree)
    {
        cerr << "Speed: Finding each list item in KDBoxTree...\n";
        Clock.Reset();
        Clock.Start();
        for(size_t i=0; i<List.size(); i++) {
            // cerr << ".";
            const Vector &Q = List[i];
            KDBVector Found;
            bool FoundIt = Tree.find(Q, Found);
        }
        cerr << "Finished. " << List.size() << " Time: " << Clock.Read() << endl;
    }

    // Find each specific item from the list
    bool FindSpecific_Verify(const vector<Vector> &List, const KDItemTree<Vector> &Tree)
    {
        cerr << "Verify: Finding each list item in KDItemTree...\n";
        bool fail = false;
        Clock.Reset();
        Clock.Start();
        for(size_t i=0; i<List.size(); i++) {
            //cerr << ".";
            const Vector &Q = List[i];
            const Vector *Found = Tree.findv(Q);
            if(Found == NULL || *Found != Q)
            {
                cerr << "Exact item not found: " << Q << endl;
                fail = true;
            }
        }
        cerr << "Finished. " << List.size() << " Time: " << Clock.Read() << endl;
        return fail;
    }

    // Find each specific item from the list
    bool FindSpecific_Verify(const vector<Vector> &List, KDBoxTree<KDBVector> &Tree)
    {
        cerr << "Verify: Finding each list item in KDBoxTree...\n";
        bool fail = false;
        Clock.Reset();
        Clock.Start();
        for(size_t i=0; i<List.size(); i++) {
            //cerr << ".";
            const Vector &Q = List[i];
            KDBVector Found;
            bool FoundIt = Tree.find(Q, Found);
            if(!FoundIt || Found != Q)
            {
                cerr << "Exact item not found: " << Q << endl;
                fail = true;
            }
        }
        cerr << "Finished. " << List.size() << " Time: " << Clock.Read() << endl;
        return fail;
    }

    // Pick a bunch of random points and find the closest point in the tree.
    // For verification, find the closest in the list and make sure they're the same.
    void FindClosest_Speed(const vector<Vector> &List, const KDItemTree<Vector> &Tree)
    {
        cerr << "Speed: Finding closest point to each of " << LOOPCOUNT << " random points in KDItemTree...\n";
        Clock.Reset();
        Clock.Start();
        for(size_t i=0; i<LOOPCOUNT; i++) {
            // cerr << ".";
            Vector Q(DRand(-2000, 2000), DRand(-2000, 2000), DRand(-2000, 2000));
            const Vector *ResultT = Tree.nearestv(Q);
        }
        cerr << "Finished. " << LOOPCOUNT << " Time: " << Clock.Read() << endl;
    }

    // Pick a bunch of random points and find the closest point in the tree.
    // For verification, find the closest in the list and make sure they're the same.
    void FindClosest_Speed(const vector<Vector> &List, const KDBoxTree<KDBVector> &Tree)
    {
        cerr << "Speed: Finding closest point to each of " << LOOPCOUNT << " random points in KDBoxTree...\n";
        Clock.Reset();
        Clock.Start();
        for(size_t i=0; i<LOOPCOUNT; i++) {
            //cerr << ".";
            Vector Q(DRand(-2000, 2000), DRand(-2000, 2000), DRand(-2000, 2000));
            const KDBVector QB(Q);
            KDBVector ResultT;
            double treedist = Tree.nearest(QB, ResultT);
        }
        cerr << "Finished. " << LOOPCOUNT << " Time: " << Clock.Read() << endl;
    }

    // Pick a bunch of random points and find the closest point in the tree.
    // For verification, find the closest in the list and make sure they're the same.
    bool FindClosest_Verify(const vector<Vector> &List, const KDItemTree<Vector> &Tree)
    {
        cerr << "Verify: Finding closest point to each of " << VLOOPCOUNT << " random points in KDItemTree...\n";
        bool fail = false;
        Clock.Reset();
        Clock.Start();
        for(size_t i=0; i<VLOOPCOUNT; i++) {
            cerr << ".";
            Vector Q(DRand(-2000, 2000), DRand(-2000, 2000), DRand(-2000, 2000));
            const Vector *ResultT = Tree.nearestv(Q);
            Vector ResultL;
            double listdist = FindClosestInList(List, Q, ResultL);

            if(*ResultT != ResultL)
            {
                cerr << "Wrong Closest: " << Q << ": " << (*ResultT) << " != " << ResultL << endl;
                cerr << "Distances: " << ((*ResultT)-Q).length() << " " << listdist << endl;
                fail = true;
            }
        }
        cerr << "\nFinished. " << VLOOPCOUNT << " Time: " << Clock.Read() << endl;
        return fail;
    }

    // Pick a bunch of random points and find the closest point in the tree.
    // For verification, find the closest in the list and make sure they're the same.
    bool FindClosest_Verify(const vector<Vector> &List, const KDBoxTree<KDBVector> &Tree)
    {
        cerr << "Verify: Finding closest point to each of " << VLOOPCOUNT << " random points in KDBoxTree...\n";
        bool fail = false;
        Clock.Reset();
        Clock.Start();
        for(size_t i=0; i<VLOOPCOUNT; i++) {
            cerr << ".";
            Vector Q(DRand(-2000, 2000), DRand(-2000, 2000), DRand(-2000, 2000));
            KDBVector ResultT;
            double treedist = Tree.nearest(Q, ResultT);
            Vector ResultL;
            double listdist = FindClosestInList(List, Q, ResultL);

            if(ResultT != ResultL)
            {
                cerr << "Wrong Closest: " << Q << ": " << ResultT << " != " << ResultL << endl;
                cerr << "Distances: " << (ResultT-Q).length() << " or " << treedist << " " << listdist << endl;
                fail = true;
            }
        }
        cerr << "\nFinished. " << VLOOPCOUNT << " Time: " << Clock.Read() << endl;
        return fail;
    }

    // Pick a bunch of random points and find the closest point in the tree.
    // For verification, find the closest in the list and make sure they're the same.
    void FindCloseEnough_Speed(const vector<Vector> &List, const KDItemTree<Vector> &Tree)
    {
        cerr << "Speed: Finding close enough for each of " << LOOPCOUNT << " random points in KDItemTree...\n";
        Clock.Reset();
        Clock.Start();
        for(size_t i=0; i<LOOPCOUNT; i++) {
            Vector Q(DRand(-2000, 2000), DRand(-2000, 2000), DRand(-2000, 2000));
            const Vector *ResultT = Tree.findv(Q, CLOSE_THRESH);
        }
        cerr << "Finished. " << LOOPCOUNT << " Time: " << Clock.Read() << endl;
    }

    // Pick a bunch of random points and find the closest point in the tree.
    // For verification, find the closest in the list and make sure they're the same.
    void FindCloseEnough_Speed(const vector<Vector> &List, KDBoxTree<KDBVector> &Tree)
    {
        cerr << "Speed: Finding close enough for each of " << LOOPCOUNT << " random points in KDBoxTree...\n";
        Clock.Reset();
        Clock.Start();
        int i;
        for(i=0; i<LOOPCOUNT; i++) {
            Vector Q(DRand(-2000, 2000), DRand(-2000, 2000), DRand(-2000, 2000));
            KDBVector ResultT;
            Tree.find(Q, ResultT, CLOSE_THRESH);
        }
        cerr << "Finished. " << LOOPCOUNT << " Time: " << Clock.Read() << endl;
    }

    // Pick a bunch of random points and find the closest point in the tree.
    // For verification, find the closest in the list and make sure they're the same.
    bool FindCloseEnough_Verify(const vector<Vector> &List, const KDItemTree<Vector> &Tree)
    {
        cerr << "Verify: Finding close enough for each of " << VLOOPCOUNT << " random points in KDItemTree...\n";
        bool fail = false;
        Clock.Reset();
        Clock.Start();
        int i;
        for(i=0; i<VLOOPCOUNT; i++) {
            Vector Q(DRand(-2000, 2000), DRand(-2000, 2000), DRand(-2000, 2000));
            const Vector *ResultT = Tree.findv(Q, CLOSE_THRESH);

            if(ResultT) {
                bool IsInList = FindSpecificInList(List, *ResultT);

                if(IsInList) {
                    double d = ((*ResultT)-Q).length();

                    if(d > CLOSE_THRESH) {
                        cerr << "Found a point that was too far: Q=" << Q << (*ResultT) << " " << d << endl;
                        fail = true;
                    } else {
                        cerr << ".";
                    }
                } else {
                    cerr << "Returned a bogus point: " << (*ResultT) << " for query " << Q << endl;
                    fail = true;
                }
            } else {
                Vector ResultL;
                double d = FindCloseEnoughInList(List, Q, ResultL, CLOSE_THRESH);

                if(d >= 0) {
                    cerr << "It couldn't find a close enough point, but there was one:\n"
                        << Q << " ResultL = " << ResultL << d << endl;
                    fail = true;
                } else {
                    cerr << "x";
                }
            }
        }
        cerr << "\nFinished. " << i << " Time: " << Clock.Read() << endl;
        return fail;
    }

    // Pick a bunch of random points and find the closest point in the tree.
    // For verification, find the closest in the list and make sure they're the same.
    bool FindCloseEnough_Verify(const vector<Vector> &List, KDBoxTree<KDBVector> &Tree)
    {
        cerr << "Verify: Finding close enough for each of " << VLOOPCOUNT << " random points in KDBoxTree...\n";
        bool fail = false;
        Clock.Reset();
        Clock.Start();
        for(int i=0; i<VLOOPCOUNT; i++) {
            Vector Q(DRand(-2000, 2000), DRand(-2000, 2000), DRand(-2000, 2000));
            KDBVector ResultT;
            bool Found = Tree.find(Q, ResultT, CLOSE_THRESH);

            if(Found) {
                bool IsInList = FindSpecificInList(List, ResultT);

                if(IsInList) {
                    double d = (ResultT-Q).length();

                    if(d > CLOSE_THRESH) {
                        cerr << "Found a point that was too far: Q=" << Q << ResultT << " " << d << endl;
                        fail = true;
                    } else {
                        cerr << ".";
                    }
                } else {
                    cerr << "Returned a bogus point: " << ResultT << " for query " << Q << endl;
                    fail = true;
                }
            } else {
                Vector ResultL;
                double d = FindCloseEnoughInList(List, Q, ResultL, CLOSE_THRESH);

                if(d >= 0) {
                    cerr << "It couldn't find a close enough point, but there was one:\n"
                        << Q << " ResultL = " << ResultL << d << endl;
                    fail = true;
                } else {
                    cerr << "x";
                }
            }
        }
        cerr << "\nFinished. " << VLOOPCOUNT << " Time: " << Clock.Read() << endl;
        return fail;
    }
};

// Return true on success
bool KDTreeTest(int argc, char **argv)
{
    KDItemTree<Vector> ITree;
    KDBoxTree<KDBVector> BTree;
    vector<Vector> List;

    FillList(List);

    Insert(List, ITree);
    Insert(List, BTree);

    FindCloseEnough_Speed(List, ITree);
    FindCloseEnough_Speed(List, BTree);

    FindSpecific_Speed(List, ITree);
    FindSpecific_Speed(List, BTree);

    FindClosest_Speed(List, ITree);
    FindClosest_Speed(List, BTree);

    bool fail = false;

    fail = fail || FindCloseEnough_Verify(List, ITree);
    fail = fail || FindCloseEnough_Verify(List, BTree);

    fail = fail || FindSpecific_Verify(List, ITree);
    fail = fail || FindSpecific_Verify(List, BTree);

    fail = fail || FindClosest_Verify(List, ITree);
    fail = fail || FindClosest_Verify(List, BTree);

    return !fail;
}
