//////////////////////////////////////////////////////////////////////
// VectorField.h - An interface to vector fields
//
// Copyright David K. McAllister, 2006.

#pragma once

#include "Util/Assert.h"
#include "Math/MiscMath.h"

#include <utility>
#include <vector>

// Space_T is the data type that locates a point in the field (input)
// Vector_T is the data type returned by the field (output)
template<class Space_T, class Vector_T>
class VectorField
{
public:
    virtual Vector_T Sample(const Space_T &P) const = 0;
};

template<class Space_T, class Vector_T>
class RBFVectorField : public VectorField<Space_T, Vector_T>
{
    typedef std::pair<Space_T, Vector_T> RBFCenter;

    std::vector<RBFCenter> Centers;
    double Sigma, Mu;

public:
    virtual Vector_T Sample(const Space_T &P) const
    {
        Vector_T Sum; Sum.Zero();
        Vector_T::ElType TotalWgt = 0;

        for(std::vector<RBFCenter>::const_iterator it = Centers.begin(); it != Centers.end(); it++) {
            Space_T::ElType dist = Space_T::ElType(0);
            dist = (it->first - P).length();
            Space_T::ElType Wgt = Gaussian(dist, Sigma, Mu);

            TotalWgt += Wgt;
            Sum += it->second * Wgt;
        }

        Sum /= TotalWgt;
        // std::cerr << Sum << TotalWgt << std::endl;

        return Sum;
    }

    void Insert(const Space_T &P, const Vector_T &V)
    {
        Centers.push_back(RBFCenter(P, V));
    }

    // Tell it the parameters for interpolation
    void InterpGaussian(const double Sigma_, const double Mu_)
    {
        Sigma = Sigma_;
        Mu = Mu_;
    }
};
