//////////////////////////////////////////////////////////////////////
// DownSimplex.cpp - Optimize the vector using Downhill Simplex
//
// Copyright David K. McAllister, Mar. 1999.
// Code is based on Numerical Recipes, ch. 10.


#include "Math/DownSimplex.h"

namespace {
  const double TINY = 1.0e-10;

  // Sum each dimension's coordinates.
  template<class Elem_T>
  DMC_DECL HVector<Elem_T> ComputePSum(HVector<Elem_T> *p, size_t ndim)
  {
    HVector<Elem_T> psum(ndim);
    psum.zero();

    // Loop over the simplex, so ndim+1.
    for(size_t i=0; i<=ndim; i++) {
      psum += p[i];
    }

    return psum;
  }

  // Tries one point based on the simplex and if it's better,
  // then replaces the worst point in the simplex with it.
  template<class Elem_T>
  DMC_DECL Elem_T TrySimplex(HVector<Elem_T> *p, HVector<Elem_T> &y, HVector<Elem_T> &psum, size_t ndim,
    Elem_T(*funk)(const HVector<Elem_T> &, void *), void *UserData,
    size_t ihi, Elem_T fac)
  {
    HVector<Elem_T> ptry(ndim);

    // These blending weights yield a point on the line between p[ihi] and the
    // midpoint of the face formed by the rest of the simplex.
    // fac=0  : on face
    // fac=-1 : reflection of p[ihi]
    // fac=1  : p[ihi]
    Elem_T fac1 =(1.0-fac)/ndim;
    Elem_T fac2 = fac1 - fac;

    // Compute a new vector to try.
    ptry = psum * fac1 - p[ihi] * fac2;

    // Try it.
    Elem_T ytry =(*funk)(ptry, UserData);

    if(ytry < y[ihi]) {
      // Replace the high one with this one.
      y[ihi] = ytry;

      psum += ptry - p[ihi];
      p[ihi] = ptry;
    }

    return ytry;
  }

};

// p is the points in the simplex.
template<class Elem_T>
Elem_T DownSimplex(HVector<Elem_T> *p, size_t ndim, Elem_T ftol,
                   Elem_T(*funk)(const HVector<Elem_T> &, void *), void *UserData, int &nfunk, const int NMax)
{
  // ihi is the point with highest error
  // inhi is the point with next-highest error
  // ilo is the point with lowest error.
  int i, ihi, ilo, inhi;
  size_t mpts = ndim + 1;
  nfunk = 0;

  // y is the error of each point in the simplex.
  HVector<Elem_T> y(mpts);

  // Compute initial error of simplex points.
  for(i=0; i<static_cast<int>(mpts); i++) {
    y[i] = (*funk)(p[i], UserData);
    nfunk++;
  }

  Elem_T ytry = y[0];
  HVector<Elem_T> psum = ComputePSum(p, ndim);

  for(;;) {
    ilo = 0;
    ihi = y[0]>y[1] ? (inhi = 1, 0) : (inhi = 0, 1);

    // Find the lowest, highest, and next-highest error.
    for (i = 0; i < static_cast<int>(mpts); i++) {
      if(y[i] <= y[ilo]) ilo = i;
      if(y[i] > y[ihi]) {
        inhi = ihi;
        ihi = i;
      } else if(y[i] > y[inhi] && i != ihi) inhi = i;
    }

    // Evaluate error-space tolerance.
    Elem_T RelTol = 2*fabs(y[ihi] - y[ilo]) /(fabs(y[ihi]) + fabs(y[ilo])+TINY);
    if(RelTol < ftol) {
      // Finish because we're within tolerance.
      // Place the best one in the top slot.
      Elem_T tmp = y[0]; y[0] = y[ilo]; y[ilo] = tmp;
      p[0].swap(p[ilo]);
      ytry = y[0];
      break;
    }

    // Evaluate call-count tolerance.
    if(nfunk >= NMax) {
      // This is usually considered an error.
      Elem_T tmp = y[0]; y[0] = y[ilo]; y[ilo] = tmp;
      p[0].swap(p[ilo]);
      ytry = y[0];
      break;
    }

    // Evaluate solution-space tolerance.
    // XXX

    // Try to improve the simplex.
    ytry = TrySimplex(p, y, psum, ndim, funk, UserData, ihi, static_cast<Elem_T>(-1));
    nfunk++;
    if(ytry <= y[ilo]) {
      // It was really good. Go again in that direction.
      // The simplex is now reversed since p[ihi] was replaced,
      // so use a positive scale factor.
      ytry = TrySimplex(p, y, psum, ndim, funk, UserData, ihi, static_cast<Elem_T>(2));
      nfunk++;
    } else if(ytry >= y[inhi]) {
      // We don't know if it was replaced or not.
      // But we know it's still the highest.
      Elem_T ysave = y[ihi];
      ytry = TrySimplex(p, y, psum, ndim, funk, UserData, ihi, static_cast<Elem_T>(0.5));
      nfunk++;

      if(ytry >= ysave) {
        // We still haven't found a better direction.
        // Time to shrink toward the lowest.
          for (i = 0; i < static_cast<int>(mpts); i++) {
          if(i != ilo) {
            p[i] = psum = (p[i] + p[ilo]) * 0.5;
            y[i] =(*funk)(psum, UserData);
          }
        }
        nfunk += static_cast<int>(ndim);
        psum = ComputePSum(p, ndim);
      }
    }
  }

  return ytry;
}

template
float DownSimplex(HVector<float> *p, size_t ndim, float ftol,
                  float(*funk)(const HVector<float> &, void *), void *UserData, int &nfunk, const int NMax);

template
double DownSimplex(HVector<double> *p, size_t ndim, double ftol,
                  double(*funk)(const HVector<double> &, void *), void *UserData, int &nfunk, const int NMax);
