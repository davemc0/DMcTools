//////////////////////////////////////////////////////////////////////
// Vector.h - Implements geometric vectors, esp. float three-vectors
//
// Copyright David K. McAllister, July 1999, Feb. 2022.
// Was patterned after Steve Parker's 1995 code, then on Timo Aila's 2010 code

#pragma once

#include "Math/MiscMath.h"
#include "Util/toolconfig.h"

#include <cmath>
#include <iosfwd>

#if defined(min) || defined(max)
#error "Must #define NOMINMAX before #include <windows.h>."
#endif

#define ASSERTVEC(x)

#ifndef CUDACC
// Base types for storage come from CUDA, but if CUDA isn't used, define base types here
struct int2 {
    int x, y;
};
struct int3 {
    int x, y, z;
};
struct int4 {
    int x, y, z, w;
};

struct float2 {
    float x, y;
};
struct float3 {
    float x, y, z;
};
struct alignas(16) float4 {
    float x, y, z, w;
};

struct double2 {
    double x, y;
};
struct double3 {
    double x, y, z;
};
struct alignas(32) double4 {
    double x, y, z, w;
};
#endif

////////////////////////////////////////////////

template <class T, int L, class S> class tVector {
public:
    typedef typename T ElType; // The type of an element of the vector
    static const int Chan = L; // The number of channels (dimensions) in the vector

    DMC_DECL tVector() {}

    DMC_DECL const T* getPtr() const { return ((S*)this)->getPtr(); }
    DMC_DECL T* getPtr() { return ((S*)this)->getPtr(); }
    DMC_DECL const T& get(int idx) const
    {
        ASSERTVEC(idx >= 0 && idx < L);
        return getPtr()[idx];
    }
    DMC_DECL T& get(int idx)
    {
        ASSERTVEC(idx >= 0 && idx < L);
        return getPtr()[idx];
    }
    DMC_DECL T set(int idx, const T& a)
    {
        T& slot = get(idx);
        T old = slot;
        slot = a;
        return old;
    }

    DMC_DECL void set(const T& a)
    {
        T* tp = getPtr();
        for (int i = 0; i < L; i++) tp[i] = a;
    }
    DMC_DECL void set(const T* ptr)
    {
        ASSERTVEC(ptr);
        T* tp = getPtr();
        for (int i = 0; i < L; i++) tp[i] = ptr[i];
    }
    DMC_DECL void setZero() { set((T)0); }
    template <class V> DMC_DECL void set(const tVector<T, L, V>& v) { set(v.getPtr()); }

    // Reduction operations
    DMC_DECL T min() const
    {
        const T* tp = getPtr();
        T r = tp[0];
        for (int i = 1; i < L; i++) r = ::min(r, tp[i]); // Use min defined with above macro
        return r;
    }
    DMC_DECL T max() const
    {
        const T* tp = getPtr();
        T r = tp[0];
        for (int i = 1; i < L; i++) r = ::max(r, tp[i]); // Use max defined with above macro
        return r;
    }
    DMC_DECL T sum() const
    {
        const T* tp = getPtr();
        T r = tp[0];
        for (int i = 1; i < L; i++) r += tp[i];
        return r;
    }
    DMC_DECL int minDim() const // Return index of min dimension
    {
        const T* tp = getPtr();
        T r = tp[0];
        int dim = 0;
        for (int i = 1; i < L; i++)
            if (tp[i] < r) {
                r = tp[i];
                dim = i;
            }
        return dim;
    }
    DMC_DECL int maxDim() const // Return index of max dimension
    {
        const T* tp = getPtr();
        T r = tp[0];
        int dim = 0;
        for (int i = 1; i < L; i++)
            if (tp[i] > r) {
                r = tp[i];
                dim = i;
            }
        return dim;
    }
    template <class V> DMC_DECL bool operator==(const tVector<T, L, V>& v) const
    {
        const T* tp = getPtr();
        const T* vp = v.getPtr();
        for (int i = 0; i < L; i++)
            if (tp[i] != vp[i]) return false;
        return true;
    }
    template <class V> DMC_DECL bool operator!=(const tVector<T, L, V>& v) const { return (!operator==(v)); }

    // Returns true if the points are within D of each other
    template <class V> friend DMC_DECL bool isNear(const tVector<T, L, V>& t, const tVector<T, L, V>& v, const T& DSqr = 0) { return (t - v).lenSqr() <= DSqr; }

    DMC_DECL T lenSqr() const
    {
        const T* tp = getPtr();
        T r = (T)0;
        for (int i = 0; i < L; i++) r += sqr(tp[i]);
        return r;
    }
    DMC_DECL T length() const { return sqrt(lenSqr()); }
    DMC_DECL S normalized(T len = (T)1) const { return operator*(len* rcp(length())); }
    DMC_DECL void normalize(T len = (T)1) { set(normalized(len)); }

    DMC_DECL bool any_inf() const
    {
        const T* tp = getPtr();
        for (int i = 0; i < L; i++)
            if (std::isinf(tp[i])) return true;
        return false;
    }
    DMC_DECL bool all_inf() const
    {
        const T* tp = getPtr();
        for (int i = 0; i < L; i++)
            if (!std::isinf(tp[i])) return false;
        return true;
    }
    DMC_DECL bool any_nan() const
    {
        const T* tp = getPtr();
        for (int i = 0; i < L; i++)
            if (std::isnan(tp[i])) return true;
        return false;
    }
    DMC_DECL bool all_nan() const
    {
        const T* tp = getPtr();
        for (int i = 0; i < L; i++)
            if (!std::isnan(tp[i])) return false;
        return true;
    }
    DMC_DECL bool any_zero() const
    {
        const T* tp = getPtr();
        for (int i = 0; i < L; i++)
            if (tp[i] == (T)0) return true;
        return false;
    }
    DMC_DECL bool all_zero() const
    {
        const T* tp = getPtr();
        for (int i = 0; i < L; i++)
            if (tp[i] != (T)0) return false;
        return true;
    }

    // Vector-scalar operations
    DMC_DECL const T& operator[](int idx) const { return get(idx); }
    DMC_DECL T& operator[](int idx) { return get(idx); }

    DMC_DECL S& operator=(const T& a)
    {
        set(a);
        return *(S*)this;
    }
    DMC_DECL S& operator+=(const T& a)
    {
        set(operator+(a));
        return *(S*)this;
    }
    DMC_DECL S& operator-=(const T& a)
    {
        set(operator-(a));
        return *(S*)this;
    }
    DMC_DECL S& operator*=(const T& a)
    {
        set(operator*(a));
        return *(S*)this;
    }
    DMC_DECL S& operator/=(const T& a)
    {
        set(operator/(a));
        return *(S*)this;
    }
    DMC_DECL S& operator%=(const T& a)
    {
        set(operator%(a));
        return *(S*)this;
    }
    DMC_DECL S& operator&=(const T& a)
    {
        set(operator&(a));
        return *(S*)this;
    }
    DMC_DECL S& operator|=(const T& a)
    {
        set(operator|(a));
        return *(S*)this;
    }
    DMC_DECL S& operator^=(const T& a)
    {
        set(operator^(a));
        return *(S*)this;
    }
    DMC_DECL S& operator<<=(const T& a)
    {
        set(operator<<(a));
        return *(S*)this;
    }
    DMC_DECL S& operator>>=(const T& a)
    {
        set(operator>>(a));
        return *(S*)this;
    }

    DMC_DECL S operator+() const { return *this; }
    DMC_DECL S operator-() const
    {
        const T* tp = getPtr();
        S r;
        T* rp = r.getPtr();
        for (int i = 0; i < L; i++) rp[i] = -tp[i];
        return r;
    }
    DMC_DECL S operator~() const
    {
        const T* tp = getPtr();
        S r;
        T* rp = r.getPtr();
        for (int i = 0; i < L; i++) rp[i] = ~tp[i];
        return r;
    }
    DMC_DECL S operator+(const T& a) const
    {
        const T* tp = getPtr();
        S r;
        T* rp = r.getPtr();
        for (int i = 0; i < L; i++) rp[i] = tp[i] + a;
        return r;
    }
    DMC_DECL S operator-(const T& a) const
    {
        const T* tp = getPtr();
        S r;
        T* rp = r.getPtr();
        for (int i = 0; i < L; i++) rp[i] = tp[i] - a;
        return r;
    }
    DMC_DECL S operator*(const T& a) const
    {
        const T* tp = getPtr();
        S r;
        T* rp = r.getPtr();
        for (int i = 0; i < L; i++) rp[i] = tp[i] * a;
        return r;
    }
    DMC_DECL S operator/(const T& a) const
    {
        const T* tp = getPtr();
        S r;
        T* rp = r.getPtr();
        for (int i = 0; i < L; i++) rp[i] = tp[i] / a;
        return r;
    }
    DMC_DECL S operator%(const T& a) const
    {
        const T* tp = getPtr();
        S r;
        T* rp = r.getPtr();
        for (int i = 0; i < L; i++) rp[i] = tp[i] % a;
        return r;
    }
    DMC_DECL S operator&(const T& a) const
    {
        const T* tp = getPtr();
        S r;
        T* rp = r.getPtr();
        for (int i = 0; i < L; i++) rp[i] = tp[i] & a;
        return r;
    }
    DMC_DECL S operator|(const T& a) const
    {
        const T* tp = getPtr();
        S r;
        T* rp = r.getPtr();
        for (int i = 0; i < L; i++) rp[i] = tp[i] | a;
        return r;
    }
    DMC_DECL S operator^(const T& a) const
    {
        const T* tp = getPtr();
        S r;
        T* rp = r.getPtr();
        for (int i = 0; i < L; i++) rp[i] = tp[i] ^ a;
        return r;
    }
    DMC_DECL S operator<<(const T& a) const
    {
        const T* tp = getPtr();
        S r;
        T* rp = r.getPtr();
        for (int i = 0; i < L; i++) rp[i] = tp[i] << a;
        return r;
    }
    DMC_DECL S operator>>(const T& a) const
    {
        const T* tp = getPtr();
        S r;
        T* rp = r.getPtr();
        for (int i = 0; i < L; i++) rp[i] = tp[i] >> a;
        return r;
    }

    // Component-wise operations
    template <class V> DMC_DECL S& operator=(const tVector<T, L, V>& v)
    {
        set(v);
        return *(S*)this;
    }
    template <class V> DMC_DECL S& operator+=(const tVector<T, L, V>& v)
    {
        set(operator+(v));
        return *(S*)this;
    }
    template <class V> DMC_DECL S& operator-=(const tVector<T, L, V>& v)
    {
        set(operator-(v));
        return *(S*)this;
    }
    template <class V> DMC_DECL S& operator*=(const tVector<T, L, V>& v)
    {
        set(operator*(v));
        return *(S*)this;
    }
    template <class V> DMC_DECL S& operator/=(const tVector<T, L, V>& v)
    {
        set(operator/(v));
        return *(S*)this;
    }
    template <class V> DMC_DECL S& operator%=(const tVector<T, L, V>& v)
    {
        set(operator%(v));
        return *(S*)this;
    }
    template <class V> DMC_DECL S& operator&=(const tVector<T, L, V>& v)
    {
        set(operator&(v));
        return *(S*)this;
    }
    template <class V> DMC_DECL S& operator|=(const tVector<T, L, V>& v)
    {
        set(operator|(v));
        return *(S*)this;
    }
    template <class V> DMC_DECL S& operator^=(const tVector<T, L, V>& v)
    {
        set(operator^(v));
        return *(S*)this;
    }
    template <class V> DMC_DECL S& operator<<=(const tVector<T, L, V>& v)
    {
        set(operator<<(v));
        return *(S*)this;
    }
    template <class V> DMC_DECL S& operator>>=(const tVector<T, L, V>& v)
    {
        set(operator>>(v));
        return *(S*)this;
    }

    // Component-wise const operations
    DMC_DECL S abs() const
    {
        const T* tp = getPtr();
        S r;
        T* rp = r.getPtr();
        for (int i = 0; i < L; i++) rp[i] = ::abs(tp[i]);
        return r;
    }
    template <class V> DMC_DECL T dot(const tVector<T, L, V>& v) const
    {
        const T* tp = getPtr();
        const T* vp = v.getPtr();
        T r = (T)0;
        for (int i = 0; i < L; i++) r += tp[i] * vp[i];
        return r;
    }
    template <class V> DMC_DECL S min(const tVector<T, L, V>& v) const
    {
        const T* tp = getPtr();
        const T* vp = v.getPtr();
        S r;
        T* rp = r.getPtr();
        for (int i = 0; i < L; i++) rp[i] = ::min(tp[i], vp[i]); // Use min defined with above macro
        return r;
    }
    template <class V> DMC_DECL S max(const tVector<T, L, V>& v) const
    {
        const T* tp = getPtr();
        const T* vp = v.getPtr();
        S r;
        T* rp = r.getPtr();
        for (int i = 0; i < L; i++) rp[i] = ::max(tp[i], vp[i]); // Use max defined with above macro
        return r;
    }
    template <class V, class W> DMC_DECL S clamp(const tVector<T, L, V>& lo, const tVector<T, L, W>& hi) const
    {
        const T* tp = getPtr();
        const T* lop = lo.getPtr();
        const T* hip = hi.getPtr();
        S r;
        T* rp = r.getPtr();
        for (int i = 0; i < L; i++) rp[i] = ::clamp(tp[i], lop[i], hip[i]);
        return r;
    }
    template <class V> DMC_DECL S operator+(const tVector<T, L, V>& v) const
    {
        const T* tp = getPtr();
        const T* vp = v.getPtr();
        S r;
        T* rp = r.getPtr();
        for (int i = 0; i < L; i++) rp[i] = tp[i] + vp[i];
        return r;
    }
    template <class V> DMC_DECL S operator-(const tVector<T, L, V>& v) const
    {
        const T* tp = getPtr();
        const T* vp = v.getPtr();
        S r;
        T* rp = r.getPtr();
        for (int i = 0; i < L; i++) rp[i] = tp[i] - vp[i];
        return r;
    }
    template <class V> DMC_DECL S operator*(const tVector<T, L, V>& v) const
    {
        const T* tp = getPtr();
        const T* vp = v.getPtr();
        S r;
        T* rp = r.getPtr();
        for (int i = 0; i < L; i++) rp[i] = tp[i] * vp[i];
        return r;
    }
    template <class V> DMC_DECL S operator/(const tVector<T, L, V>& v) const
    {
        const T* tp = getPtr();
        const T* vp = v.getPtr();
        S r;
        T* rp = r.getPtr();
        for (int i = 0; i < L; i++) rp[i] = tp[i] / vp[i];
        return r;
    }
    template <class V> DMC_DECL S operator%(const tVector<T, L, V>& v) const
    {
        const T* tp = getPtr();
        const T* vp = v.getPtr();
        S r;
        T* rp = r.getPtr();
        for (int i = 0; i < L; i++) rp[i] = tp[i] % vp[i];
        return r;
    }
    template <class V> DMC_DECL S operator&(const tVector<T, L, V>& v) const
    {
        const T* tp = getPtr();
        const T* vp = v.getPtr();
        S r;
        T* rp = r.getPtr();
        for (int i = 0; i < L; i++) rp[i] = tp[i] & vp[i];
        return r;
    }
    template <class V> DMC_DECL S operator|(const tVector<T, L, V>& v) const
    {
        const T* tp = getPtr();
        const T* vp = v.getPtr();
        S r;
        T* rp = r.getPtr();
        for (int i = 0; i < L; i++) rp[i] = tp[i] | vp[i];
        return r;
    }
    template <class V> DMC_DECL S operator^(const tVector<T, L, V>& v) const
    {
        const T* tp = getPtr();
        const T* vp = v.getPtr();
        S r;
        T* rp = r.getPtr();
        for (int i = 0; i < L; i++) rp[i] = tp[i] ^ vp[i];
        return r;
    }
    template <class V> DMC_DECL S operator<<(const tVector<T, L, V>& v) const
    {
        const T* tp = getPtr();
        const T* vp = v.getPtr();
        S r;
        T* rp = r.getPtr();
        for (int i = 0; i < L; i++) rp[i] = tp[i] << vp[i];
        return r;
    }
    template <class V> DMC_DECL S operator>>(const tVector<T, L, V>& v) const
    {
        const T* tp = getPtr();
        const T* vp = v.getPtr();
        S r;
        T* rp = r.getPtr();
        for (int i = 0; i < L; i++) rp[i] = tp[i] >> vp[i];
        return r;
    }
};

