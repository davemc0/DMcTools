//////////////////////////////////////////////////////////////////////
// KDVertex.h - A mesh vertex pointer that can be stored in a KDBoxTree.
//
// Copyright David K. McAllister, Aug. 1999.

#ifndef _kd_vertex_h
#define _kd_vertex_h

#include <Math/KDBoxTree.h>
#include <Model/MeshElements.h>

struct KDVertex
{
	Vertex *Vert;
	
	inline KDVertex() {Vert = NULL;}
	
	inline KDVertex(Vertex *Ver) : Vert(Ver) {}
	
	friend inline bool lessX(const KDVertex &a, const KDVertex &b)
	{
#ifdef TMP_DEBUG
		of << &a << " " << a.Vert->V << " lessX " << &b << " " << b.Vert->V << endl;
	of << a.Vert << " " << b.Vert << endl;
#endif
		return a.Vert->V.x < b.Vert->V.x;
	}
	
	friend inline bool lessY(const KDVertex &a, const KDVertex &b)
	{
#ifdef TMP_DEBUG
		of << a.Vert->V << " lessY " << b.Vert->V << endl;
#endif
		return a.Vert->V.y < b.Vert->V.y;
	}
	
	friend inline bool lessZ(const KDVertex &a, const KDVertex &b)
	{
#ifdef TMP_DEBUG
		of << a.Vert->V << " lessZ " << b.Vert->V << endl;
#endif
		return a.Vert->V.z < b.Vert->V.z;
	}
	
	// These three are for breaking ties in the KDTree.
	friend inline bool lessFX(const KDVertex &a, const KDVertex &b)
	{
		if(a.Vert->V.x < b.Vert->V.x) return true;
		else if(a.Vert->V.x > b.Vert->V.x) return false;
		else if(a.Vert->V.y < b.Vert->V.y) return true;
		else if(a.Vert->V.y > b.Vert->V.y) return false;
		else return a.Vert->V.z < b.Vert->V.z;
	}
	
	friend inline bool lessFY(const KDVertex &a, const KDVertex &b)
	{
		if(a.Vert->V.y < b.Vert->V.y) return true;
		else if(a.Vert->V.y > b.Vert->V.y) return false;
		else if(a.Vert->V.z < b.Vert->V.z) return true;
		else if(a.Vert->V.z > b.Vert->V.z) return false;
		else return a.Vert->V.x < b.Vert->V.x;
	}
	
	friend inline bool lessFZ(const KDVertex &a, const KDVertex &b)
	{
		if(a.Vert->V.z < b.Vert->V.z) return true;
		else if(a.Vert->V.z > b.Vert->V.z) return false;
		else if(a.Vert->V.x < b.Vert->V.x) return true;
		else if(a.Vert->V.x > b.Vert->V.x) return false;
		else return a.Vert->V.y < b.Vert->V.y;
	}

	inline bool operator==(const KDVertex &a) const
	{
		return Vert->V == a.Vert->V;
	}

	inline KDVertex& operator=(const KDVertex &a)
	{
#ifdef TMP_DEBUG
		of << this << " assign " << &a << endl;
		of << Vert << " asVVgn " << a.Vert << endl;
#endif
		Vert = a.Vert;
		return *this;
	}
	
	inline Vector& vector() const
	{
		return Vert->V;
	}
};

#endif
