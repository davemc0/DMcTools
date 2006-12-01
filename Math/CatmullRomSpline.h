//////////////////////////////////////////////////////////////////////
// CatmullRomSpline.h - Implements a Catmull-Rom interpolating spline.
//
// Changes Copyright David K. McAllister, Dec. 1997.
// Originally written by Steven G. Parker, Mar. 1994.

#ifndef _CatmullRomSpline_h
#define _CatmullRomSpline_h

#include "toolconfig.h"

#include <vector>

template<class T>
class CatmullRomSpline {
	std::vector<T> d;
	int nintervals;
	int nset;
	int mx;
	
public:
	CatmullRomSpline<T>();
	CatmullRomSpline<T>(const std::vector<T>&);
	CatmullRomSpline<T>(const int);
	CatmullRomSpline<T>(const CatmullRomSpline<T>&);
	
	void setData(const std::vector<T>&);
	void add(const T&);
	// void insertData(const int, const T&);
	// void removeData(const int);
	
	T operator()(double) const; // 0-1
	T& operator[](const int);
};

#endif
