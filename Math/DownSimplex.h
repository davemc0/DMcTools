//////////////////////////////////////////////////////////////////////
// DownSimplex.h - Optimize the vector using Downhill Simplex
//
// Copyright David K. McAllister, Mar. 1999.

#pragma once

#include "Math/HVector.h"

#define NMAX 1000

// p is the initial ndim+1 points in the simplex.
// ndim is the number of dimensions.
// ftol is the error tolerance to achieve.
// funk is the error functional to evaluate a try.
// UserData is passed into funk.
// nfunk returns the function call count.
// NMax is the max number of function calls.
// Returns the error of the answer p[0].

template<class Elem_T>
Elem_T DownSimplex(HVector<Elem_T> *p, size_t ndim, Elem_T ftol,
                          Elem_T (*funk)(const HVector<Elem_T> &, void *),
                          void *UserData, int &nfunk, const int NMax = NMAX);
