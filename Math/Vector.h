//////////////////////////////////////////////////////////////////////
// Vector.h - A standard 3-Vector with all the trimmings.
//
// Changes Copyright David K. McAllister, July 1999.
// Originally written by Steven G. Parker, Feb. 1994.

#ifndef dmc_t3Vector_h
#define dmc_t3Vector_h

#include "Util/Assert.h"
#include "Math/MiscMath.h"

#include <string>

// #define DMC_VEC_DEBUG

#ifdef DMC_VEC_DEBUG
#define ASSERTVEC(x) ASSERT_R(x)
#else
#define ASSERTVEC(x)
#endif

template<class Elem_T>
class t3Vector
{
public:
    Elem_T x, y, z;
private:
#ifdef DMC_VEC_DEBUG
    int init;
#endif
public:
    typedef Elem_T ElType; // The type of an element of the vector.
    static const int Chan = 3; // The number of channels (dimensions) in the vector

    t3Vector(Elem_T dx, Elem_T dy, Elem_T dz): x(dx), y(dy), z(dz)
#ifdef DMC_VEC_DEBUG
        , init(true)
#endif
    { }

    t3Vector<Elem_T>()
    {
#ifdef DMC_VEC_DEBUG
        init=false;
#endif
    }

    template<class SrcElem_T> t3Vector(const t3Vector<SrcElem_T> &p)
    {
        x = static_cast<Elem_T>(p.x);
        y = static_cast<Elem_T>(p.y);
        z = static_cast<Elem_T>(p.z);
    }

    t3Vector(const Elem_T &p){
        x = y = z = static_cast<Elem_T>(p);
    }

    t3Vector operator-() const;
    t3Vector operator+(const Elem_T) const;
    t3Vector operator-(const Elem_T) const;
    t3Vector operator*(const Elem_T) const;
    t3Vector& operator*=(const Elem_T);
    t3Vector operator/(const Elem_T) const;
    t3Vector& operator/=(const Elem_T);

    t3Vector operator+(const t3Vector<Elem_T>&) const;
    t3Vector& operator+=(const t3Vector<Elem_T>&);
    t3Vector operator-(const t3Vector<Elem_T>&) const;
    t3Vector& operator-=(const t3Vector<Elem_T>&);

    bool operator<(const t3Vector<Elem_T>&) const; // These were added to appease STL's t3Vectors.
    bool operator==(const t3Vector<Elem_T>&) const;
    bool operator!=(const t3Vector<Elem_T>&) const;

    // Return an element of this Vector.
    Elem_T &operator[](int p)
    {
        ASSERTVEC(p >= 0 && p < Chan);
        return ((Elem_T *)this)[p];
    }

    // Return a const element of this Vector.
    const Elem_T &operator[](int p) const
    {
        ASSERTVEC(p >= 0 && p < Chan);
        return ((Elem_T *)this)[p];
    }

    Elem_T length() const
    {
        ASSERTVEC(init);
        return Sqrt(x*x+y*y+z*z);
    }

    Elem_T length2() const;

    // Returns the previous length.
    Elem_T normalize()
    {
        ASSERTVEC(init);
        Elem_T l2=x*x+y*y+z*z;
        ASSERTVEC(l2 >= 0.0);
        Elem_T len=Sqrt(l2);
        ASSERTVEC(len > 0.0);
        Elem_T linv = 1./len;
        x*=linv;
        y*=linv;
        z*=linv;
        return len;
    }

    // Return a new vector that is the normalized this vector.
    // Doesn't modify this vector.
    t3Vector<Elem_T> normal() const
    {
        ASSERTVEC(init);
        t3Vector<Elem_T> v(*this);
        v.normalize();
        return v;
    }

    void Zero() { x = y = z = Elem_T(0); }

    // Find two vectors that, with this one, form an orthogonal frame
    // this may not be normalized, but the other two are.
    void find_orthogonal(t3Vector<Elem_T> &v1, t3Vector<Elem_T> &v2) const
    {
        ASSERTVEC(init);
        t3Vector<Elem_T> v0(Cross(*this, t3Vector<Elem_T>(1,0,0)));
        if(v0.length2() == 0){
            v0=Cross(*this, t3Vector<Elem_T>(0,1,0));
        }
        v1=Cross(*this, v0);
        v1.normalize();
        v2=Cross(*this, v1);
        v2.normalize();
    }

