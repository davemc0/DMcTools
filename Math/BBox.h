//////////////////////////////////////////////////////////////////////
// BBox.h - Implements an axis-aligned bounding box.
//
// Copyright David K. McAllister, Aug. 1998.

#ifndef _bbox_h
#define _bbox_h

#include <Math/Vector.h>

class BBox
{
	bool valid;
    bool sphere_valid;
    Vector Ctr;
    double Rad;
	
    inline void validate_sphere()
    {
        if(sphere_valid)
            return;

        Vector Span = (MaxV - MinV) * 0.5;

        Ctr = MinV + Span;
        Rad = Span.length();
        sphere_valid = true;
    }

public:
	Vector MinV, MaxV;
	
	inline BBox() {valid = false;}
	inline BBox(const Vector &_min, const Vector &_max)
		: MinV(_min), MaxV(_max), valid(true), sphere_valid(false)
	{}

	inline void Reset() {valid = false;}
	inline double MaxDim() const
	{
		return Max(MaxV.x - MinV.x, MaxV.y - MinV.y, MaxV.z - MinV.z);
	}
	
	inline Vector Center()
	{
        validate_sphere();
		return Ctr;
	}
	
	inline double Radius()
	{
        validate_sphere();
		return Rad;
	}
	
	// Returns true if the point is in the box.
	inline bool Inside(const Vector &P) const
	{
		return !((P.x < MinV.x || P.y < MinV.y || P.z < MinV.z ||
			P.x > MaxV.x || P.y > MaxV.y || P.z > MaxV.z));
	}
	
	// Returns true if any of the sphere-box is in the box.
	// XXX For speed we will return true if the P+-r box intersects the bbox.
	inline bool SphereIntersect(const Vector &P, const double r) const
	{
		ASSERT(r >= 0.0);
		return (!(P.x+r < MinV.x || P.y+r < MinV.y || P.z+r < MinV.z ||
			P.x-r > MaxV.x || P.y-r > MaxV.y || P.z-r > MaxV.z));
	}
	
	inline BBox& operator+=(const Vector &v)
	{
		if(valid)
		{
			MinV.x = Min(MinV.x, v.x);
			MinV.y = Min(MinV.y, v.y);
			MinV.z = Min(MinV.z, v.z);
			
			MaxV.x = Max(MaxV.x, v.x);
			MaxV.y = Max(MaxV.y, v.y);
			MaxV.z = Max(MaxV.z, v.z);
		}
		else
		{
			valid = true;
			MinV = v;
			MaxV = v;
		}
		
        sphere_valid = false;
		
		return *this;
	}
	
	inline BBox& operator+=(const BBox &b)
	{
		if(valid)
		{
			MinV.x = Min(MinV.x, b.MinV.x);
			MinV.y = Min(MinV.y, b.MinV.y);
			MinV.z = Min(MinV.z, b.MinV.z);
			
			MaxV.x = Max(MaxV.x, b.MaxV.x);
			MaxV.y = Max(MaxV.y, b.MaxV.y);
			MaxV.z = Max(MaxV.z, b.MaxV.z);
		}
		else
		{
			valid = true;
			MinV = b.MinV;
			MaxV = b.MaxV;
		}
		
        sphere_valid = false;

		return *this;
	}

	inline bool is_valid() const {return valid;}
};

inline ostream& operator<<(ostream& os, const BBox& b)
{
	os << b.MinV.print() << b.MaxV.print();
	return os;
}

inline istream& operator>>(istream& is, BBox& b)
{
	is >> b.MinV >> b.MaxV;
	return is;
}

#endif
