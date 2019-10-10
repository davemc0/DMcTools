//////////////////////////////////////////////////////////////////////
// CatmullRomSpline.h - Implements a Catmull-Rom interpolating spline
//
// Copyright David K. McAllister, 1997-2007.

#pragma once

#include "toolconfig.h"

#include <vector>

// Just hand it any STL vector with your data and a parameter on 0..1 and it will return the interpolated value.
// Interpolates all points except first and last. So t=0 returns d[1] and t=1 returns d[size()-2].
// Typically T is float, double, f3Vector, etc. and Param is float or double.
template<class T, class Param>
T CatmullRomSpline(const std::vector<T>& d, Param x) // 0..1
{
    int nset = int(d.size());
    int nintervals = nset-3;
    int mx = nset - 4;
    ASSERT_D(nset >= 4);

    Param xs = x*nintervals;
    int idx = (int)xs;
    Param t = xs-idx;
    if(idx<0) { idx=0; t=0; }
    if(idx>mx) { idx=mx; t=1; }
    Param t2 = t*t;
    Param t3 = t*t*t;

    return ((d[idx]*-1 + d[idx+1]*3 + d[idx+2]*-3 + d[idx+3]) *(t3*0.5)+
        (d[idx]*2 + d[idx+1]*-5 + d[idx+2]*4 + d[idx+3]*-1)*(t2*0.5)+
        (d[idx]*-1 + d[idx+2] )*(t*0.5)+
        ( d[idx+1] ));
}