////////////////////////////////////////////////

class i2vec : public tVector<int, 2, i2vec>, public int2 {
public:
    DMC_DECL i2vec() { setZero(); }
    DMC_DECL i2vec(int a) { set(a); }
    DMC_DECL i2vec(int xx, int yy)
    {
        x = xx;
        y = yy;
    }

    DMC_DECL const int* getPtr() const { return &x; }
    DMC_DECL int* getPtr() { return &x; }
    static DMC_DECL i2vec fromPtr(const int* ptr) { return i2vec(ptr[0], ptr[1]); }

    template <class V> DMC_DECL i2vec(const tVector<int, 2, V>& v) { set(v); }
    template <class V> DMC_DECL i2vec& operator=(const tVector<int, 2, V>& v)
    {
        set(v);
        return *this;
    }
};

class f2vec : public tVector<float, 2, f2vec>, public float2 {
public:
    DMC_DECL f2vec() { setZero(); }
    DMC_DECL f2vec(float a) { set(a); }
    DMC_DECL f2vec(float xx, float yy)
    {
        x = xx;
        y = yy;
    }

    DMC_DECL const float* getPtr() const { return &x; }
    DMC_DECL float* getPtr() { return &x; }
    static DMC_DECL f2vec fromPtr(const float* ptr) { return f2vec(ptr[0], ptr[1]); }

