//////////////////////////////////////////////////////////////////////
// KDItemTree.h - Implements a template class for a KD-Tree
//
// Copyright David K. McAllister, Sep. 1999.

#pragma once

#include "Math/Vector.h"

#include <algorithm>

template<class Item_T>
class KDItem
{
public:
    Item_T Value;
    KDItem *lower, *higher;

    DMC_DECL KDItem() {lower = higher = NULL;}

    DMC_DECL KDItem(const Item_T &Value) : Value(Value) {lower = higher = NULL;}

    // A copy constructor.
    DMC_DECL KDItem(const KDItem &Tr) : Value(Tr.Value)
    {
        // cerr << "Copying KDItem\n";
        lower = higher = NULL;
        if(Tr.lower)
        {
            lower = new KDItem(*Tr.lower);
            ASSERT_RM(lower, "memory alloc failed");
        }
        if(Tr.higher)
        {
            higher = new KDItem(*Tr.higher);
            ASSERT_RM(higher, "memory alloc failed");
        }
    }

    DMC_DECL ~KDItem()
    {
        if(lower)
            delete lower;
        if(higher)
            delete higher;
    }
};

// The class Item_T needs to inherit from f3Vector.
// Or you can edit this to require .vector().

// Always stores the lesser item in the lower child.
template<class Item_T, int NumDim = 3>
class KDItemTree
{
    KDItem<Item_T> *Root;

    DMC_DECL bool lessX(const f3Vector &a, const f3Vector &b) const
    {
        //cerr << "lessX\n";
        if(a.x < b.x) return true;
        else if(a.x > b.x) return false;
        else if(a.y < b.y) return true;
        else if(a.y > b.y) return false;
        else return a.z < b.z;
    }

    DMC_DECL bool lessY(const f3Vector &a, const f3Vector &b) const
    {
        if(a.y < b.y) return true;
        else if(a.y > b.y) return false;
        else if(a.z < b.z) return true;
        else if(a.z > b.z) return false;
        else return a.x < b.x;
    }

    DMC_DECL bool lessZ(const f3Vector &a, const f3Vector &b) const
    {
        if(a.z < b.z) return true;
        else if(a.z > b.z) return false;
        else if(a.x < b.x) return true;
        else if(a.x > b.x) return false;
        else return a.y < b.y;
    }

    DMC_DECL bool myless(const f3Vector &a, const f3Vector &b, const int lev) const
    {
        if(lev==2) return lessZ(a, b);
        else if(lev) return lessY(a, b);
        else return lessX(a, b);
    }

    // Returns true if the two are close enough.
    DMC_DECL bool within(const f3Vector &a, const f3Vector &b, const f3Vector::ElType threshSq) const
    {
        return (a-b).length2() < threshSq;
    }

public:
    DMC_DECL KDItemTree()
    {
        Root = NULL;
    }

    // A copy constructor.
    DMC_DECL KDItemTree(const KDItemTree &Tr)
    {
        if(Tr.Root)
        {
            Root = new KDItem<Item_T>(*Tr.Root);
            ASSERT_RM(Root, "memory alloc failed");
        }
        else
            Root = NULL;
    }

    DMC_DECL ~KDItemTree()
    {
        if(Root)
            delete Root;
    }

    // Remove everything in the tree.
    DMC_DECL void clear()
    {
        if(Root)
            delete Root;
        Root = NULL;
    }

    DMC_DECL bool empty() const
    {
        return Root == NULL;
    }

    // Insert an item into the tree.
    DMC_DECL void insert(const Item_T &Val)
    {
        //cerr << "In\n";
        KDItem<Item_T> *It = new KDItem<Item_T>(Val);
        ASSERT_RM(It, "memory alloc failed");

        // fprintf(stderr, "I: 0x%08x ", long(this));
        // cerr << Value.size() << Box << " " << It.Vert->V << endl;

        if(!Root)
        {
            Root = It;
        }
        else
        {
            rinsert(Root, It);
        }

        return;
    }

    // Find an exact match. Returns NULL if not there.
    // Queries given a Item_T.
    DMC_DECL const Item_T * find(const Item_T &Qr) const
    {
        // fprintf(stderr, "F: 0x%08x ", long(this));
        // cerr << Value.size() << Box << " " << Qr.Vert->V << endl;

        if(Root)
            return rfind(Root, Qr.Value);
        else
            return Root->Value; // It's NULL.
    }

    // Find an exact match. Returns NULL if not there.
    // Queries on a f3Vector instead of on a Item_T.
    DMC_DECL const Item_T * findv(const f3Vector &Qr) const
    {
        if(Root)
            return rfind(Root, Qr);
        else
            return NULL;
    }

    // Find a close enough one. Returns NULL if not there.
    // Queries on a f3Vector instead of on a Item_T.
    DMC_DECL const Item_T * findv(const f3Vector &Qr, f3Vector::ElType thresh) const
    {
        if(Root)
            return rfindeps(Root, Qr, thresh);
        else
            return NULL;
    }

