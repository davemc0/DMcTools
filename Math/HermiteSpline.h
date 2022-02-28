//////////////////////////////////////////////////////////////////////
// HermiteSpline.h - Implements a Hermite interpolating spline
//
// Copyright David K. McAllister, 1997, 2009.

// XXX This code may not be working right.

#pragma once

#include "Util/toolconfig.h"

#include <vector>

template <class T, class Param> class HermiteSpline {
    std::vector<T> d; // The control points
    std::vector<T> p; // The tangents

    std::vector<Param> h00; // The precomputed basis functions
    std::vector<Param> h01;
    std::vector<Param> h10;
    std::vector<Param> h11;
    int nset;
    int NumBasisSamples;

public:
    HermiteSpline();
    HermiteSpline(const std::vector<T>&);
    HermiteSpline(const std::vector<T>&, const std::vector<T>&);
    HermiteSpline(const int);
    HermiteSpline(const HermiteSpline<T, Param>&);

    void SampleBasisFuncs(int NumSamples);     // Precompute basis functions
    void CompleteSpline(bool GenEnds = false); // Generate tangents for C2 continuity.

    // Tangent is undefined.
    void setData(const std::vector<T>&);
    // You specify tangent.
    void setData(const std::vector<T>&, const std::vector<T>&);

    // Tangent is undefined.
    void add(const T&);
    // You specify tangent.
    void add(const T&, const T&);

    // Tangent is undefined.
    void insertData(const int, const T&);
    // You specify tangent.
    void insertData(const int, const T&, const T&);
    void removeData(const int);

    T sample(Param) const;     // Sample with pre-computed basis functions
    T operator()(Param) const; // 0 -> (nset-1)

    // This provides no way to access the tangent.
    T& operator[](const int);

    DMC_DECL int size() const { return d.size(); }
};

template <class T, class Param> DMC_DECL HermiteSpline<T, Param>::HermiteSpline() : d(0), p(0), nset(0), NumBasisSamples(0) {}

template <class T, class Param>
DMC_DECL HermiteSpline<T, Param>::HermiteSpline(const std::vector<T>& data) : d(data), p(data.size()), nset(data.size()), NumBasisSamples(0)
{
}

template <class T, class Param>
DMC_DECL HermiteSpline<T, Param>::HermiteSpline(const std::vector<T>& data, const std::vector<T>& tang) :
    d(data), p(tang), nset(data.size()), NumBasisSamples(0)
{
    ASSERT_D(data.size() == tang.size());
}

template <class T, class Param> DMC_DECL HermiteSpline<T, Param>::HermiteSpline(const int n) : d(n), nset(n), NumBasisSamples(0) {}

template <class T, class Param>
DMC_DECL HermiteSpline<T, Param>::HermiteSpline(const HermiteSpline<T, Param>& s) : d(s.d), p(s.p), nset(s.nset), NumBasisSamples(0)
{
    ASSERT_D(d.size() == p.size());
    ASSERT_D(d.size() == nset);
}

template <class T, class Param> void DMC_DECL HermiteSpline<T, Param>::setData(const std::vector<T>& data)
{
    d = data;
    nset = data.size();
    p.resize(nset);
}

template <class T, class Param> void DMC_DECL HermiteSpline<T, Param>::setData(const std::vector<T>& data, const std::vector<T>& tang)
{
    d = data;
    p = tang;
    nset = data.size();
    ASSERT_D(data.size() == tang.size());
}

template <class T, class Param> void DMC_DECL HermiteSpline<T, Param>::add(const T& obj)
{
    d.add(obj);
    p.grow(1);
    nset++;
}

template <class T, class Param> void DMC_DECL HermiteSpline<T, Param>::add(const T& obj, const T& tan)
{
    d.add(obj);
    p.add(tan);
    nset++;
}

template <class T, class Param> void DMC_DECL HermiteSpline<T, Param>::insertData(const int idx, const T& obj)
{
    d.insert(idx, obj);
    T tmp;
    p.insert(idx, tmp);
    nset++;
}

template <class T, class Param> void DMC_DECL HermiteSpline<T, Param>::insertData(const int idx, const T& obj, const T& tan)
{
    d.insert(idx, obj);
    p.insert(idx, tan);
    nset++;
}