    template <class V> DMC_DECL f2vec(const tVector<float, 2, V>& v) { set(v); }
    template <class V> DMC_DECL f2vec& operator=(const tVector<float, 2, V>& v)
    {
        set(v);
        return *this;
    }
};

class d2vec : public tVector<double, 2, d2vec>, public double2 {
public:
    DMC_DECL d2vec() { setZero(); }
    DMC_DECL d2vec(double a) { set(a); }
    DMC_DECL d2vec(double xx, double yy)
    {
        x = xx;
        y = yy;
    }

    DMC_DECL const double* getPtr() const { return &x; }
    DMC_DECL double* getPtr() { return &x; }
    static DMC_DECL d2vec fromPtr(const double* ptr) { return d2vec(ptr[0], ptr[1]); }

    template <class V> DMC_DECL d2vec(const tVector<double, 2, V>& v) { set(v); }
    template <class V> DMC_DECL d2vec& operator=(const tVector<double, 2, V>& v)
    {
        set(v);
        return *this;
    }
};

////////////////////////////////////////////////

class i3vec : public tVector<int, 3, i3vec>, public int3 {
public:
    DMC_DECL i3vec() { setZero(); }
    DMC_DECL i3vec(int a) { set(a); }
    DMC_DECL i3vec(int xx, int yy, int zz)
    {
        x = xx;
        y = yy;
        z = zz;
    }