    // A member function declaration means three things:
    // 1. The function can access private stuff in the class
    // 2. The function is in the scope of the class
    // 3. The function must be invoked with a this pointer
    //
    // static member functions remove the third property.
    // friend member functions remove the second and third properties.

    std::string string() const
    {
        const int PRDIG = 8;
        char xx[40], yy[40], zz[40];
        return std::string("[") + gcvt(x, PRDIG, xx) + std::string(", ") + gcvt(y, PRDIG, yy) + std::string(", ") + gcvt(z, PRDIG, zz)
#ifdef DMC_VEC_DEBUG
            + (init ? "" : " **uninit!")
#endif
            + "]";
    }
};

/////////////////////////////////////////////////
// Component-wise operations

template<class Elem_T>
DMC_INLINE t3Vector<Elem_T> Abs(const t3Vector<Elem_T> &v)
{
    ASSERTVEC(v.init);
    Elem_T x=Abs(v.x);
    Elem_T y=Abs(v.y);
    Elem_T z=Abs(v.z);
    return t3Vector<Elem_T>(x, y, z);
}

template<class Elem_T>
DMC_INLINE t3Vector<Elem_T> Min(const t3Vector<Elem_T>& v1, const t3Vector<Elem_T>& v2)
{
    ASSERTVEC(v1.init && v2.init);
    return t3Vector<Elem_T>(std::min(v1.x,v2.x), std::min(v1.y,v2.y), std::min(v1.z,v2.z));
}

template<class Elem_T>
DMC_INLINE t3Vector<Elem_T> Max(const t3Vector<Elem_T>& v1, const t3Vector<Elem_T>& v2)
{
    ASSERTVEC(v1.init && v2.init);
    return t3Vector<Elem_T>(std::max(v1.x,v2.x), std::max(v1.y,v2.y), std::max(v1.z,v2.z));
}

template<class Elem_T>
DMC_INLINE t3Vector<Elem_T> CompMult(const t3Vector<Elem_T>& v1, const t3Vector<Elem_T>& v2)
{
    ASSERTVEC(v1.init && v2.init);
    return t3Vector<Elem_T>(v1.x*v2.x, v1.y*v2.y, v1.z*v2.z);
}

template<class Elem_T>
DMC_INLINE t3Vector<Elem_T> CompDiv(const t3Vector<Elem_T>& v1, const t3Vector<Elem_T>& v2)
{
    ASSERTVEC(v1.init && v2.init);
    return t3Vector<Elem_T>(v1.x/v2.x, v1.y/v2.y, v1.z/v2.z);
}

/////////////////////////////////////////////////
// Utility operations

template<class Elem_T>
DMC_INLINE std::ostream& operator<<(std::ostream& os, const t3Vector<Elem_T>& v)
{
    os << '[' << v.x << ", " << v.y << ", " << v.z << ']';

    return os;
}

template<class Elem_T>
DMC_INLINE std::istream& operator>>(std::istream& is, t3Vector<Elem_T>& v)
{
    Elem_T x, y, z;
    char st;
    is >> st >> x >> st >> y >> st >> z >> st;
    v=t3Vector<Elem_T>(x,y,z);
    return is;
}

/////////////////////////////////////////////////
// Multi-vector math operations

template<class Elem_T>
DMC_INLINE Elem_T Dot(const t3Vector<Elem_T>& v1, const t3Vector<Elem_T>& v2)
{
    ASSERTVEC(v1.init && v2.init);
    return v1.x*v2.x+v1.y*v2.y+v1.z*v2.z;
}

template<class Elem_T>
DMC_INLINE t3Vector<Elem_T> Cross(const t3Vector<Elem_T>& v1, const t3Vector<Elem_T>& v2)
{
    ASSERTVEC(v1.init && v2.init);
    return t3Vector<Elem_T>(
        v1.y*v2.z-v1.z*v2.y,
        v1.z*v2.x-v1.x*v2.z,
        v1.x*v2.y-v1.y*v2.x);
}

