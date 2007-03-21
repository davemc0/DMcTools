//////////////////////////////////////////////////////////////////////
// DownSimplex.cpp - Optimize the vector using Downhill Simplex.
//
// Changes Copyright David K. McAllister, Mar. 1999.
// Code is based on Numerical Recipes, ch. 10.

#define TINY 1.0e-10

#include <Math/DownSimplex.h>

// Sum each dimension's coordinates.
static inline HVector ComputePSum(HVector *p, int ndim)
{
    HVector psum(ndim);
    psum.zero();

    // Loop over the simplex, so ndim+1.
    for(int i=0; i<=ndim; i++)
    {
        psum += p[i];
    }

    return psum;
}

// Tries one point based on the simplex and if it's better,
// then replaces the worst point in the simplex with it.
static inline double TrySimplex(HVector *p, HVector &y, HVector &psum, int ndim,
                                double(*funk)(const HVector &, void *), void *UserData,
                                int ihi, double fac)
{
    HVector ptry(ndim);

    // These blending weights yield a point on the line between p[ihi] and the
    // midpoint of the face formed by the rest of the simplex.
    // fac=0  : on face
    // fac=-1 : reflection of p[ihi]
    // fac=1  : p[ihi]
    double fac1 =(1.0-fac)/ndim;
    double fac2 = fac1 - fac;

    // Compute a new vector to try.
    ptry = psum * fac1 - p[ihi] * fac2;

    // Try it.
    double ytry =(*funk)(ptry, UserData);

    if(ytry < y[ihi])
    {
        // Replace the high one with this one.
        y[ihi] = ytry;

        psum += ptry - p[ihi];
        p[ihi] = ptry;
    }

    return ytry;
}

// p is the points in the simplex.
double DownSimplex(HVector *p, int ndim, double ftol,
                   double(*funk)(const HVector &, void *), void *UserData, int &nfunk, const int NMax)
{
    // ihi is the point with highest error
    // inhi is the point with next-highest error
    // ilo is the point with lowest error.
    int i, ihi, ilo, inhi, mpts = ndim + 1;
    nfunk = 0;

    // y is the error of each point in the simplex.
    HVector y(mpts);

    // Compute initial error of simplex points.
    for(i=0; i<mpts; i++)
    {
        y[i] = (*funk)(p[i], UserData);
        nfunk++;
    }

    double ytry = y[0];
    HVector psum = ComputePSum(p, ndim);

    for(;;)
    {
        ilo = 0;
        ihi = y[0]>y[1] ? (inhi = 1, 0) : (inhi = 0, 1);

        // Find the lowest, highest, and next-highest error.
        for(i = 0; i < mpts; i++)
        {
            if(y[i] <= y[ilo]) ilo = i;
            if(y[i] > y[ihi])
            {
                inhi = ihi;
                ihi = i;
            }
            else if(y[i] > y[inhi] && i != ihi) inhi = i;
        }

        // Evaluate error-space tolerance.
        double RelTol = 2.0*fabs(y[ihi] - y[ilo]) /(fabs(y[ihi]) + fabs(y[ilo])+TINY);
        if(RelTol < ftol)
        {
            // Finish because we're within tolerance.
            // Place the best one in the top slot.
            double tmp = y[0]; y[0] = y[ilo]; y[ilo] = tmp;
            p[0].swap(p[ilo]);
            ytry = y[0];
            break;
        }

        // Evaluate call-count tolerance.
        if(nfunk >= NMax)
        {
            // This is usually considered an error.
            double tmp = y[0]; y[0] = y[ilo]; y[ilo] = tmp;
            p[0].swap(p[ilo]);
            ytry = y[0];
            break;
        }

        // Evaluate solution-space tolerance.
        // XXX

        // Try to improve the simplex.
        ytry = TrySimplex(p, y, psum, ndim, funk, UserData, ihi, -1.0);
        nfunk ++;
        if(ytry <= y[ilo])
        {
            // It was really good. Go again in that direction.
            // The simplex is now reversed since p[ihi] was replaced,
            // so use a positive scale factor.
            ytry = TrySimplex(p, y, psum, ndim, funk, UserData, ihi, 2.0);
            nfunk++;
        }
        else if(ytry >= y[inhi])
        {
            // We don't know if it was replaced or not.
            // But we know it's still the highest.
            double ysave = y[ihi];
            ytry = TrySimplex(p, y, psum, ndim, funk, UserData, ihi, 0.5);
            nfunk++;

            if(ytry >= ysave)
            {
                // We still haven't found a better direction.
                // Time to shrink toward the lowest.
                for(i = 0; i < mpts; i++)
                {
                    if(i != ilo)
                    {
                        p[i] = psum = (p[i] + p[ilo]) * 0.5;
                        y[i] =(*funk)(psum, UserData);
                    }
                }
                nfunk += ndim;
                psum = ComputePSum(p, ndim);
            }
        }
    }

    return ytry;
}
