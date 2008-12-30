//////////////////////////////////////////////////////////////////////
// HVector.h - Math for long vectors.
//
// Copyright David K. McAllister, Mar. 1998.

#ifndef dmc_hvector_h
#define dmc_hvector_h

#include "Util/Assert.h"
#include "Math/HMatrix.h"
#include "Math/MiscMath.h"

#include <string>

class HVector
{
    double *data;
    int r;
public:

    DMC_INLINE HVector()
    {
        data = NULL;
        r = 0;
    }

    DMC_INLINE HVector(const int r) : r(r)
    {
        if(r > 0) {
            data = new double[r];
            ASSERT_RM(data, "memory alloc failed");
        } else
            data = NULL;
    }

    DMC_INLINE HVector(const HVector &A)
    {
        if(A.data)
        {
            data = new double[A.r];
            ASSERT_RM(data, "memory alloc failed");
            memcpy(data, A.data, A.r * sizeof(double));
            r = A.r;
        }
        else
        {
            data = NULL;
            r = 0;
        }
    }

    virtual DMC_INLINE ~HVector()
    {
        if(data)
            delete [] data;
    }

    DMC_INLINE double & operator[](const int i)
    {
        ASSERT_D(data != NULL);

        return data[i];
    }

    DMC_INLINE const double & operator[](const int i) const
    {
        ASSERT_D(data != NULL);

        return data[i];
    }

    DMC_INLINE HVector & operator=(const HVector &A)
    {
        if(A.data && A.r)
        {
            if(A.r > r)
            {
                delete [] data;
                data = new double[A.r];
                ASSERT_RM(data, "memory alloc failed");
            }
            memcpy(data, A.data, A.r * sizeof(double));
            r = A.r;
        }
        else
        {
            data = NULL;
            r = 0;
        }

        return *this;
    }

    // Operators on doubles.

    DMC_INLINE HVector operator*(const double d) const
    {
        ASSERT_R(data != NULL);

        HVector b(r);

        for(int i=0; i<r; i++)
            b[i] = data[i] * d;

        return b;
    }

    DMC_INLINE HVector & operator*=(const double d)
    {
        ASSERT_R(data != NULL);

        for(int i=0; i<r; i++)
            data[i] = data[i] * d;

        return *this;
    }

    DMC_INLINE HVector & operator/=(const double d)
    {
        ASSERT_R(data != NULL);

        for(int i=0; i<r; i++)
            data[i] = data[i] / d;

        return *this;
    }

    DMC_INLINE HVector & operator+=(const double d)
    {
        ASSERT_R(data != NULL);

        for(int i=0; i<r; i++)
            data[i] = data[i] + d;

        return *this;
    }

    DMC_INLINE HVector & operator-=(const double d)
    {
        ASSERT_R(data != NULL);

        for(int i=0; i<r; i++)
            data[i] = data[i] - d;

        return *this;
    }

    // Add a constant to each element.
    DMC_INLINE HVector operator+(const double d) const
    {
        ASSERT_R(data != NULL);

        HVector b(r);

        for(int i=0; i<r; i++)
            b[i] = data[i] + d;

        return b;
    }

    // Operators on HVectors.

    DMC_INLINE HVector operator+(const HVector &A) const
    {
        ASSERT_R(data != NULL);
        ASSERT_R(A.data != NULL);
        ASSERT_R(A.r == r);

        HVector b(r);

        for(int i=0; i<r; i++)
            b[i] = data[i] + A.data[i];

        return b;
    }

    DMC_INLINE HVector & operator+=(const HVector &A)
    {
        ASSERT_R(data != NULL);
        ASSERT_R(A.data != NULL);
        ASSERT_R(A.r == r);

        for(int i=0; i<r; i++)
            data[i] += A.data[i];

        return *this;
    }

    DMC_INLINE HVector operator-(const HVector &A) const
    {
        ASSERT_R(data != NULL);
        ASSERT_R(A.data != NULL);
        ASSERT_R(A.r == r);

        HVector b(r);

        for(int i=0; i<r; i++)
            b[i] = data[i] - A.data[i];

        return b;
    }

    friend DMC_INLINE double Dot(const HVector &A, const HVector &B)
    {
        ASSERT_R(A.data != NULL);
        ASSERT_R(B.data != NULL);
        ASSERT_R(A.r == B.r);

        double dot;
        for(int i=0; i<A.r; i++)
            dot += A.data[i] * B.data[i];

        return dot;
    }

    // Swap the data in these two vectors.
    DMC_INLINE void swap(const HVector &A)
    {
        ASSERT_R(data != NULL);
        ASSERT_R(A.data != NULL);
        ASSERT_R(A.r == r);

        double tmp;
        for(int i=0; i<r; i++)
        {
            tmp = data[i]; data[i] = A.data[i]; A.data[i] = tmp;
        }
    }

    // Set it to a new size.
    // Warning: Doesn't preserve data on growth.
    DMC_INLINE void size(int sz)
    {
        ASSERT_D(sz >= 0);

        if(sz <= r && data)
        {
            // We're shrinking it but not deleting it.
            r = sz;
            return;
        }

        if(data)
            delete [] data;

        r = sz;
        if(r > 0)
        {
            data = new double[r];
            ASSERT_RM(data, "memory alloc failed");
        }
        else
            data = NULL;
    }

    DMC_INLINE int size() const
    {
        return r;
    }

    DMC_INLINE HVector & zero()
    {
        ASSERT_R(data != NULL);
        ASSERT_R(r > 0);

        memset(data, 0, r * sizeof(double));

        return *this;
    }

    // Set all elements to the given value.
    DMC_INLINE HVector & set(double val)
    {
        ASSERT_R(data != NULL);
        ASSERT_R(r > 0);

        for(int i=0; i<r; i++)
            data[r] = val;

        return *this;
    }

    //Return a string with this vector printed in it.
    std::string print() const;

    // Fill the vector with rands on 0.0 -> 1.0.
    HVector & rand();

    // Fill the vector with a normal distribution of random numbers,
    // with the given standard deviation.
    HVector & nrand(const double sigma = 1.0);

    DMC_INLINE double length2() const
    {
        ASSERT_R(data != NULL && r > 0);

        double l2 = 0;
        for(int i=0; i<r; i++)
        {
            l2 += Sqr(data[i]);
        }

        return l2;
    }

    DMC_INLINE double length() const
    {
        return Sqrt(length2());
    }

    friend Matrix OuterProd(const HVector &A, const HVector &B);

    // Takes an array of HVectors, all the same size.
    friend Matrix Covariance(const HVector *VA, const int count);

    friend HVector Mean(const HVector *VA, const int count);
};

std::ostream& operator<<(std::ostream& os, const HVector& p);
std::istream& operator>>(std::istream& os, HVector& p);

#endif
