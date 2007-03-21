//////////////////////////////////////////////////////////////////////
// Vector.h - A standard 3-Vector with all the trimmings.
//
// Changes Copyright David K. McAllister, July 1999.
// Originally written by Steven G. Parker, Feb. 1994.

#ifndef _t3Vector_h
#define _t3Vector_h

#include <Util/Assert.h>
#include <Math/MiscMath.h>

#include <string>

// #define DMC_VEC_DEBUG

#ifdef DMC_VEC_DEBUG
#define ASSERTVEC(x) ASSERT_R(x)
#else
#define ASSERTVEC(x)
#endif

template<class _ElType> class t3Vector;

template<class _ElType>
inline t3Vector<_ElType> Solve(const t3Vector<_ElType> &p, const t3Vector<_ElType> &u, const t3Vector<_ElType> &v, const t3Vector<_ElType> &w)
{
    _ElType det = 1/(w.z*u.x*v.y-w.z*u.y*v.x-u.z*w.x*v.y-u.x*v.z*w.y+v.z*w.x*u.y+u.z*v.x*w.y);

    return t3Vector<_ElType>(((v.x*w.y-w.x*v.y)*p.z+(v.z*w.x-v.x*w.z)*p.y+(w.z*v.y-v.z*w.y)*p.x)*det,
        -((u.x*w.y-w.x*u.y)*p.z+(u.z*w.x-u.x*w.z)*p.y+(u.y*w.z-u.z*w.y)*p.x)*det,
        ((u.x*v.y-u.y*v.x)*p.z+(v.z*u.y-u.z*v.y)*p.x+(u.z*v.x-u.x*v.z)*p.y)*det);
}

template<class _ElType>
inline t3Vector<_ElType> LinearInterp(const t3Vector<_ElType>& v1, const t3Vector<_ElType>& v2, _ElType t)
{
    ASSERTVEC(v1.init && v2.init);
    return t3Vector<_ElType>(v1.x+(v2.x-v1.x)*t,
        v1.y+(v2.y-v1.y)*t,
        v1.z+(v2.z-v1.z)*t);
}

template<class _ElType>
inline t3Vector<_ElType> CubicInterp(const t3Vector<_ElType>& v1, const t3Vector<_ElType>& v2, _ElType f)
{
    ASSERTVEC(v1.init && v2.init);

    _ElType t = f*f*(3-2*f);
    return t3Vector<_ElType>(v1.x+(v2.x-v1.x)*t,
        v1.y+(v2.y-v1.y)*t,
        v1.z+(v2.z-v1.z)*t);
}

template<class _ElType>
class t3Vector
{
public:
    _ElType x, y, z;
private:
#ifdef DMC_VEC_DEBUG
    int init;
#endif
public:
    typedef _ElType ElType; // The type of an element of the vector.
    static const int Chan = 3; // The number of channels (dimensions) in the vector

    inline t3Vector(_ElType dx, _ElType dy, _ElType dz): x(dx), y(dy), z(dz)
#ifdef DMC_VEC_DEBUG
        , init(true)
#endif
    { }
    // inline t3Vector(const t3Vector<_ElType>&);

    inline t3Vector<_ElType>()
    {
#ifdef DMC_VEC_DEBUG
        init=false;
#endif
    }

    template<class _ElTypeSrc> inline t3Vector(const t3Vector<_ElTypeSrc> &p)
    {
        x = static_cast<_ElType>(p.x);
        y = static_cast<_ElType>(p.y);
        z = static_cast<_ElType>(p.z);
    }

    template<class _ElTypeSrc> inline t3Vector(const _ElTypeSrc &p){
        x = y = z = static_cast<_ElType>(p);
    }

    inline t3Vector operator-() const;
    inline t3Vector operator+(const _ElType) const;
    inline t3Vector operator-(const _ElType) const;
    inline t3Vector operator*(const _ElType) const;
    inline t3Vector& operator*=(const _ElType);
    inline t3Vector operator/(const _ElType) const;
    inline t3Vector& operator/=(const _ElType);

    inline t3Vector operator+(const t3Vector<_ElType>&) const;
    inline t3Vector& operator+=(const t3Vector<_ElType>&);
    inline t3Vector operator-(const t3Vector<_ElType>&) const;
    inline t3Vector& operator-=(const t3Vector<_ElType>&);
    inline t3Vector& operator=(const t3Vector<_ElType>&);