    DMC_DECL const int* getPtr() const { return &x; }
    DMC_DECL int* getPtr() { return &x; }
    static DMC_DECL i3vec fromPtr(const int* ptr) { return i3vec(ptr[0], ptr[1], ptr[2]); }

    template <class V> DMC_DECL i3vec(const tVector<int, 3, V>& v) { set(v); }
    template <class V> DMC_DECL i3vec& operator=(const tVector<int, 3, V>& v)
    {
        set(v);
        return *this;
    }
};

class f3vec : public tVector<float, 3, f3vec>, public float3 {
public:
    DMC_DECL f3vec() { setZero(); }
    DMC_DECL f3vec(float a) { set(a); }
    DMC_DECL f3vec(i3vec v)
    {
        x = (float)v.x;
        y = (float)v.y;
        z = (float)v.z;
    }
    DMC_DECL f3vec(float xx, float yy, float zz)
    {
        x = xx;
        y = yy;
        z = zz;
    }
    DMC_DECL f3vec(const float* const v)
    {
        x = v[0];
        y = v[1];
        z = v[2];
    }

    DMC_DECL const float* getPtr() const { return &x; }
    DMC_DECL float* getPtr() { return &x; }
    static DMC_DECL f3vec fromPtr(const float* ptr) { return f3vec(ptr[0], ptr[1], ptr[2]); }
    DMC_DECL operator i3vec() const { return i3vec((int)x, (int)y, (int)z); }

