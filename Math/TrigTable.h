//////////////////////////////////////////////////////////////////////
// TrigTable.h - Faster ways to do trig.
//
// Written by:
// Steven G. Parker
// Department of Computer Science
// University of Utah
// May 1994
//
// Copyright (C) 1994 SCI Group

#ifndef _TrigTable_h
#define _TrigTable_h

#include <Math/MiscMath.h>

class SinCosTable
{
	double* sindata;
	double* cosdata;
	int n;
	
public:
	inline SinCosTable(int _n, double min, double max, double ampl=1.0)
	{
		n = _n;
		sindata=new double[n];
		cosdata=new double[n];
		ASSERTERR(sindata && cosdata, "memory alloc failed");		
		double d = max-min;
		for(int i=0;i<n;i++)
		{
			double th = d*double(i)/double(n-1) + min;
			sindata[i] = sin(th)*ampl;
			cosdata[i] = cos(th)*ampl;
		}
	}
	
	inline ~SinCosTable()
	{
		delete[] sindata;
		delete[] cosdata;
	}
	
	inline double Sin(int i) const
	{
		return sindata[i];
	}
	
	inline double Cos(int i) const
	{
		return cosdata[i];
	}
};

class ASinACosTable
{
	double* asindata;
	double* acosdata;
	int n;
    double k, max, min;
	
public:
	inline ASinACosTable(int _n, double _min, double _max, double ampl=1.0)
	{
		n = _n;
        max = _max;
        min = _min;
		asindata=new double[n];
		acosdata=new double[n];
		ASSERTERR(asindata && acosdata, "memory alloc failed");		
		double d = max-min;
		for(int i=0;i<n;i++)
		{
			double th = d*double(i)/double(n-1) + min;
			asindata[i] = asin(th)*ampl;
			acosdata[i] = acos(th)*ampl;
		}
        k = (n-1.0)/d;
	}
	
	inline ~ASinACosTable()
	{
		delete[] asindata;
		delete[] acosdata;
	}
	
	inline double ASin(int i) const
	{
		return asindata[i];
	}
	
	inline double ACos(int i) const
	{
		return acosdata[i];
	}

    inline double ACos(double c) const
    {
        int i = int((c - min) * k);
        //ASSERT0(i >= 0 && i < n);
        return acosdata[i];
    }
};

#endif