    // Find the closest one. Returns NULL if tree is empty.
    // Queries on a f3Vector instead of on a Item_T.
    DMC_DECL const Item_T * nearestv(const f3Vector &Qr) const
    {
        const Item_T *bestItem = NULL;
        f3Vector::ElType bestdSqr = DMC_MAXFLOAT;
        rnearest(Root, Qr, bestdSqr, bestItem);
        return bestItem;
    }

private:
    // This one is called recursively internally.
    DMC_DECL void rinsert(KDItem<Item_T> *Root, KDItem<Item_T> *It, int lev = 0)
    {
        if(lev==3)
            lev = 0;
        //cerr << "insert\n";

        // insert into the kids.
        if(myless(It->Value, Root->Value, lev))
        {
            if(Root->lower)
                rinsert(Root->lower, It, lev+1);
            else
                Root->lower = It;
        }
        else
        {
            if(Root->higher)
                rinsert(Root->higher, It, lev+1);
            else
                Root->higher = It;
        }

        return;
    }

    // This one is called recursively internally.
    // The strategy is to traverse the whole tree except for branches
    // that can be pruned because the distance to that partition from
    // the query point is farther away than the currently closest point.
    // Uses a global best point.
    DMC_DECL void rnearest(KDItem<Item_T> *Root, const f3Vector &Qr,
        f3Vector::ElType &bestdSqr, const Item_T *&bestItem, int lev = 0) const
    {
        if(Root == NULL)
            return;
        if(lev==3)
            lev = 0;

        // See if Root is closer and update best.
        f3Vector::ElType rdSqr = (Root->Value - Qr).length2();

        if(rdSqr < bestdSqr)
        {
            bestdSqr = rdSqr;
            bestItem = &(Root->Value);
        }

        // Compute distance to the dividing line.
        f3Vector::ElType distL =
            (lev == 0) ? (Root->Value.x - Qr.x) :
        (lev == 1) ? (Root->Value.y - Qr.y) :
        (Root->Value.z - Qr.z);
        f3Vector::ElType distLSqr = dmcm::Sqr(distL);

        // Qr is on one side of the plane that contains Root->Value.
        // We have to traverse this side.
        // We also have to traverse the other side if the plane is within
        // bestdSqr of Qr.
        if(distL > 0)
        {
            // Query point is on lower of root.
            // We have to traverse the lower side.
            rnearest(Root->lower, Qr, bestdSqr, bestItem, lev+1);

            // Do we also have to traverse higher side?
            if(distLSqr < bestdSqr)
                rnearest(Root->higher, Qr, bestdSqr, bestItem, lev+1);
        }
        else
        {
            // Query point is on higher of root.
            // We have to traverse the higher side.
            rnearest(Root->higher, Qr, bestdSqr, bestItem, lev+1);

            // Do we also have to traverse lower side?
            if(distLSqr < bestdSqr)
                rnearest(Root->lower, Qr, bestdSqr, bestItem, lev+1);
        }
    }

    // This one is called recursively internally.
    DMC_DECL const Item_T * rfind(const KDItem<Item_T> *Root, const f3Vector &Qr,
        int lev = 0) const
    {
        if(lev==3)
            lev = 0;

        if(Root->lower && myless(Qr, Root->Value, lev))
        {
            return rfind(Root->lower, Qr, lev+1);
        }
        else
        {
            if(Qr == Root->Value)
                return &Root->Value;
            else if(Root->higher)
                return rfind(Root->higher, Qr, lev+1);
            else
                return NULL;
        }
    }

    // This one returns any point that is within a distance epsilon.
    // This one is called recursively internally.
    DMC_DECL const Item_T * rfindeps(const KDItem<Item_T> *Root, const f3Vector &Qr,
        f3Vector::ElType thresh, int lev = 0) const
    {
        if(lev==3)
            lev=0;

        if(within(Qr, Root->Value, dmcm::Sqr(thresh)))
            return &(Root->Value);

        if(myless(Qr, Root->Value, lev))
        {
            if(Root->lower)
            {
                if(const Item_T *Res = rfindeps(Root->lower, Qr, thresh, lev+1))
                    return Res;
            }
            // Didn't find in the proper half.
            // Do we need to search the other half?
            if(Root->higher == NULL)
                return NULL;

            if(((lev==0) && fabs(Root->Value.x - Qr.x) < thresh) ||
                ((lev==1) && fabs(Root->Value.y - Qr.y) < thresh) ||
                ((lev==2) && fabs(Root->Value.z - Qr.z) < thresh))
                return rfindeps(Root->higher, Qr, thresh, lev+1);
            else
                return NULL;
        }
        else
        {
            if(Root->higher)
            {
                if(const Item_T *Res = rfindeps(Root->higher, Qr, thresh, lev+1))
                    return Res;
            }

            // Didn't find in the proper half.
            // Do we need to search the other half?
            if(Root->lower == NULL)
                return NULL;

            if(((lev==0) && fabs(Root->Value.x - Qr.x) < thresh) ||
                ((lev==1) && fabs(Root->Value.y - Qr.y) < thresh) ||
                ((lev==2) && fabs(Root->Value.z - Qr.z) < thresh))
                return rfindeps(Root->lower, Qr, thresh, lev+1);
            else
                return NULL;
        }
    }
};