    template <class V> DMC_DECL f3vec(const tVector<float, 3, V>& v) { set(v); }
    template <class V> DMC_DECL f3vec(const tVector<float, 4, V>& v) { set(v.getPtr()); }
    template <class V> DMC_DECL f3vec& operator=(const tVector<float, 3, V>& v)
    {
        set(v);
        return *this;
    }
};

DMC_DECL f3vec Cross(const f3vec& v1, const f3vec& v2) { return f3vec(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x); }

class d3vec : public tVector<double, 3, d3vec>, public double3 {
public:
    DMC_DECL d3vec() { setZero(); }
    DMC_DECL d3vec(double xx, double yy, double zz)
    {
        x = xx;
        y = yy;
        z = zz;
    }
    DMC_DECL d3vec(float xx, float yy, float zz)
    {
        x = double(xx);
        y = double(yy);
        z = double(zz);
    }
    DMC_DECL d3vec(f3vec v)
    {
        x = double(v.x);
        y = double(v.y);
        z = double(v.z);
    }
    DMC_DECL const double* getPtr() const { return &x; }
    DMC_DECL double* getPtr() { return &x; }
    static DMC_DECL d3vec fromPtr(const double* ptr) { return d3vec(ptr[0], ptr[1], ptr[2]); }

    template <class V> DMC_DECL d3vec(const tVector<double, 3, V>& v) { set(v); }
    template <class V> DMC_DECL d3vec& operator=(const tVector<double, 3, V>& v)
    {
        set(v);
        return *this;
    }
};

