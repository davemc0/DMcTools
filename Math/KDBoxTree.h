//////////////////////////////////////////////////////////////////////
// KDBoxTree.h - Implements a template class for a K-D Tree
// using nested bounding boxes.
//
// Copyright David K. McAllister, Sep. 1999.

#ifndef _kdboxtree_h
#define _kdboxtree_h

#include <Math/BBox.h>

#include <vector>
#include <algorithm>

// The class _Tp must support the following functions:
// friend inline bool lessX(const _Tp &a, const _Tp &b);
// friend inline bool lessY(const _Tp &a, const _Tp &b);
// friend inline bool lessZ(const _Tp &a, const _Tp &b);
// inline bool operator==(const _Tp &a) const;
// inline Vector &std::vector() const;
// These can point to the normal ones if no ties cannot occur.
// friend inline bool lessFX(const _Tp &a, const _Tp &b);
// friend inline bool lessFY(const _Tp &a, const _Tp &b);
// friend inline bool lessFZ(const _Tp &a, const _Tp &b);

#ifndef DMC_KD_MAX_BOX_SIZE
#define DMC_KD_MAX_BOX_SIZE 128
#endif

template<class _Tp>
class KDBoxTree
{
    BBox Box;
    std::vector<_Tp> Items;
    _Tp Med;
    KDBoxTree *left, *right;
    bool (*myless)(const _Tp &a, const _Tp &b);

public:
    inline KDBoxTree() {left = right = NULL; myless = NULL;}

    inline KDBoxTree(const _Tp *first, const _Tp *last)
    {
        left = right = NULL;
        myless = NULL;
        Items.assign(first, last);
        for(typename std::vector<_Tp>::iterator I = Items.begin(); I!=Items.end(); I++)
            Box += I->vector();

        // std::cerr << "Constructed KDBoxTree with " << Items.size() << " items.\n";
    }

    // A copy constructor.
    inline KDBoxTree(const KDBoxTree &Tr) : Box(Tr.Box), Items(Tr.Items), Med(Tr.Med)
    {
        // std::cerr << "Copying KDBoxTree\n";
        myless = Tr.myless;
        if(Tr.left)
        {
            left = new KDBoxTree(*Tr.left);
            right = new KDBoxTree(*Tr.right);
            ASSERT_RM(left && right, "memory alloc failed");
        }
        else
            left = right = NULL;
    }

    inline ~KDBoxTree()
    {
        if(left)
            delete left;
        if(right)
            delete right;
    }

    // Remove everything in the tree.
    inline void clear()
    {
        if(left)
            delete left;
        if(right)
            delete right;

        Items.clear();
        left = right = NULL;
        myless = NULL;
        Box.Reset();
    }

    inline void insert(const _Tp &It)
    {
        //std::cerr << "Box In\n";
        Box += It.vector();

        // std::cerr << Items.size() << Box << " " << It.Vert->V << std::endl;

        if(left)
        {
            // Insert into the kids.
            if(myless(It, Med))
                left->insert(It);
            else
                right->insert(It);
            return;
        }

        Items.push_back(It);

        // Do I need to split the box?
        if(Items.size() > DMC_KD_MAX_BOX_SIZE)
        {
            // Find which dimension.
            Vector d = Box.MaxV - Box.MinV;

            if(d.x > d.y) myless = (d.x > d.z) ? _Tp::lessX : _Tp::lessZ;
            else myless = (d.y > d.z) ? _Tp::lessY : _Tp::lessZ;

            for(typename std::vector<_Tp>::const_iterator Ti = Items.begin(); Ti != Items.end(); Ti++)
            {
                Vector Vf = Ti->vector();
                if(IsNaN(Vf.x) || IsNaN(Vf.y) || IsNaN(Vf.z))
                    std::cerr << "NAN " << Vf << std::endl;
            }

            // Split the box into two kids and find median.
            sort(Items.begin(), Items.end(), myless);

            _Tp *MedP = &Items[Items.size()/2];

            // Handle yucky case of median being duplicate.
            if(!myless(*(MedP-1), *MedP))
            {
                // Shift gears to more complex comparator.
                if(d.x > d.y) myless = d.x > d.z ? _Tp::lessFX : _Tp::lessFZ;
                else myless = d.y > d.z ? _Tp::lessFY : _Tp::lessFZ;

                sort(Items.begin(), Items.end(), myless);
            }
            ASSERT_D(myless(*(MedP-1), *MedP));
            Med = *MedP;

            // Puts the median in the right child.
            left = new KDBoxTree(&(*Items.begin()), MedP);
            right = new KDBoxTree(MedP, &(*Items.end()));
            ASSERT_RM(left && right, "memory alloc failed");
            Items.clear();
        }
    }

