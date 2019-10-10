/////////////////////////////////////////////////////////////
// HMatrix.h - Math for variable-sized matrices
//
// Copyright David K. McAllister, Mar. 1998.

#pragma once

#include "Util/Assert.h"

template<class Elem_T>
class HMatrix
{
    size_t r, c;
    Elem_T *data;

public:
    HMatrix()
    {
        r = c = 0;
        data = NULL;
    }

    HMatrix(const size_t c, const size_t r) : r(r), c(c)
    {
        if(c*r > 0) {
            data = new Elem_T[c*r];
            ASSERT_RM(data, "memory alloc failed");

        } else
            data = NULL;
    }

    HMatrix(const HMatrix &A)
    {
        if(A.data) {
            data = new Elem_T[A.c * A.r];
            ASSERT_RM(data, "memory alloc failed");
            c = A.c;
            r = A.r;

            for(size_t i=0; i<c*r; i++)
                data[i] = A.data[i];
        } else {
            data = NULL;
            c = r = 0;
        }
    }

    ~HMatrix()
    {
        if(data)
            delete [] data;
    }

    HMatrix operator+(const HMatrix &A) const
    {
        ASSERT_R((data != NULL));
        ASSERT_R(A.c == c && A.r == r);

        HMatrix C(c, r);

        for(size_t i=0; i<c*r; i++)
            C.data[i] = data[i] + A.data[i];

        return C;
    }

    HMatrix & operator+=(const HMatrix &A)
    {
        ASSERT_R(data != NULL);
        ASSERT_R(A.c == c && A.r == r);

        for(size_t i=0; i<c*r; i++)
            data[i] += A.data[i];

        return *this;
    }

    HMatrix & operator/=(const Elem_T d)
    {
        ASSERT_R(data != NULL);
        ASSERT_R(c > 0 && r > 0);

        Elem_T dinv = 1. / d;
        for(size_t i=0; i<c*r; i++)
            data[i] *= dinv;

        return *this;
    }

    Elem_T & operator()(const size_t x, const size_t y)
    {
        ASSERT_D(data != NULL);
        ASSERT_D(x >= 0 && x < c);
        ASSERT_D(y >= 0 && y < r);

        return data[y*c+x];
    }

    HMatrix & zero()
    {
        ASSERT_R(data != NULL);
        ASSERT_R(c > 0 && r > 0);

        for(size_t i=0; i<c*r; i++)
            data[i] = 0;
        
        return *this;
    }

    std::string print() const
    {
        ASSERT_D(data != NULL);

        std::string s;
        char ii[32];

        for(size_t i=0; i<r; i++) {
            for(size_t j=0; j<c; j++) {
                s += gcvt(data[i*c+j], 8, ii);
                s += " ";
            }
            s += "\n";
        }

        return s;
    }
};

template<class Elem_T>
std::ostream& operator<<(std::ostream& os, const HMatrix<Elem_T>& m)
{
    os << m.print();
    return os;
}
