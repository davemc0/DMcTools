//////////////////////////////////////////////////////////////////////
// KDIVector.h - A 3D Vector that can be stored in a KDItemTree.
//
// Copyright David K. McAllister, Jan. 2000.

// This class extends the Vector class with the functions needed to store it in a K-D Tree.
// You generally make a KDItemTree<KDVec> and then add these things to it.

// This class only works with KDItemTree.

#ifndef dmc_kd_ivector_h
#define dmc_kd_ivector_h

#include "Math/Vector.h"
#include "Math/KDItemTree.h"

struct KDIVector : public f3Vector
{
    // Might add things here like normal and coords in spherical map.

    DMC_INLINE KDIVector() {}

    DMC_INLINE KDIVector(const f3Vector &Vert) : f3Vector(Vert) {}
};

#endif