    inline bool operator<(const t3Vector<_ElType>&) const; // These were added to appease STL's t3Vectors.
    inline bool operator==(const t3Vector<_ElType>&) const;
    inline bool operator!=(const t3Vector<_ElType>&) const;

    // Return an element of this Vector.
    _ElType &operator[](int p)
    {
        ASSERTVEC(p >= 0 && p < _Chan);
        return ((_ElType *)this)[p];
    }

    // Return a const element of this Vector.
    const _ElType &operator[](int p) const
    {
        ASSERTVEC(p >= 0 && p < _Chan);
        return ((_ElType *)this)[p];
    }

    inline _ElType length() const;
    inline _ElType length2() const;

    // Returns the previous length.
    inline _ElType normalize()
    {
        ASSERTVEC(init);
        _ElType l2=x*x+y*y+z*z;
        ASSERTVEC(l2 >= 0.0);
        _ElType len=Sqrt(l2);
        ASSERTVEC(len > 0.0);
        _ElType linv = 1./len;
        x*=linv;
        y*=linv;
        z*=linv;
        return len;
    }

    // Return a new vector that is the normalized this vector.
    // Doesn't modify this vector.
    inline t3Vector<_ElType> normal() const
    {
        ASSERTVEC(init);
        t3Vector<_ElType> v(*this);
        v.normalize();
        return v;
    }

    // A member function declaration means three things:
    // 1. The function can access private stuff in the class
    // 2. The function is in the scope of the class
    // 3. The function must be invoked with a this pointer
    //
    // static member functions remove the third property.
    // friend member functions remove the second and third properties.

    friend inline _ElType Dot(const t3Vector<_ElType>& v1, const t3Vector<_ElType>& v2)
    {
        ASSERTVEC(v1.init && v2.init);
        return v1.x*v2.x+v1.y*v2.y+v1.z*v2.z;
    }

    friend inline t3Vector<_ElType> Cross(const t3Vector<_ElType>& v1, const t3Vector<_ElType>& v2)
    {
        ASSERTVEC(v1.init && v2.init);
        return t3Vector<_ElType>(
            v1.y*v2.z-v1.z*v2.y,
            v1.z*v2.x-v1.x*v2.z,
            v1.x*v2.y-v1.y*v2.x);
    }

    // Component-wise absolute value.
    friend inline t3Vector<_ElType> Abs(const t3Vector<_ElType> &v)
    {
        ASSERTVEC(v.init);
        _ElType x=Abs(v.x);
        _ElType y=Abs(v.y);
        _ElType z=Abs(v.z);
        return t3Vector<_ElType>(x, y, z);
    }

    // Component-wise multiply.
    friend inline t3Vector<_ElType> CompMult(const t3Vector<_ElType>& v1, const t3Vector<_ElType>& v2)
    {
        ASSERTVEC(v1.init && v2.init);
        return t3Vector<_ElType>(v1.x*v2.x, v1.y*v2.y, v1.z*v2.z);
    }

    friend inline t3Vector<_ElType> CompDiv(const t3Vector<_ElType>& v1, const t3Vector<_ElType>& v2)
    {
        ASSERTVEC(v1.init && v2.init);
        return t3Vector<_ElType>(v1.x/v2.x, v1.y/v2.y, v1.z/v2.z);
    }

    // Find the point p in terms of the u,v,w basis.
    friend t3Vector<_ElType> Solve<>(const t3Vector<_ElType> &p, const t3Vector<_ElType> &u, const t3Vector<_ElType> &v, const t3Vector<_ElType> &w);

    // Compute the plane (N and D) given the triangle.
    inline void ComputePlane(const t3Vector<_ElType> &V0, const t3Vector<_ElType> &V1, const t3Vector<_ElType> &V2,
        t3Vector<_ElType> &N, _ElType &D);

    friend t3Vector<_ElType> LinearInterp<>(const t3Vector<_ElType>&, const t3Vector<_ElType>&, _ElType);
    friend t3Vector<_ElType> CubicInterp<>(const t3Vector<_ElType>&, const t3Vector<_ElType>&, _ElType);

