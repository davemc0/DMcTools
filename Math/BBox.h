//////////////////////////////////////////////////////////////////////
// BBox.h - Implements an axis-aligned bounding box.
//
// Copyright David K. McAllister, Aug. 1998.

#ifndef dmc_bbox_h
#define dmc_bbox_h

#include "Math/Vector.h"

template<class Vector_T>
class BBox
{
    Vector_T Ctr;
    typename Vector_T::ElType Rad;
    bool valid;
    bool sphere_valid;

    DMC_INLINE void validate_sphere()
    {
        if(sphere_valid)
            return;

        Vector_T Span = (MaxV - MinV) * 0.5;

        Ctr = MinV + Span;
        Rad = Span.length();
        sphere_valid = true;
    }

public:
    Vector_T MinV, MaxV;

    DMC_INLINE BBox() {valid = false;}
    DMC_INLINE BBox(const Vector_T &MinV, const Vector_T &MaxV)
        :  valid(true), sphere_valid(false), MinV(MinV), MaxV(MaxV)
    {}

    DMC_INLINE void Reset() {valid = false;}
    DMC_INLINE typename Vector_T::ElType MaxDim() const
    {
        Vector_T D = MaxV - MinV;
        return Max(D.x, D.y, D.z);
    }

    DMC_INLINE Vector_T Center()
    {
        validate_sphere();
        return Ctr;
    }

    DMC_INLINE typename Vector_T::ElType Radius()
    {
        validate_sphere();
        return Rad;
    }

    // Returns true if the point is in the box.
    DMC_INLINE bool Inside(const Vector_T &P) const
    {
        return !((P.x < MinV.x || P.y < MinV.y || P.z < MinV.z ||
            P.x > MaxV.x || P.y > MaxV.y || P.z > MaxV.z));
    }

    // Returns true if any of the sphere-box is in the box.
    // XXX For speed we will return true if the P+-r box intersects the bbox.
    DMC_INLINE bool SphereIntersect(const Vector_T &P, const typename Vector_T::ElType r) const
    {
        ASSERT_D(r >= 0.0);
        return (!(P.x+r < MinV.x || P.y+r < MinV.y || P.z+r < MinV.z ||
            P.x-r > MaxV.x || P.y-r > MaxV.y || P.z-r > MaxV.z));
    }

    DMC_INLINE BBox& operator+=(const Vector_T &v)
    {
        if(valid) {
            MinV = Min(MinV, v);
            MaxV = Max(MaxV, v);
        } else {
            valid = true;
            MinV = v;
            MaxV = v;
        }

        sphere_valid = false;

        return *this;
    }

    DMC_INLINE BBox& operator+=(const BBox &b)
    {
        if(valid) {
            MinV = Min(MinV, b.MinV);
            MaxV = Max(MaxV, b.MaxV);
        } else {
            valid = true;
            MinV = b.MinV;
            MaxV = b.MaxV;
        }

        sphere_valid = false;

        return *this;
    }

    DMC_INLINE bool is_valid() const {return valid;}
};

template<class Vector_T>
DMC_INLINE std::ostream& operator<<(std::ostream& os, const BBox<Vector_T>& b)
{
    os << b.MinV.string() << b.MaxV.string();
    return os;
}

template<class Vector_T>
DMC_INLINE std::istream& operator>>(std::istream& is, BBox<Vector_T>& b)
{
    is >> b.MinV >> b.MaxV;
    return is;
}

#endif
