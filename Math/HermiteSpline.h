//////////////////////////////////////////////////////////////////////
// HermiteSpline.h - Implements a hermite interpolating spline.
//
// Copyright David K. McAllister, Aug. 1997.

// XXX It's not working right now.
#if 0
#ifndef _HermiteSpline_h
#define _HermiteSpline_h

#ifdef DMC_MACHINE_sgi
#include <vector.h>
#endif

#ifdef DMC_MACHINE_hp
#include <vector>
#endif

#ifdef DMC_MACHINE_win
#include <vector>
using namespace std;
#endif

template<class T>
class HermiteSpline
{
	vector<T> d; // The control points
	vector<T> p; // The tangents
	
	vector<double> h00; // The precompute basis functions
	vector<double> h01;
	vector<double> h10;
	vector<double> h11;
	int nset;
	int NumBasisSamples;
	
public:
	HermiteSpline();
	HermiteSpline( const vector<T>& );
	HermiteSpline( const vector<T>&, const vector<T>& );
	HermiteSpline( const int );
	HermiteSpline( const HermiteSpline<T>& );
	
	void SampleBasisFuncs(int NumSamples); // Precompute basis functions
	void CompleteSpline(bool GenEnds = false); // Generate tangents for C2 continuity.
	
	// Tangent is undefined.
	void setData( const vector<T>& );
	// You specify tangent.
	void setData( const vector<T>&, const vector<T>& );
	// Tangent is undefined.
	void add( const T& );
	// You specify tangent.
	void add( const T&, const T& );
	// Tangent is undefined.
	void insertData( const int, const T& );
	// You specify tangent.
	void insertData( const int, const T&, const T& );
	void removeData( const int );
	
	T sample( double ) const; // sample with pre-computed basis functions
	T operator()( double ) const; // 0 -> (nset-1)
	
	// This provides no way to access the tangent.
	T& operator[]( const int );
	
	inline int size() const
	{
		return d.size();
	}
};

#endif
#endif