    void find_orthogonal(t3Vector<_ElType>&, t3Vector<_ElType>&) const;

    std::string print() const;

    void Zero()
    {
        x = y = z = _ElType(0);
    }
};

template<class _ElType>
inline std::ostream& operator<<(std::ostream& os, const t3Vector<_ElType>& v)
{
    os << '[' << v.x << ", " << v.y << ", " << v.z << ']';

    return os;
}

template<class _ElType>
inline std::istream& operator>>(std::istream& is, t3Vector<_ElType>& v)
{
    _ElType x, y, z;
    char st;
    is >> st >> x >> st >> y >> st >> z >> st;
    v=t3Vector<_ElType>(x,y,z);
    return is;
}

template<class _ElType>
inline t3Vector<_ElType> MakeDRand(const _ElType low=0.0, const _ElType high=1.0);
template<class _ElType>
inline t3Vector<_ElType> MakeNRand(const _ElType sigma=1.0);

//////////////////////////////////////////////////////////////////////
// Implementation of all inline functions.

#if 0
template<class _ElType>
inline t3Vector<_ElType>::t3Vector<_ElType>(const t3Vector<_ElType>& p)
{
    x=p.x;
    y=p.y;
    z=p.z;
#ifdef DMC_VEC_DEBUG
    ASSERTVEC(p.init);
    init=true;
#endif
}
#endif

template<class _ElType>
inline _ElType t3Vector<_ElType>::length() const
{
#ifdef DMC_VEC_DEBUG
    ASSERTVEC(init);
#endif
    return Sqrt(x*x+y*y+z*z);
}

template<class _ElType>
inline _ElType t3Vector<_ElType>::length2() const
{
    ASSERTVEC(init);
    return x*x+y*y+z*z;
}

template<class _ElType>
inline t3Vector<_ElType>& t3Vector<_ElType>::operator=(const t3Vector<_ElType>& v)
{
    ASSERTVEC(v.init);
    x=v.x;
    y=v.y;
    z=v.z;
#ifdef DMC_VEC_DEBUG
    init=true;
#endif
    return *this;
}

template<class _ElType>
inline t3Vector<_ElType> t3Vector<_ElType>::operator*(const _ElType s) const
{
    ASSERTVEC(init);
    return t3Vector<_ElType>(x*s, y*s, z*s);
}

template<class _ElType>
inline t3Vector<_ElType> t3Vector<_ElType>::operator/(const _ElType d) const
{
    ASSERTVEC(init);
    return t3Vector<_ElType>(x/d, y/d, z/d);
}

template<class _ElType>
inline t3Vector<_ElType> t3Vector<_ElType>::operator+(const _ElType d) const
{
    ASSERTVEC(init);
    return t3Vector<_ElType>(x+d, y+d, z+d);
}

template<class _ElType>
inline t3Vector<_ElType> t3Vector<_ElType>::operator-(const _ElType d) const
{
    ASSERTVEC(init);
    return t3Vector<_ElType>(x-d, y-d, z-d);
}

template<class _ElType>
inline t3Vector<_ElType> t3Vector<_ElType>::operator+(const t3Vector<_ElType>& v) const
{
    ASSERTVEC(init && v.init);
    return t3Vector<_ElType>(x+v.x, y+v.y, z+v.z);
}

template<class _ElType>
inline t3Vector<_ElType> t3Vector<_ElType>::operator-(const t3Vector<_ElType>& v) const
{
    ASSERTVEC(init && v.init);
    return t3Vector<_ElType>(x-v.x, y-v.y, z-v.z);
}

template<class _ElType>
inline t3Vector<_ElType>& t3Vector<_ElType>::operator+=(const t3Vector<_ElType>& v)
{
    ASSERTVEC(init && v.init);
    x+=v.x;
    y+=v.y;
    z+=v.z;
    return *this;
}

template<class _ElType>
inline t3Vector<_ElType>& t3Vector<_ElType>::operator-=(const t3Vector<_ElType>& v)
{
    ASSERTVEC(init && v.init);
    x-=v.x;
    y-=v.y;
    z-=v.z;
    return *this;
}

