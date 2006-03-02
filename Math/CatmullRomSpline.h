//////////////////////////////////////////////////////////////////////
// CatmullRomSpline.h - Implements a Catmull-Rom interpolating spline.
//
// Changes Copyright David K. McAllister, Dec. 1997.
// Originally written by Steven G. Parker, Mar. 1994.

#ifndef _CatmullRomSpline_h
#define _CatmullRomSpline_h

#include "toolconfig.h"

#ifdef DMC_MACHINE_sgi
#include <vector>
using namespace std;
#endif

#ifdef DMC_MACHINE_win
#include <vector>
using namespace std;
#endif

#ifdef DMC_MACHINE_hp
#include <vector>
#endif

template<class T>
class CatmullRomSpline {
	vector<T> d;
	int nintervals;
	int nset;
	int mx;
	
public:
	CatmullRomSpline<T>();
	CatmullRomSpline<T>(const vector<T>&);
	CatmullRomSpline<T>(const int);
	CatmullRomSpline<T>(const CatmullRomSpline<T>&);
	
	void setData(const vector<T>&);
	void add(const T&);
	// void insertData(const int, const T&);
	// void removeData(const int);
	
	T operator()(double) const; // 0-1
	T& operator[](const int);
};

#endif
