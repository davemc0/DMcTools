//////////////////////////////////////////////////////////////////////
// KDBVector.h - A 3D Vector that can be stored in a KDBoxTree.
//
// Copyright David K. McAllister, Jan. 2000.

// This class extends the Vector class with the functions needed to store it in a K-D Tree.
// You generally make a KDBoxTree<KDBVector> and then add these things to it.

// This class only works with KDBoxTree.

#ifndef _kd_bvector_h
#define _kd_bvector_h

#include <Math/Vector.h>

class KDBVector : public Vector
{
public:
	// Might add things here like normal and coords in spherical map.
	
	inline KDBVector() {}
	
	inline KDBVector(const Vector &V) : Vector(V) {}
	
	friend inline bool lessX(const KDBVector &a, const KDBVector &b)
	{
		return a.x < b.x;
	}
	
	friend inline bool lessY(const KDBVector &a, const KDBVector &b)
	{
		return a.y < b.y;
	}
	
	friend inline bool lessZ(const KDBVector &a, const KDBVector &b)
	{
		return a.z < b.z;
	}
	
	// These three are for breaking ties in the KDTree.
	friend inline bool lessFX(const KDBVector &a, const KDBVector &b)
	{
		if(a.x < b.x) return true;
		else if(a.x > b.x) return false;
		else if(a.y < b.y) return true;
		else if(a.y > b.y) return false;
		else return a.z < b.z;
	}
	
	friend inline bool lessFY(const KDBVector &a, const KDBVector &b)
	{
		if(a.y < b.y) return true;
		else if(a.y > b.y) return false;
		else if(a.z < b.z) return true;
		else if(a.z > b.z) return false;
		else return a.x < b.x;
	}
	
	friend inline bool lessFZ(const KDBVector &a, const KDBVector &b)
	{
		if(a.z < b.z) return true;
		else if(a.z > b.z) return false;
		else if(a.x < b.x) return true;
		else if(a.x > b.x) return false;
		else return a.y < b.y;
	}
	
	inline const Vector &vector() const
	{
	  // return Vert;
	  return *this;
	}
};

#endif