template<class _ElType>
inline t3Vector<_ElType> t3Vector<_ElType>::operator-() const
{
    ASSERTVEC(init);
    return t3Vector<_ElType>(-x, -y, -z);
}

template<class _ElType>
inline t3Vector<_ElType>& t3Vector<_ElType>::operator*=(const _ElType d)
{
    ASSERTVEC(init);
    x*=d;
    y*=d;
    z*=d;
    return *this;
}

template<class _ElType>
inline t3Vector<_ElType>& t3Vector<_ElType>::operator/=(const _ElType d)
{
    ASSERTVEC(init);
    _ElType dinv = 1.0 / d;
    x*=dinv;
    y*=dinv;
    z*=dinv;
    return *this;
}

template<class _ElType>
inline bool t3Vector<_ElType>::operator==(const t3Vector<_ElType>& v) const
{
    ASSERTVEC(init && v.init);
    return v.x == x && v.y == y && v.z == z;
}

template<class _ElType>
inline bool t3Vector<_ElType>::operator!=(const t3Vector<_ElType>& v) const
{
    ASSERTVEC(init && v.init);
    return v.x != x || v.y != y || v.z != z;
}

template<class _ElType>
inline bool t3Vector<_ElType>::operator<(const t3Vector<_ElType>& v) const
{
    ASSERTVEC(init && v.init);
    return v.length2() < length2();
}

// Returns true if the points are within D of eachother.
template<class _ElType>
inline bool VecEq(const t3Vector<_ElType> &V0, const t3Vector<_ElType> &V1, const _ElType &DSqr = 0.0)
{
    return (V0 - V1).length2() <= DSqr;
}

#define PRDIG 8

template<class _ElType>
inline std::string t3Vector<_ElType>::print() const
{
    char xx[40], yy[40], zz[40];
    return std::string("[") + gcvt(x, PRDIG, xx) +
        std::string(", ") + gcvt(y, PRDIG, yy) +
        std::string(", ") + gcvt(z, PRDIG, zz)
#ifdef DMC_VEC_DEBUG
        + (init ? "" : " **uninit!")
#endif
        + "]";
}

template<class _ElType>
inline void t3Vector<_ElType>::find_orthogonal(t3Vector<_ElType>& v1, t3Vector<_ElType>& v2) const
{
    ASSERTVEC(init);
    t3Vector<_ElType> v0(Cross(*this, t3Vector<_ElType>(1,0,0)));
    if(v0.length2() == 0){
        v0=Cross(*this, t3Vector<_ElType>(0,1,0));
    }
    v1=Cross(*this, v0);
    v1.normalize();
    v2=Cross(*this, v1);
    v2.normalize();
}

// Given three points, find the plane that they lie on.
template<class _ElType>
inline void ComputePlane(const t3Vector<_ElType> &V0, const t3Vector<_ElType> &V1, const t3Vector<_ElType> &V2,
                         t3Vector<_ElType> &N, _ElType &D)
{
    t3Vector<_ElType> E0 = V1 - V0;
    E0.normalize();

    t3Vector<_ElType> E1 = V2 - V0;
    E1.normalize();

    N = Cross(E0, E1);
    N.normalize();

    D = -Dot(V0, N);

    // cerr << D << " " << Dot(V1, N) << " " << Dot(V2, N) << endl;
}

// Get area of this triangle.
template<class _ElType>
inline _ElType TriangleArea(const t3Vector<_ElType> &P0, const t3Vector<_ElType> &P1, const t3Vector<_ElType> &P2)
{
    t3Vector<_ElType> E1 = P1 - P0;
    t3Vector<_ElType> E2 = P2 - P1;

    return fabs(Cross(E1, E2).length()*0.5);
}

template<class _ElType>
inline t3Vector<_ElType> MakeDRand(const _ElType low, const _ElType high)
{
    return t3Vector<_ElType>(DRand(low, high), DRand(low, high), DRand(low, high));
}

template<class _ElType>
inline t3Vector<_ElType> MakeNRand(const _ElType sigma)
{
    return t3Vector<_ElType>(NRand(sigma), NRand(sigma), NRand(sigma));
}

typedef t3Vector<float> f3Vector;
typedef t3Vector<double> d3Vector;
typedef d3Vector Vector;

#define DMC_DID_INCLUDE_VECTOR_H

#endif