DMC_DECL d3vec Cross(const d3vec& v1, const d3vec& v2) { return d3vec(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x); }

////////////////////////////////////////////////

class i4vec : public tVector<int, 4, i4vec>, public int4 {
public:
    DMC_DECL i4vec() { setZero(); }
    DMC_DECL i4vec(int a) { set(a); }
    DMC_DECL i4vec(int xx, int yy, int zz, int ww)
    {
        x = xx;
        y = yy;
        z = zz;
        w = ww;
    }
    DMC_DECL i4vec(const i3vec& xyz, int ww)
    {
        x = xyz.x;
        y = xyz.y;
        z = xyz.z;
        w = ww;
    }

    DMC_DECL const int* getPtr() const { return &x; }
    DMC_DECL int* getPtr() { return &x; }
    static DMC_DECL i4vec fromPtr(const int* ptr) { return i4vec(ptr[0], ptr[1], ptr[2], ptr[3]); }

    DMC_DECL i3vec getXYZ() const { return i3vec(x, y, z); }
    DMC_DECL i3vec getXYW() const { return i3vec(x, y, w); }

    template <class V> DMC_DECL i4vec(const tVector<int, 4, V>& v) { set(v); }
    template <class V> DMC_DECL i4vec& operator=(const tVector<int, 4, V>& v)
    {
        set(v);
        return *this;
    }
};