    // Find an exact match.
    // This function probably should be const but it's hard because we would
    // have to make Res const (since it's part of the Tree) and we can't make
    // Res const because it's returned by modifying it. Maybe switch to pointers?
    inline bool find(const _Tp &It, _Tp &Res)
    {
        // fprintf(stderr, "F: 0x%08x ", long(this));
        // std::cerr << Items.size() << Box << " " << It.Vert->V << std::endl;

        if(left)
        {
            // Find into the kids.
            if(myless(It, Med))
                return left->find(It, Res);
            else
                return right->find(It, Res);
        }

        for(_Tp *I =&(*Items.begin()); I!=&(*Items.end()); I++)
        {
            if(*I == It)
            {
                Res = *I;
                return true;
            }
        }

        return false;
    }

    // Finds a close enough item to the query point.
    // Returns true if there is one, false if not.
    inline bool find(const _Tp &It, _Tp &Res, const double &D)
    {
        if(FNE(It, Res, Sqr(D)))
            return true;

        if(D > 0)
            return FNEB(It, Res, D);
        else
            return false;
    }

    // Finds the closest item to the query point.
    // Returns the distance to it (not squared).
    inline double nearest(const _Tp &It, _Tp &Res) const
    {
        double dist = FC(It, Res);

        return FCB(It, Res, dist);
    }

    void Dump()
    {
        std::cerr << Box << std::endl;

        if(left)
        {
            left->Dump();
            right->Dump();
        }
        else
        {
            std::cerr << "Count = " << Items.size() << std::endl;
            for(int i=0; i<Items.size(); i++)
            {
                const Vector &V = Items[i].vector();
                fprintf(stderr, "%d %0.20lf %0.20lf %0.20lf\n", i, V.x, V.y, V.z);
                // std::cerr << i << " " << Items[i].vector() << std::endl;
            }
        }
    }

    const BBox &GetBBox()
    {
        return Box;
    }

private:
    // Finds a close enough item in the same box as the query point.
    // Returns true if there is one, false if not.
    inline bool FNE(const _Tp &It, _Tp &Res, const double &DSqr = 0)
    {
        if(left)
        {
            // Find into the kids.
            if(myless(It, Med))
                return left->FNE(It, Res, DSqr);
            else
                return right->FNE(It, Res, DSqr);
        }

        for(_Tp *I=Items.begin(); I!=Items.end(); I++)
            if(VecEq(I->vector(), It.vector(), DSqr))
            {
                Res = *I;
                return true;
            }

        return false;
    }

    // Find a close enough item in any box.
    // Returns true if there is one, false if not.
    // Uses box - bounding box test.
    inline bool FNEB(const _Tp &It, _Tp &Res, const double &D = 0)
    {
        if(left)
        {
            if(left->Box.SphereIntersect(It.vector(), D))
                if(left->FNEB(It, Res, D))
                    return true;

            if(right->Box.SphereIntersect(It.vector(), D))
                if(right->FNEB(It, Res, D))
                    return true;
            return false;
        }

        double DSqr = Sqr(D);
        for(_Tp *I=Items.begin(); I!=Items.end(); I++)
            if(VecEq(I->vector(), It.vector(), DSqr))
            {
                Res = *I;
                return true;
            }

        return false;
    }

    // Finds the closest item in the same box as the query point.
    // Returns it in Res and returns distance from It.
    inline double FC(const _Tp &It, _Tp &Res) const
    {
        if(left)
        {
            // Find into the kids.
            if(myless(It, Med))
                return left->FC(It, Res);
            else
                return right->FC(It, Res);
        }

        if(Items.size() == 0)
            return DMC_MAXFLOAT;

        double BestLenSqr = DMC_MAXFLOAT;
        for(const _Tp *I=Items.begin(); I!=Items.end(); I++)
        {
            double lensqr;
            if((lensqr = (I->vector() - It.vector()).length2()) < BestLenSqr)
            {
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
    inline double FCB(const _Tp &It, _Tp &Res, const double &D) const
    {
        if(left)
        {
            double tb = D; // Value to beat.
            if(left->Box.SphereIntersect(It.vector(), tb))
                tb = left->FCB(It, Res, tb);

            if(right->Box.SphereIntersect(It.vector(), tb))
                tb = right->FCB(It, Res, tb);

            return tb;
        }

        double BestLenSqr = D*D;
        for(const _Tp *I=Items.begin(); I!=Items.end(); I++)
        {
            double lensqr;
            if((lensqr = (I->vector() - It.vector()).length2()) < BestLenSqr)
            {
                Res = *I;
                BestLenSqr = lensqr;
            }
        }

        // Avoid the sqrt for precision.
        if(BestLenSqr == D*D) return D;
        return sqrt(BestLenSqr);
    }
};

#endif