template <class T, class Param> void DMC_DECL HermiteSpline<T, Param>::removeData(const int idx)
{
    d.remove(idx);
    p.remove(idx);
    nset--;
}

// Sample at intervals specified by SampleBasisFuncs
template <class T, class Param> T DMC_DECL HermiteSpline<T, Param>::sample(Param x) const
{
    int i = int(x);
    int iP1 = i + 1;
    int k = int((x - Param(i)) * Param(NumBasisSamples));

    ASSERT_D(nset >= 2);
    ASSERT_D(iP1 < nset);
    ASSERT_D(i >= 0);
    ASSERT_D(NumBasisSamples > k);

    return (d[i] * h00[k] + d[iP1] * h10[k] + p[i] * h01[k] + p[iP1] * h11[k]);
}

// Sample at an arbitrary t value
template <class T, class Param> T DMC_DECL HermiteSpline<T, Param>::operator()(Param t) const
{
    int i = int(t);
    int iP1 = i + 1;
    Param x = t - Param(i);

    // cerr << "oper() i=" << i << " x=" << x << " t=" << t << endl;
    ASSERT_D(nset >= 2);
    ASSERT_D(iP1 < nset);
    ASSERT_D(i >= 0);
    // cerr << "d.size() = " << d.size() << " p.size() = " << p.size() << endl;

    return (d[i] * ((2.0 * x + 1.0) * (x - 1.0) * (x - 1.0)) + d[iP1] * ((-2.0 * x + 3.0) * x * x) + p[i] * (x * (x - 1.0) * (x - 1.0)) +
            p[iP1] * (x * x * (x - 1.0)));
}

// Read / write control points.
template <class T, class Param> T& DMC_DECL HermiteSpline<T, Param>::operator[](const int idx) { return d[idx]; }

// Make the Hermite basis functions.
template <class T, class Param> void DMC_DECL HermiteSpline<T, Param>::SampleBasisFuncs(int NumSamples)
{
    Param dt = 1. / NumBasisSamples;
    NumBasisSamples = NumSamples;

    h00.resize(NumBasisSamples);
    h01.resize(NumBasisSamples);
    h10.resize(NumBasisSamples);
    h11.resize(NumBasisSamples);

    int i = 0;
    for (Param x = 0.0; i < NumBasisSamples; i++, x += dt) {
        h00[i] = (2.0 * x + 1.0) * (x - 1.0) * (x - 1.0);
        h01[i] = x * (x - 1.0) * (x - 1.0);
        h10[i] = (-2.0 * x + 3.0) * x * x;
        h11[i] = x * x * (x - 1.0);
    }
}

// This function solves the tridiagonal matrix for the complete parametric spline. Solves the (t,x) and (t,y) systems simultaneously.
template <class T, class Param> void DMC_DECL HermiteSpline<T, Param>::CompleteSpline(bool GenEndTangents)
{
    ASSERT_D(nset == d.size());
    ASSERT_D(nset >= 2);

    std::vector<Param> a(nset), c(nset), D(nset);
    std::vector<T> b(nset);
    Param scale;
    int i, iM1, iP1, nM1 = nset - 1;

    // Fill in the known data
    T E = d[1] - d[0];

    a[0] = 0.0;
    D[0] = 1.0;
    c[0] = 0.0;
    if (GenEndTangents) p[0] = d[1] - d[0];
    b[0] = p[0];

    for (i = 1, iM1 = 0, iP1 = 2; i < nM1; iM1 = i, i = iP1, iP1++) {
        a[i] = 1.0;
        c[i] = 1.0;
        D[i] = 4.0;

        T EM1 = E;
        E = d[iP1] - d[i];
        b[i] = (EM1 + E) * 3.0;
    }
    a[nset - 1] = 0.0;
    c[nset - 1] = 0.0;
    D[nset - 1] = 1.0;
    if (GenEndTangents) p[nset - 1] = d[nset - 1] - d[nset - 2];
    b[nset - 1] = p[nset - 1];

    // Do forward elimination
    for (i = 1, iM1 = 0; i < nset; iM1 = i, i++) {
        scale = a[i] / D[iM1];
        D[i] -= scale * c[iM1];
        b[i] -= b[iM1] * scale;
    }

    // Do back substitution
    T prev = p[nset - 1];
    for (i = nset - 2; i >= 0; i--) prev = p[i] = (b[i] - prev * c[i]) / D[i];
}
