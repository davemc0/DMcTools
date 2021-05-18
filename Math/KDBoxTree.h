//////////////////////////////////////////////////////////////////////
// KDBoxTree.h - Implements a template class for a K-D Tree using nested bounding boxes
//
// Copyright David K. McAllister, Sep. 1999.

#pragma once

#include "Math/BBox.h"

#include <algorithm>
#include <vector>

// The class Item_T must support the following functions:
// friend DMC_INLINE bool lessX(const Item_T &a, const Item_T &b);
// friend DMC_INLINE bool lessY(const Item_T &a, const Item_T &b);
// friend DMC_INLINE bool lessZ(const Item_T &a, const Item_T &b);
// DMC_INLINE bool operator==(const Item_T &a) const;
// DMC_INLINE f3Vector &std::vector() const;
// These can point to the normal ones if no ties can occur.
// friend DMC_INLINE bool lessFX(const Item_T &a, const Item_T &b);
// friend DMC_INLINE bool lessFY(const Item_T &a, const Item_T &b);
// friend DMC_INLINE bool lessFZ(const Item_T &a, const Item_T &b);

#ifndef DMC_KD_MAX_BOX_SIZE
#define DMC_KD_MAX_BOX_SIZE 128
#endif

template <typename Item_T> class KDBoxTree {
    BBox<f3Vector> Box;
    std::vector<Item_T> Items;
    Item_T Med;
    KDBoxTree *left, *right;
    bool (*myless)(const Item_T& a, const Item_T& b);

public:
    KDBoxTree()
    {
        left = right = NULL;
        myless = NULL;
    }

    KDBoxTree(const typename std::vector<Item_T>::iterator first, const typename std::vector<Item_T>::iterator last)
    {
        left = right = NULL;
        myless = NULL;
        Items.assign(first, last);
        for (typename std::vector<Item_T>::iterator I = Items.begin(); I != Items.end(); I++) Box += I->vector();

        // std::cerr << "Constructed KDBoxTree with " << Items.size() << " items.\n";
    }

    // A copy constructor.
    KDBoxTree(const KDBoxTree& Tr) : Box(Tr.Box), Items(Tr.Items), Med(Tr.Med)
    {
        // std::cerr << "Copying KDBoxTree\n";
        myless = Tr.myless;
        if (Tr.left) {
            left = new KDBoxTree(*Tr.left);
            right = new KDBoxTree(*Tr.right);
            ASSERT_RM(left && right, "memory alloc failed");
        } else
            left = right = NULL;
    }

    ~KDBoxTree()
    {
        if (left) delete left;
        if (right) delete right;
    }

    // Remove everything in the tree.
    void clear()
    {
        if (left) delete left;
        if (right) delete right;

        Items.clear();
        left = right = NULL;
        myless = NULL;
        Box.Reset();
    }

    void insert(const Item_T& It)
    {
        // std::cerr << "Box In\n";
        Box += It.vector();

        // std::cerr << Items.size() << Box << " " << It.Vert->V << std::endl;

        if (left) {
            // Insert into the kids.
            if (myless(It, Med))
                left->insert(It);
            else
                right->insert(It);
            return;
        }

        Items.push_back(It);

        // Do I need to split the box?
        if (Items.size() > DMC_KD_MAX_BOX_SIZE) SplitBox();
    }

    // Find an exact match.
    // This function probably should be const but it's hard because we would
    // have to make Res const (since it's part of the Tree) and we can't make
    // Res const because it's returned by modifying it. Maybe switch to pointers?
    bool find(const Item_T& It, Item_T& Res)
    {
        // fprintf(stderr, "F: 0x%08x ", long(this));
        // std::cerr << Items.size() << Box << " " << It.Vert->V << std::endl;

        if (left) {
            // Find into the kids.
            if (myless(It, Med))
                return left->find(It, Res);
            else
                return right->find(It, Res);
        }

        for (auto I : Items) {
            if (I == It) {
                Res = I;
                return true;
            }
        }

        return false;
    }

    // Finds a close enough item to the query point.
    // Returns true if there is one, false if not.
    bool find(const Item_T& It, Item_T& Res, const f3Vector::ElType& D)
    {
        if (FNE(It, Res, dmcm::Sqr(D))) return true;

        if (D > 0)
            return FNEB(It, Res, D);
        else
            return false;
    }

    // Finds the closest item to the query point.
    // Returns the distance to it (not squared).
    f3Vector::ElType nearest(const Item_T& It, Item_T& Res) const
    {
        f3Vector::ElType dist = FC(It, Res);

        return FCB(It, Res, dist);
    }

    void Dump()
    {
        std::cerr << Box << std::endl;

        if (left) {
            left->Dump();
            right->Dump();
        } else {
            std::cerr << "Count = " << Items.size() << std::endl;
            for (int i = 0; i < Items.size(); i++) {
                const f3Vector& V = Items[i].vector();
                fprintf(stderr, "%d %0.20lf %0.20lf %0.20lf\n", i, V.x, V.y, V.z);
                // std::cerr << i << " " << Items[i].vector() << std::endl;
            }
        }
    }

    const BBox<f3Vector>& GetBBox() { return Box; }

private:
    // While inserting, the box got too full and must be split
    void SplitBox()
    {
        // Find which dimension.
        f3Vector d = Box.MaxV - Box.MinV;

        if (d.x > d.y)
            myless = (d.x > d.z) ? Item_T::lessX : Item_T::lessZ;
        else
            myless = (d.y > d.z) ? Item_T::lessY : Item_T::lessZ;

        for (typename std::vector<Item_T>::const_iterator Ti = Items.begin(); Ti != Items.end(); Ti++) {
            f3Vector Vf = Ti->vector();
            if (dmcm::isNaN(Vf.x) || dmcm::isNaN(Vf.y) || dmcm::isNaN(Vf.z)) std::cerr << "NAN " << Vf << std::endl;
        }

        // Split the box into two kids and find median.
        sort(Items.begin(), Items.end(), myless);

        typename std::vector<Item_T>::iterator MedP = Items.begin() + Items.size() / 2;

        // Handle yucky case of median being duplicate.
        if (!myless(*(MedP - 1), *MedP)) {
            // Shift gears to more complex comparator.
            if (d.x > d.y)
                myless = d.x > d.z ? Item_T::lessFX : Item_T::lessFZ;
            else
                myless = d.y > d.z ? Item_T::lessFY : Item_T::lessFZ;

            sort(Items.begin(), Items.end(), myless);
        }
        ASSERT_D(myless(*(MedP - 1), *MedP));
        Med = *MedP;

        // Puts the median in the right-hand child.
        left = new KDBoxTree(Items.begin(), MedP);
        right = new KDBoxTree(MedP, Items.end());
        ASSERT_RM(left && right, "memory alloc failed");
        Items.clear();
    }

    // Finds a close enough item in the same box as the query point.
    // Returns true if there is one, false if not.
    bool FNE(const Item_T& It, Item_T& Res, const f3Vector::ElType& DSqr = 0)
    {
        if (left) {
            // Find into the kids.
            if (myless(It, Med))
                return left->FNE(It, Res, DSqr);
            else
                return right->FNE(It, Res, DSqr);
        }

        for (typename std::vector<Item_T>::iterator I = Items.begin(); I != Items.end(); I++)
            if (VecEq(I->vector(), It.vector(), DSqr)) {
                Res = *I;
                return true;
            }

        return false;
    }

    // Find a close enough item in any box.
    // Returns true if there is one, false if not.
    // Uses box - bounding box test.
    bool FNEB(const Item_T& It, Item_T& Res, const f3Vector::ElType& D = 0)
    {
        if (left) {
            if (left->Box.SphereIntersect(It.vector(), D))
                if (left->FNEB(It, Res, D)) return true;

            if (right->Box.SphereIntersect(It.vector(), D))
                if (right->FNEB(It, Res, D)) return true;
            return false;
        }

        f3Vector::ElType DSqr = dmcm::Sqr(D);
        for (typename std::vector<Item_T>::iterator I = Items.begin(); I != Items.end(); I++)
            if (VecEq(I->vector(), It.vector(), DSqr)) {
                Res = *I;
                return true;
            }

        return false;
    }

    // Finds the closest item in the same box as the query point.
    // Returns it in Res and returns distance from It.
    f3Vector::ElType FC(const Item_T& It, Item_T& Res) const
    {
        if (left) {
            // Find into the kids.
            if (myless(It, Med))
                return left->FC(It, Res);
            else
                return right->FC(It, Res);
        }

        if (Items.size() == 0) return DMC_MAXFLOAT;

        f3Vector::ElType BestLenSqr = DMC_MAXFLOAT;
        for (typename std::vector<Item_T>::const_iterator I = Items.begin(); I != Items.end(); I++) {
            f3Vector::ElType lensqr;
            if ((lensqr = (I->vector() - It.vector()).length2()) < BestLenSqr) {
                Res = *I;
                BestLenSqr = lensqr;
            }
        }

        return sqrt(BestLenSqr);
    }

    // Find the closest point in any box that is within D of It.
    // Returns it in Res and returns distance from It.
    // If there are none then it returns D and doesn't touch Res.
    // Uses box - bounding box test.
    f3Vector::ElType FCB(const Item_T& It, Item_T& Res, const f3Vector::ElType& D) const
    {
        if (left) {
            f3Vector::ElType tb = D; // Value to beat.
            if (left->Box.SphereIntersect(It.vector(), tb)) tb = left->FCB(It, Res, tb);

            if (right->Box.SphereIntersect(It.vector(), tb)) tb = right->FCB(It, Res, tb);

            return tb;
        }

        f3Vector::ElType BestLenSqr = D * D;
        for (typename std::vector<Item_T>::const_iterator I = Items.begin(); I != Items.end(); I++) {
            f3Vector::ElType lensqr;
            if ((lensqr = (I->vector() - It.vector()).length2()) < BestLenSqr) {
                Res = *I;
                BestLenSqr = lensqr;
            }
        }

        // Avoid the sqrt for precision.
        if (BestLenSqr == D * D) return D;
        return sqrt(BestLenSqr);
    }
};