class f4vec : public tVector<float, 4, f4vec>, public float4 {
public:
    DMC_DECL f4vec() { setZero(); }
    DMC_DECL f4vec(float a) { set(a); }
    DMC_DECL f4vec(float xx, float yy, float zz, float ww)
    {
        x = xx;
        y = yy;
        z = zz;
        w = ww;
    }
    DMC_DECL f4vec(const f3vec& xyz, float ww)
    {
        x = xyz.x;
        y = xyz.y;
        z = xyz.z;
        w = ww;
    }

    DMC_DECL const float* getPtr() const { return &x; }
    DMC_DECL float* getPtr() { return &x; }
    static DMC_DECL f4vec fromPtr(const float* ptr) { return f4vec(ptr[0], ptr[1], ptr[2], ptr[3]); }

    DMC_DECL f3vec getXYZ() const { return f3vec(x, y, z); }
    DMC_DECL f3vec getXYW() const { return f3vec(x, y, w); }

    template <class V> DMC_DECL f4vec(const tVector<float, 4, V>& v) { set(v); }
    template <class V> DMC_DECL f4vec& operator=(const tVector<float, 4, V>& v)
    {
        set(v);
        return *this;
    }
};

class d4vec : public tVector<double, 4, d4vec>, public double4 {
public:
    DMC_DECL d4vec() { setZero(); }
    DMC_DECL d4vec(double a) { set(a); }
    DMC_DECL d4vec(double xx, double yy, double zz, double ww)
    {
        x = xx;
        y = yy;
        z = zz;
        w = ww;
    }
    DMC_DECL d4vec(const d3vec& xyz, double ww)
    {
        x = xyz.x;
        y = xyz.y;
        z = xyz.z;
        w = ww;
    }

    DMC_DECL const double* getPtr() const { return &x; }
    DMC_DECL double* getPtr() { return &x; }
    static DMC_DECL d4vec fromPtr(const double* ptr) { return d4vec(ptr[0], ptr[1], ptr[2], ptr[3]); }

    DMC_DECL d3vec getXYZ() const { return d3vec(x, y, z); }
    DMC_DECL d3vec getXYW() const { return d3vec(x, y, w); }

    template <class V> DMC_DECL d4vec(const tVector<double, 4, V>& v) { set(v); }
    template <class V> DMC_DECL d4vec& operator=(const tVector<double, 4, V>& v)
    {
        set(v);
        return *this;
    }
};

////////////////////////////////////////////////
// Bare functions, mostly operating on a vector and a scalar

template <class T, int L, class S> DMC_DECL T min(const tVector<T, L, S>& v) { return v.min(); }
template <class T, int L, class S> DMC_DECL T max(const tVector<T, L, S>& v) { return v.max(); }
template <class T, int L, class S> DMC_DECL T sum(const tVector<T, L, S>& v) { return v.sum(); }
template <class T, int L, class S> DMC_DECL S abs(const tVector<T, L, S>& v) { return v.abs(); }
template <class T, int L, class S> DMC_DECL S normalize(const tVector<T, L, S>& v, T len = (T)1) { return v.normalized(len); }
template <class T, int L, class S> DMC_DECL T lenSqr(const tVector<T, L, S>& v) { return v.lenSqr(); }
template <class T, int L, class S> DMC_DECL T length(const tVector<T, L, S>& v) { return v.length(); }
template <class T, int L, class S, class V> DMC_DECL T dot(const tVector<T, L, S>& a, const tVector<T, L, V>& b) { return a.dot(b); }

