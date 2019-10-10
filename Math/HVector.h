//////////////////////////////////////////////////////////////////////
// HVector.h - Math for long vectors
//
// Copyright David K. McAllister, Mar. 1998.

#pragma once

#include "Math/HMatrix.h"
#include "Math/MiscMath.h"

#include <vector>
#include <string>

template<class Elem_T>
class HVector
{
public:
    // Operators on Elem_Ts.

    DMC_DECL HVector<Elem_T>(size_t siz=0) : V(siz) {}

    DMC_DECL size_t size() const { return V.size(); }
    DMC_DECL const typename Elem_T& operator[](const size_t i) const { return V[i]; }
    DMC_DECL typename Elem_T& operator[](const size_t i) { return V[i]; }

    DMC_DECL HVector<Elem_T> operator*(const Elem_T d) const
    {
        HVector<Elem_T> b(size());
        for(size_t i=0; i<size(); i++)
            b[i] = V[i] * d;

        return b;
    }

    DMC_DECL HVector<Elem_T> & operator*=(const Elem_T d)
    {
        for(size_t i=0; i<size(); i++)
            V[i] = V[i] * d;

        return *this;
    }

    DMC_DECL HVector<Elem_T> & operator/=(const Elem_T d)
    {
        for(size_t i=0; i<size(); i++)
            V[i] = V[i] / d;

        return *this;
    }

    DMC_DECL HVector<Elem_T> & operator+=(const Elem_T d)
    {
        for(size_t i=0; i<size(); i++)
            V[i] = V[i] + d;

        return *this;
    }

    DMC_DECL HVector<Elem_T> & operator-=(const Elem_T d)
    {
        for(size_t i=0; i<size(); i++)
            V[i] = V[i] - d;

        return *this;
    }

    // Add a constant to each element.
    DMC_DECL HVector<Elem_T> operator+(const Elem_T d) const
    {
        HVector<Elem_T> b(size());
        for(size_t i=0; i<size(); i++)
            b[i] = V[i] + d;

        return b;
    }

    // Operators on HVectors.

    DMC_DECL HVector<Elem_T> operator+(const HVector<Elem_T> &A) const
    {
        ASSERT_R(A.size() == size());

        HVector<Elem_T> b(size());
        for(size_t i=0; i<size(); i++)
            b[i] = V[i] + A[i];

        return b;
    }

    DMC_DECL HVector<Elem_T> & operator+=(const HVector<Elem_T> &A)
    {
        ASSERT_R(A.size() == size());

        for(size_t i=0; i<size(); i++)
            V[i] += A[i];

        return *this;
    }

    DMC_DECL HVector<Elem_T> operator-(const HVector<Elem_T> &A) const
    {
        ASSERT_R(A.size() == size());

        HVector<Elem_T> b(size());
        for(size_t i=0; i<size(); i++)
            b[i] = V[i] - A[i];

        return b;
    }

    friend DMC_DECL Elem_T Dot(const HVector<Elem_T> &A, const HVector<Elem_T> &B)
    {
        ASSERT_R(A.size() == B.size());

        Elem_T sum = static_cast<Elem_T>(0);
        for(size_t i=0; i<size(); i++)
            sum += A[i] * B[i];

        return sum;
    }

    // Swap the data in these two vectors.
    DMC_DECL void swap(HVector<Elem_T> &A)
    {
        ASSERT_R(A.size() == size());

        for(size_t i=0; i<size(); i++) {
            Elem_T tmp = V[i]; V[i] = A[i]; A[i] = tmp;
        }
    }

    DMC_DECL HVector<Elem_T> & zero()
    {
        for(size_t i=0; i<size(); i++)
            V[i] = static_cast<Elem_T>(0);

        return *this;
    }

    //Return a string with this vector printed in it.
    std::string print() const;

    // Fill the vector with rands on 0.0 -> 1.0.
    HVector<Elem_T> & rand();

    // Fill the vector with a normal distribution of random numbers,
    // with the given standard deviation.
    HVector<Elem_T> & nrand(const Elem_T sigma = 1.0);

    DMC_DECL Elem_T length2() const
    {
        ASSERT_R(size() > 0);

        Elem_T l2 = static_cast<Elem_T>(0);
        for(size_t i=0; i<size(); i++) {
            l2 += Sqr(V[i]);
        }

        return l2;
    }

    DMC_DECL Elem_T length() const
    {
        return dmcm::Sqrt(length2());
    }

    friend HMatrix<Elem_T> OuterProd(const HVector<Elem_T> &A, const HVector<Elem_T> &B);

    // Takes an array of HVectors, all the same size.
    friend HMatrix<Elem_T> Covariance(const HVector<Elem_T> *VA, const size_t count);

    friend HVector<Elem_T> Mean(const HVector<Elem_T> *VA, const size_t count);

private:
    std::vector<Elem_T> V;
};

template<class Elem_T>
std::ostream& operator<<(std::ostream& os, const HVector<Elem_T>& p);
template<class Elem_T>
std::istream& operator>>(std::istream& os, HVector<Elem_T>& p);
