//////////////////////////////////////////////////////////////////////
// VectorField.h - An interface to vector fields
//
// Copyright David K. McAllister, 2006.

#ifndef _VectorField_h
#define _VectorField_h

#include <Util/Assert.h>
#include <Math/MiscMath.h>

#include <utility>
#include <vector>

// _SpaceType is the data type that locates a point in the field (input)
// _VecType is the data type returned by the field (output)
template<class _SpaceType, class _VecType>
class VectorField
{
public:
	virtual _VecType Sample(const _SpaceType &P) const = 0;
};


template<class _SpaceType, class _VecType>
class RBFVectorField : public VectorField<_SpaceType, _VecType>
{
	typedef std::pair<_SpaceType, _VecType> RBFCenter;

	std::vector<RBFCenter> Centers;
	double Sigma, Mu;

public:
	virtual _VecType Sample(const _SpaceType &P) const
	{
		_VecType Sum; Sum.Zero();
		_VecType::ElType TotalWgt = 0;

		for(std::vector<RBFCenter>::const_iterator it = Centers.begin(); it != Centers.end(); it++) {
			_SpaceType::ElType dist = _SpaceType::ElType(0);
			dist = (it->first - P).length();
			_SpaceType::ElType Wgt = Gaussian(dist, Sigma, Mu);

			TotalWgt += Wgt;
			Sum += it->second * Wgt;
		}

		Sum /= TotalWgt;
		// std::cerr << Sum << TotalWgt << std::endl;

		return Sum;
	}

	void Insert(const _SpaceType &P, const _VecType &V)
	{
		Centers.push_back(RBFCenter(P, V));
	}

	// Tell it the parameters for interpolation
	void InterpGaussian(const double _Sigma, const double _Mu)
	{
		Sigma = _Sigma;
		Mu = _Mu;
	}
};


#endif
