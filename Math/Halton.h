//////////////////////////////////////////////////////////////////////
// Halton.h - The Halton and Hammersley sequences
//
// Copyright David K. McAllister, 2008.

#pragma once

#include <vector>

// The Hammersley Sequence has low discrepancy.
namespace {
    int ithprime[10] = {2,3,5,7,11,13,17,19,23,29};
};

DMC_DECL std::vector<float> Hammersley(int i, int dim, int Count)
{
    ASSERT_D(dim<=10);
    std::vector<float> T(dim);
    for(int k=0; k<dim; k++) T[k] = 0;
    T[0] = i/float(Count);
    for(int k=1; k<dim; k++) {
        int R = ithprime[k-1];
        for(int j=0; j<i; j++) {
            float x = 1 - T[k];
            float y = 1 / float(R);
            while (x<=y)
                y = y / R;
            T[k] += (R+1)*y - 1;
        }
    }

    return T;
}

// The Halton Sequence has low discrepancy and is cumulative.
DMC_DECL std::vector<float> Halton(int i, int dim)
{
    ASSERT_D(dim<=10);
    std::vector<float> T(dim);
    for(int k=0; k<dim; k++) T[k] = 0;
    for(int k=0; k<dim; k++) {
        int R = ithprime[k];
        for(int j=0; j<i; j++) {
            float x = 1 - T[k];
            float y = 1 / float(R);
            while (x<=y)
                y = y / R;
            T[k] += (R+1)*y - 1;
        }
    }

    return T;
}

class SampleTable_t
{
public:
    static const int TABLE_SIZE = 128;
    float tab[TABLE_SIZE][2];
    float scalexx, scaleyy;

public:
    SampleTable_t()
    {
        scalexx = -1;
        scaleyy = -1;
    }

    // Fill in the sample table with the Halton sequence, scaled to -scale/2 to scale/2.
    void ScaleHalton(const float scalex, const float scaley)
    {
        if(scalex == scalexx && scaley == scaleyy) return;

        scalexx = scalex;
        scaleyy = scaley;
        float scale2x = scalex * 0.5f;
        float scale2y = scaley * 0.5f;

        for(int i=0; i<TABLE_SIZE; i++) {
            std::vector<float> H = Halton(i, 2);
            tab[i][0] = H[0]*scalex - scale2x;
            tab[i][1] = H[1]*scaley - scale2y;
            // Now tab[i] is centered at 0,0.
            // The render loop will re-bias to 0.5,0.5.
        }
    }

    // This is a thread-safe get since index is passed in.
    DMC_DECL void Get(float &vx, float &vy, const int index)
    {
        ASSERT_D(index < TABLE_SIZE);
        vx = tab[index][0];
        vy = tab[index][1];
    }
};