// Find the point p in terms of the u,v,w basis.
template<class Elem_T>
DMC_INLINE t3Vector<Elem_T> Solve(const t3Vector<Elem_T> &p, const t3Vector<Elem_T> &u, const t3Vector<Elem_T> &v, const t3Vector<Elem_T> &w)
{
    Elem_T det = 1/(w.z*u.x*v.y-w.z*u.y*v.x-u.z*w.x*v.y-u.x*v.z*w.y+v.z*w.x*u.y+u.z*v.x*w.y);

    return t3Vector<Elem_T>(((v.x*w.y-w.x*v.y)*p.z+(v.z*w.x-v.x*w.z)*p.y+(w.z*v.y-v.z*w.y)*p.x)*det,
        -((u.x*w.y-w.x*u.y)*p.z+(u.z*w.x-u.x*w.z)*p.y+(u.y*w.z-u.z*w.y)*p.x)*det,
        ((u.x*v.y-u.y*v.x)*p.z+(v.z*u.y-u.z*v.y)*p.x+(u.z*v.x-u.x*v.z)*p.y)*det);
}

template<class Elem_T>
DMC_INLINE t3Vector<Elem_T> LinearInterp(const t3Vector<Elem_T>& v1, const t3Vector<Elem_T>& v2, Elem_T t)
{
    ASSERTVEC(v1.init && v2.init);
    return t3Vector<Elem_T>(v1.x+(v2.x-v1.x)*t,
        v1.y+(v2.y-v1.y)*t,
        v1.z+(v2.z-v1.z)*t);
}

template<class Elem_T>
DMC_INLINE t3Vector<Elem_T> CubicInterp(const t3Vector<Elem_T>& v1, const t3Vector<Elem_T>& v2, Elem_T f)
{
    ASSERTVEC(v1.init && v2.init);
    Elem_T t = f*f*(3-2*f);
    return t3Vector<Elem_T>(v1.x+(v2.x-v1.x)*t, v1.y+(v2.y-v1.y)*t, v1.z+(v2.z-v1.z)*t);
}

// Given three points, find the plane (N and D) that they lie on.
template<class Elem_T>
DMC_INLINE void ComputePlane(const t3Vector<Elem_T> &V0, const t3Vector<Elem_T> &V1, const t3Vector<Elem_T> &V2,
                         t3Vector<Elem_T> &N, Elem_T &D)
{
    t3Vector<Elem_T> E0 = V1 - V0;
    E0.normalize();
    t3Vector<Elem_T> E1 = V2 - V0;
    E1.normalize();
    N = Cross(E0, E1);
    N.normalize();
    D = -Dot(V0, N);
}

// Get area of this triangle.
template<class Elem_T>
DMC_INLINE Elem_T TriangleArea(const t3Vector<Elem_T> &P0, const t3Vector<Elem_T> &P1, const t3Vector<Elem_T> &P2)
{
    t3Vector<Elem_T> E1 = P1 - P0;
    t3Vector<Elem_T> E2 = P2 - P1;

    return fabs(Cross(E1, E2).length()*Elem_T(0.5));
}

template<class Elem_T>
DMC_INLINE t3Vector<Elem_T> MakeDRand(const Elem_T low, const Elem_T high)
{
    return t3Vector<Elem_T>(DRand(low, high), DRand(low, high), DRand(low, high));
}

template<class Elem_T>
DMC_INLINE t3Vector<Elem_T> MakeNRand(const Elem_T sigma = 1)
{
    return t3Vector<Elem_T>(NRand(sigma), NRand(sigma), NRand(sigma));
}

template<class Elem_T>
DMC_INLINE bool length2_less(const t3Vector<Elem_T>& v1, const t3Vector<Elem_T>& v2)
{
    ASSERTVEC(init && v.init);
    return v1.length2() < v2.length2();
}

