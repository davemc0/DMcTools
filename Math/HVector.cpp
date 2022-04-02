//////////////////////////////////////////////////////////////////////
// HVector.cpp - Math for long vectors
//
// Copyright David K. McAllister, Mar. 1998.

#include "Math/HVector.h"

#include "Math/Random.h"

#include <string>
#include <vector>

template <class Elem_T> std::string HVector<Elem_T>::print() const
{
    std::string s;

    for (size_t i = 0; i < size(); i++) { s += std::to_string(V[i]) + " "; }

    return s;
}
template std::string HVector<float>::print() const;

template <class Elem_T> std::ostream& operator<<(std::ostream& os, const HVector<Elem_T>& p)
{
    os << p.print();
    return os;
}
template std::ostream& operator<<(std::ostream& os, const HVector<float>& p);

template <class Elem_T> HVector<Elem_T>& HVector<Elem_T>::rand()
{
    for (size_t i = 0; i < size(); i++) (*this)[i] = tRand<Elem_T>();

    return *this;
}
template HVector<float>& HVector<float>::rand();

template <class Elem_T> HVector<Elem_T>& HVector<Elem_T>::nrand(const Elem_T sigma)
{
    for (size_t i = 0; i < size(); i++) (*this)[i] = ndrand(0, sigma);

    return *this;
}

// Takes an array of HVectors.
template <class Elem_T> HVector<Elem_T> Mean(const HVector<Elem_T>* VA, const size_t count)
{
    ASSERT_R(count > 0);

    size_t dim = VA[0].size();

    HVector<Elem_T> Mu(dim);
    Mu.zero();

    for (size_t i = 0; i < count; i++) {
        ASSERT_D(VA[i].size() == dim);

        Mu += VA[i];
    }

    Mu /= count;

    return Mu;
}

template <class Elem_T> HMatrix<Elem_T> OuterProd(const HVector<Elem_T>& A, const HVector<Elem_T>& B)
{
    ASSERT_R(A.size() == B.size());

    HMatrix C(A.size(), A.size());

    for (size_t i = 0; i < A.size(); i++)
        for (size_t j = 0; j < A.size(); j++) { C(i, j) = A[i] * B[j]; }

    return C;
}

// Compute the covariance matrix of this array of vectors.
template <class Elem_T> HMatrix<Elem_T> Covariance(const HVector<Elem_T>* VA, const size_t count)
{
    ASSERT_R(count > 0);

    size_t dim = VA[0].size();
    HMatrix Cov(dim, dim);
    Cov.zero();

    HVector<Elem_T> Mu = Mean(VA, count);
    HVector<Elem_T>* VL = new HVector<Elem_T>[count];
    ASSERT_RM(VL, "memory alloc failed");

    for (size_t i = 0; i < count; i++) {
        ASSERT_D(VA[i].size() == dim);
        VL[i] = VA[i] - Mu;
    }

    for (size_t i = 0; i < dim; i++) {
        for (size_t j = 0; j < dim; j++) {
            Elem_T t = 0;
            for (size_t k = 0; k < count; k++) t += VL[k][i] * VL[k][j];

            Cov(i, j) += t;
        }
    }

    Cov /= Elem_T(count);

    delete[] VL;

    return Cov;
}
