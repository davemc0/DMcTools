//////////////////////////////////////////////////////////////////////
// KDBVector.h - A 3D Vector that can be stored in a KDBoxTree.
//
// Copyright David K. McAllister, Jan. 2000.

// This class extends the Vector class with the functions needed to store it in a K-D Tree.
// You generally make a KDBoxTree<KDBVector> and then add these things to it.

// This class only works with KDBoxTree.

#ifndef dmc_kd_bvector_h
#define dmc_kd_bvector_h

#include "Math/Vector.h"

class KDBVector : public Vector
{
public:
    // Might add things here like normal and coords in spherical map.

    KDBVector() {}

    KDBVector(const Vector &V) : Vector(V) {}

    static bool lessX(const KDBVector &a, const KDBVector &b)
    {
        return a.x < b.x;
    }

    static bool lessY(const KDBVector &a, const KDBVector &b)
    {
        return a.y < b.y;
    }

    static bool lessZ(const KDBVector &a, const KDBVector &b)
    {
        return a.z < b.z;
    }

    // These three are for breaking ties in the KDTree.
    static bool lessFX(const KDBVector &a, const KDBVector &b)
    {
        if(a.x < b.x) return true;
        else if(a.x > b.x) return false;
        else if(a.y < b.y) return true;
        else if(a.y > b.y) return false;
        else return a.z < b.z;
    }

    static bool lessFY(const KDBVector &a, const KDBVector &b)
    {
        if(a.y < b.y) return true;
        else if(a.y > b.y) return false;
        else if(a.z < b.z) return true;
        else if(a.z > b.z) return false;
        else return a.x < b.x;
    }

    static bool lessFZ(const KDBVector &a, const KDBVector &b)
    {
        if(a.z < b.z) return true;
        else if(a.z > b.z) return false;
        else if(a.x < b.x) return true;
        else if(a.x > b.x) return false;
        else return a.y < b.y;
    }

    const Vector &vector() const
    {
      // return Vert;
      return *this;
    }
};

#endif