template <class T, int L, class S> DMC_DECL S operator+(const T& a, const tVector<T, L, S>& b) { return b + a; }
template <class T, int L, class S> DMC_DECL S operator-(const T& a, const tVector<T, L, S>& b) { return -b + a; }
template <class T, int L, class S> DMC_DECL S operator*(const T& a, const tVector<T, L, S>& b) { return b * a; }
template <class T, int L, class S> DMC_DECL S operator/(const T& a, const tVector<T, L, S>& b)
{
    const T* bp = b.getPtr();
    S r;
    T* rp = r.getPtr();
    for (int i = 0; i < L; i++) rp[i] = a / bp[i];
    return r;
}
template <class T, int L, class S> DMC_DECL S operator%(const T& a, const tVector<T, L, S>& b)
{
    const T* bp = b.getPtr();
    S r;
    T* rp = r.getPtr();
    for (int i = 0; i < L; i++) rp[i] = a % bp[i];
    return r;
}
template <class T, int L, class S> DMC_DECL S operator&(const T& a, const tVector<T, L, S>& b) { return b & a; }
template <class T, int L, class S> DMC_DECL S operator|(const T& a, const tVector<T, L, S>& b) { return b | a; }
template <class T, int L, class S> DMC_DECL S operator^(const T& a, const tVector<T, L, S>& b) { return b ^ a; }
template <class T, int L, class S> DMC_DECL S operator<<(const T& a, const tVector<T, L, S>& b)
{
    const T* bp = b.getPtr();
    S r;
    T* rp = r.getPtr();
    for (int i = 0; i < L; i++) rp[i] = a << bp[i];
    return r;
}
template <class T, int L, class S> DMC_DECL S operator>>(const T& a, const tVector<T, L, S>& b)
{
    const T* bp = b.getPtr();
    S r;
    T* rp = r.getPtr();
    for (int i = 0; i < L; i++) rp[i] = a >> bp[i];
    return r;
}

// Component-wise vector-vector min/max/clamp
#define VECCOMPMINMAX(V)                                                                              \
    DMC_DECL V min(const V& a, const V& b) { return a.min(b); }                                       \
    DMC_DECL V min(V& a, V& b) { return a.min(b); }                                                   \
    DMC_DECL V max(const V& a, const V& b) { return a.max(b); }                                       \
    DMC_DECL V max(V& a, V& b) { return a.max(b); }                                                   \
    DMC_DECL V min(const V& a, const V& b, const V& c) { return a.min(b).min(c); }                    \
    DMC_DECL V min(V& a, V& b, V& c) { return a.min(b).min(c); }                                      \
    DMC_DECL V max(const V& a, const V& b, const V& c) { return a.max(b).max(c); }                    \
    DMC_DECL V max(V& a, V& b, V& c) { return a.max(b).max(c); }                                      \
    DMC_DECL V min(const V& a, const V& b, const V& c, const V& d) { return a.min(b).min(c).min(d); } \
    DMC_DECL V min(V& a, V& b, V& c, V& d) { return a.min(b).min(c).min(d); }                         \
    DMC_DECL V max(const V& a, const V& b, const V& c, const V& d) { return a.max(b).max(c).max(d); } \
    DMC_DECL V max(V& a, V& b, V& c, V& d) { return a.max(b).max(c).max(d); }                         \
    DMC_DECL V clamp(const V& v, const V& lo, const V& hi) { return v.clamp(lo, hi); }                \
    DMC_DECL V clamp(V& v, V& lo, V& hi) { return v.clamp(lo, hi); }

VECCOMPMINMAX(i2vec)
VECCOMPMINMAX(f2vec)
VECCOMPMINMAX(d2vec)
VECCOMPMINMAX(i3vec)
VECCOMPMINMAX(f3vec)
VECCOMPMINMAX(d3vec)
VECCOMPMINMAX(i4vec)
VECCOMPMINMAX(f4vec)
VECCOMPMINMAX(d4vec)
#undef VECCOMPMINMAX

template <class T, int L, class S> DMC_DECL std::ostream& operator<<(std::ostream& os, const tVector<T, L, S>& b);
template <class T, int L, class S> DMC_DECL std::istream& operator>>(std::istream& is, tVector<T, L, S>& b);
