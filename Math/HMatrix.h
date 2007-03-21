/////////////////////////////////////////////////////////////
// HMatrix.h - Math for variable-sized matrices.
//
// Copyright David K. McAllister, Mar. 1998.

#ifndef _hmatrix_h
#define _hmatrix_h

#include <Util/Assert.h>

#include <string>
#include <fstream>

#include <string.h>

class HVector;

class Matrix
{
    int r, c;
    double *data;

public:
    inline Matrix()
    {
        r = c = 0;
        data = NULL;
    }

    inline Matrix(const int _c, const int _r)
    {
        if(_r * _c > 0)
        {
            data = new double[_r * _c];
            ASSERT_RM(data, "memory alloc failed");

        }
        else
            data = NULL;

        r = _r;
        c = _c;
    }

    inline Matrix(const Matrix &A)
    {
        if(A.data)
        {
            data = new double[A.c * A.r];
            ASSERT_RM(data, "memory alloc failed");
            memcpy(data, A.data, A.c * A.r * sizeof(double));
            c = A.c;
            r = A.r;
        }
        else
        {
            data = NULL;
            c = r = 0;
        }
    }

    inline ~Matrix()
    {
        if(data)
            delete [] data;
    }

    inline Matrix operator+(const Matrix &A) const
    {
        ASSERT_R((data != NULL));
        ASSERT_R(A.c == c && A.r == r);

        Matrix C(c, r);

        int cnt = c*r;

        for(int i=0; i<cnt; i++)
        {
            C.data[i] = data[i] + A.data[i];
        }

        return C;
    }

    inline Matrix & operator+=(const Matrix &A)
    {
        ASSERT_R(data != NULL);
        ASSERT_R(A.c == c && A.r == r);

        int cnt = c*r;

        for(int i=0; i<cnt; i++)
        {
            data[i] += A.data[i];
        }

        return *this;
    }

    inline Matrix & operator/=(const double d)
    {
        ASSERT_R(data != NULL);
        ASSERT_R(c > 0 && r > 0);

        double dinv = 1. / d;
        int cnt = c*r;

        for(int i=0; i<cnt; i++)
        {
            data[i] *= dinv;
        }

        return *this;
    }

    inline double & operator()(const int x, const int y)
    {
        ASSERT_D(data != NULL);
        ASSERT_D(x >= 0 && x < c);
        ASSERT_D(y >= 0 && y < r);

        return data[y*c+x];
    }

    inline Matrix & zero()
    {
        ASSERT_R(data != NULL);
        ASSERT_R(c > 0 && r > 0);

        memset(data, 0, c * r * sizeof(double));

        return *this;
    }

    inline std::string print() const
    {
        ASSERT_D(data != NULL);

        std::string s;
        char ii[15];

        for(int i=0; i<r; i++)
        {
            for(int j=0; j<c; j++)
            {
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
