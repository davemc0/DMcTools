//////////////////////////////////////////////////////////////////////
// KDIVector.h - A 3D Vector that can be stored in a KDItemTree
//
// Copyright David K. McAllister, Jan. 2000.

// This class extends the f3Vector class with the functions needed to store it in a K-D Tree.
// You generally make a KDItemTree<KDVec> and then add these things to it.

// This class only works with KDItemTree.

#pragma once

#include "Math/KDItemTree.h"
#include "Math/Vector.h"

struct KDIVector : public f3Vector {
    // Might add things here like normal and coords in spherical map.

    DMC_DECL KDIVector() {}

    DMC_DECL KDIVector(const f3Vector& Vert) : f3Vector(Vert) {}
};
