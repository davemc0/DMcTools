/////////////////////////////////////////////////////////////
// HMatrix.h - Math for variable-sized matrices.
//
// Copyright David K. McAllister, Mar. 1998.

#ifndef dmc_hmatrix_h
#define dmc_hmatrix_h

#include "Util/Assert.h"

#include <iostream>

class HVector;

class Matrix
{
    int r, c;
    double *data;

public:
    Matrix()
    {
        r = c = 0;
        data = NULL;
    }

    Matrix(const int c, const int r) : r(r), c(c)
    {
        if(r * c > 0) {
            data = new double[r * c];
            ASSERT_RM(data, "memory alloc failed");

        } else
            data = NULL;
    }

    Matrix(const Matrix &A)
    {
        if(A.data) {
            data = new double[A.c * A.r];
            ASSERT_RM(data, "memory alloc failed");
            c = A.c;
            r = A.r;
            int cnt = c*r;
            for(int i=0; i<cnt; i++)
                data[i] = A.data[i];
        } else {
            data = NULL;
            c = r = 0;
        }
    }

    ~Matrix()
    {
        if(data)
            delete [] data;
    }

    Matrix operator+(const Matrix &A) const
    {
        ASSERT_R((data != NULL));
        ASSERT_R(A.c == c && A.r == r);

        Matrix C(c, r);

        int cnt = c*r;

        for(int i=0; i<cnt; i++)
            C.data[i] = data[i] + A.data[i];

        return C;
    }

    Matrix & operator+=(const Matrix &A)
    {
        ASSERT_R(data != NULL);
        ASSERT_R(A.c == c && A.r == r);

        int cnt = c*r;
        for(int i=0; i<cnt; i++)
            data[i] += A.data[i];

        return *this;
    }

    Matrix & operator/=(const double d)
    {
        ASSERT_R(data != NULL);
        ASSERT_R(c > 0 && r > 0);

        double dinv = 1. / d;
        int cnt = c*r;
        for(int i=0; i<cnt; i++)
            data[i] *= dinv;

        return *this;
    }

    double & operator()(const int x, const int y)
    {
        ASSERT_D(data != NULL);
        ASSERT_D(x >= 0 && x < c);
        ASSERT_D(y >= 0 && y < r);

        return data[y*c+x];
    }

    Matrix & zero()
    {
        ASSERT_R(data != NULL);
        ASSERT_R(c > 0 && r > 0);

        int cnt = c*r;
        for(int i=0; i<cnt; i++)
            data[i] = 0;
        
        return *this;
    }

    std::string print() const
    {
        ASSERT_D(data != NULL);

        std::string s;
        char ii[15];

        for(int i=0; i<r; i++) {
            for(int j=0; j<c; j++) {
                s += gcvt(data[i*c+j], 15, ii);
                s += " ";
            }
            s += "\n";
        }

        return s;
    }
};

// Defined in HVector.cpp
std::ostream& operator<<(std::ostream& os, const Matrix& m);

#endif
