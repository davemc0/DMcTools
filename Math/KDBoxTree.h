//////////////////////////////////////////////////////////////////////
// KDBoxTree.h - Implements a template class for a K-D Tree using nested bounding boxes
//
// Copyright David K. McAllister, Sep. 1999.

#pragma once

#include "Math/AABB.h"
#include "Util/Assert.h"

#include <algorithm>
#include <vector>

// The class Item_T must support the following functions:
// friend DMC_DECL bool lessX(const Item_T &a, const Item_T &b);
// friend DMC_DECL bool lessY(const Item_T &a, const Item_T &b);
// friend DMC_DECL bool lessZ(const Item_T &a, const Item_T &b);
// DMC_DECL bool operator==(const Item_T &a) const;
// DMC_DECL f3vec &std::vector() const;
// These can point to the normal ones if no ties can occur.
// friend DMC_DECL bool lessFX(const Item_T &a, const Item_T &b);
// friend DMC_DECL bool lessFY(const Item_T &a, const Item_T &b);
// friend DMC_DECL bool lessFZ(const Item_T &a, const Item_T &b);

#ifndef DMC_KD_MAX_BOX_SIZE
#define DMC_KD_MAX_BOX_SIZE 128
#endif

template <typename Item_T> class KDBoxTree {
    Aabb Box;
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
        for (typename std::vector<Item_T>::iterator I = Items.begin(); I != Items.end(); I++) Box.grow(I->vec3());

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
        Box.reset();
    }

    void insert(const Item_T& It)
    {
        // std::cerr << "Box In\n";
        Box.grow(It.vec3());

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
    bool find(const Item_T& It, Item_T& Res, const f3vec::ElType& D)
    {
        if (FNE(It, Res, D * D)) return true;

        if (D > 0)
            return FNEB(It, Res, D);
        else
            return false;
    }

    // Finds the closest item to the query point.
    // Returns the distance to it (not squared).
    f3vec::ElType nearest(const Item_T& It, Item_T& Res) const
    {
        f3vec::ElType dist = FC(It, Res);

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
                const f3vec& V = Items[i].vec3();
                fprintf(stderr, "%d %0.20lf %0.20lf %0.20lf\n", i, V.x, V.y, V.z);
                // std::cerr << i << " " << Items[i].vec3() << std::endl;
            }
        }
    }

    const Aabb& GetBBox() { return Box; }

private:
    // While inserting, the box got too full and must be split
    void SplitBox()
    {
        // Find which dimension.
        f3vec d = Box.hi() - Box.lo();

        if (d.x > d.y)
            myless = (d.x > d.z) ? Item_T::lessX : Item_T::lessZ;
        else
            myless = (d.y > d.z) ? Item_T::lessY : Item_T::lessZ;

        for (typename std::vector<Item_T>::const_iterator Ti = Items.begin(); Ti != Items.end(); Ti++) {
            f3vec Vf = Ti->vec3();
            if (std::isnan(Vf.x) || std::isnan(Vf.y) || std::isnan(Vf.z)) std::cerr << "NAN " << Vf << std::endl;
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
    bool FNE(const Item_T& It, Item_T& Res, const f3vec::ElType& DSqr = 0)
    {
        if (left) {
            // Find into the kids.
            if (myless(It, Med))
                return left->FNE(It, Res, DSqr);
            else
                return right->FNE(It, Res, DSqr);
        }

        for (typename std::vector<Item_T>::iterator I = Items.begin(); I != Items.end(); I++)
            if (isNear(I->vec3(), It.vec3(), DSqr)) {
                Res = *I;
                return true;
            }

        return false;
    }

    // Find a close enough item in any box.
    // Returns true if there is one, false if not.
    // Uses box - bounding box test.
    bool FNEB(const Item_T& It, Item_T& Res, const f3vec::ElType& D = 0)
    {
        if (left) {
            if (left->Box.intersectsSphere(It.vec3(), D))
                if (left->FNEB(It, Res, D)) return true;

            if (right->Box.intersectsSphere(It.vec3(), D))
                if (right->FNEB(It, Res, D)) return true;
            return false;
        }

        for (typename std::vector<Item_T>::iterator I = Items.begin(); I != Items.end(); I++)
            if (isNear(I->vec3(), It.vec3(), D * D)) {
                Res = *I;
                return true;
            }

        return false;
    }

    // Finds the closest item in the same box as the query point.
    // Returns it in Res and returns distance from It.
    f3vec::ElType FC(const Item_T& It, Item_T& Res) const
    {
        if (left) {
            // Find into the kids.
            if (myless(It, Med))
                return left->FC(It, Res);
            else
                return right->FC(It, Res);
        }

        if (Items.size() == 0) return DMC_MAXFLOAT;

        f3vec::ElType BestLenSqr = DMC_MAXFLOAT;
        for (typename std::vector<Item_T>::const_iterator I = Items.begin(); I != Items.end(); I++) {
            f3vec::ElType lensqr;
            if ((lensqr = (I->vec3() - It.vec3()).lenSqr()) < BestLenSqr) {
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
    f3vec::ElType FCB(const Item_T& It, Item_T& Res, const f3vec::ElType& D) const
    {
        if (left) {
            f3vec::ElType tb = D; // Value to beat.
            if (left->Box.intersectsSphere(It.vec3(), tb)) tb = left->FCB(It, Res, tb);

            if (right->Box.intersectsSphere(It.vec3(), tb)) tb = right->FCB(It, Res, tb);

            return tb;
        }

        f3vec::ElType BestLenSqr = D * D;
        for (typename std::vector<Item_T>::const_iterator I = Items.begin(); I != Items.end(); I++) {
            f3vec::ElType lensqr;
            if ((lensqr = (I->vec3() - It.vec3()).lenSqr()) < BestLenSqr) {
                Res = *I;
                BestLenSqr = lensqr;
            }
        }

        // Avoid the sqrt for precision.
        if (BestLenSqr == D * D) return D;
        return sqrt(BestLenSqr);
    }
};