// Returns true if the points are within D of eachother.
template<class Elem_T>
DMC_INLINE bool VecEq(const t3Vector<Elem_T> &V0, const t3Vector<Elem_T> &V1, const Elem_T &DSqr = 0.0)
{
    return (V0 - V1).length2() <= DSqr;
}

//////////////////////////////////////////////////////////////////////
// Implementation of all DMC_INLINE functions.

template<class Elem_T>
DMC_INLINE Elem_T t3Vector<Elem_T>::length2() const
{
    ASSERTVEC(init);
    return x*x+y*y+z*z;
}

template<class Elem_T>
DMC_INLINE t3Vector<Elem_T> t3Vector<Elem_T>::operator*(const Elem_T s) const
{
    ASSERTVEC(init);
    return t3Vector<Elem_T>(x*s, y*s, z*s);
}

template<class Elem_T>
DMC_INLINE t3Vector<Elem_T> t3Vector<Elem_T>::operator/(const Elem_T d) const
{
    ASSERTVEC(init);
    return t3Vector<Elem_T>(x/d, y/d, z/d);
}

template<class Elem_T>
DMC_INLINE t3Vector<Elem_T> t3Vector<Elem_T>::operator+(const Elem_T d) const
{
    ASSERTVEC(init);
    return t3Vector<Elem_T>(x+d, y+d, z+d);
}

template<class Elem_T>
DMC_INLINE t3Vector<Elem_T> t3Vector<Elem_T>::operator-(const Elem_T d) const
{
    ASSERTVEC(init);
    return t3Vector<Elem_T>(x-d, y-d, z-d);
}

template<class Elem_T>
DMC_INLINE t3Vector<Elem_T> t3Vector<Elem_T>::operator+(const t3Vector<Elem_T>& v) const
{
    ASSERTVEC(init && v.init);
    return t3Vector<Elem_T>(x+v.x, y+v.y, z+v.z);
}

template<class Elem_T>
DMC_INLINE t3Vector<Elem_T> t3Vector<Elem_T>::operator-(const t3Vector<Elem_T>& v) const
{
    ASSERTVEC(init && v.init);
    return t3Vector<Elem_T>(x-v.x, y-v.y, z-v.z);
}

template<class Elem_T>
DMC_INLINE t3Vector<Elem_T>& t3Vector<Elem_T>::operator+=(const t3Vector<Elem_T>& v)
{
    ASSERTVEC(init && v.init);
    x+=v.x;
    y+=v.y;
    z+=v.z;
    return *this;
}

template<class Elem_T>
DMC_INLINE t3Vector<Elem_T>& t3Vector<Elem_T>::operator-=(const t3Vector<Elem_T>& v)
{
    ASSERTVEC(init && v.init);
    x-=v.x;
    y-=v.y;
    z-=v.z;
    return *this;
}

template<class Elem_T>
DMC_INLINE t3Vector<Elem_T> t3Vector<Elem_T>::operator-() const
{
    ASSERTVEC(init);
    return t3Vector<Elem_T>(-x, -y, -z);
}

template<class Elem_T>
DMC_INLINE t3Vector<Elem_T>& t3Vector<Elem_T>::operator*=(const Elem_T d)
{
    ASSERTVEC(init);
    x*=d;
    y*=d;
    z*=d;
    return *this;
}

template<class Elem_T>
DMC_INLINE t3Vector<Elem_T>& t3Vector<Elem_T>::operator/=(const Elem_T d)
{
    ASSERTVEC(init);
    Elem_T dinv = 1.0 / d;
    x*=dinv;
    y*=dinv;
    z*=dinv;
    return *this;
}

template<class Elem_T>
DMC_INLINE bool t3Vector<Elem_T>::operator==(const t3Vector<Elem_T>& v) const
{
    ASSERTVEC(init && v.init);
    return v.x == x && v.y == y && v.z == z;
}

template<class Elem_T>
DMC_INLINE bool t3Vector<Elem_T>::operator!=(const t3Vector<Elem_T>& v) const
{
    ASSERTVEC(init && v.init);
    return v.x != x || v.y != y || v.z != z;
}

typedef t3Vector<float> f3Vector;
typedef t3Vector<double> d3Vector;
typedef d3Vector Vector;

#endif
