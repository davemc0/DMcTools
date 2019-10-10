//////////////////////////////////////////////////////////////////////
// Filter.cpp - Stuff to filter images
//
// This stuff is mostly deprecated by ImageAlgorithms.h.
//
// Copyright David K. McAllister, 1998.

#if 0

#include "Util/Assert.h"
#include "Math/MiscMath.h"

// 3x3 blur with special kernel for a uc1Image.
void ucImage::FastBlur1Chan()
{
    ASSERT_R(chan == 1);
    // cerr << "FastBlur1\n";
    int y;

    // Allocates space for image.
    unsigned char *P2 = new unsigned char[dsize];
    ASSERT_RM(P2, "memory alloc failed");

    // Do corners.
    {
        unsigned short C = Pix[0] << 2;
        C += Pix[1] << 2;
        C += Pix[wid] << 2;
        C += Pix[wid+1] << 1;
        C += Pix[wid+1];
        P2[0] = (unsigned char)((C + 16) / 15);

        C = Pix[wid-1] << 2;
        C += Pix[wid-2] << 2;
        C += Pix[wid+wid-1] << 2;
        C += Pix[wid+wid-2] << 1;
        C += Pix[wid+wid-2];
        P2[wid-1] = (unsigned char)((C + 16) / 15);

        int ib=(hgt-1)*wid;
        C = Pix[ib] << 2;
        C += Pix[ib+1] << 2;
        C += Pix[ib-wid] << 2;
        C += Pix[ib-wid+1] << 1;
        C += Pix[ib-wid+1];
        P2[ib] = (unsigned char)((C + 16) / 15);

        C = Pix[ib+wid-1] << 2;
        C += Pix[ib+wid-2] << 2;
        C += Pix[ib-1] << 2;
        C += Pix[ib-2] << 1;
        C += Pix[ib-2];
        P2[ib+wid-1] = (unsigned char)((C + 16) / 15);
    }

    // Do top and bottom edges.
    int it=1, ib=(hgt-1)*wid+1;
    for(; it<wid-1; ib++, it++)
    {
        // Top
        unsigned short C = Pix[it] << 2;
        C += Pix[it+1] << 2;
        C += Pix[it-1] << 2;
        C += Pix[it+wid] << 2;
        C += Pix[it+wid+1];
        C += Pix[it+wid+1] << 1;
        C += Pix[it+wid-1];
        C += Pix[it+wid-1] << 1;
        P2[it] = (unsigned char)((C + 16) / 22);

        // Bottom
        C = Pix[ib] << 2;
        C += Pix[ib+1] << 2;
        C += Pix[ib-1] << 2;
        C += Pix[ib-wid] << 2;
        C += Pix[ib-wid+1];
        C += Pix[ib-wid+1] << 1;
        C += Pix[ib-wid-1];
        C += Pix[ib-wid-1] << 1;
        P2[ib] = (unsigned char)((C + 16) / 22);
        //P2[ib] = 255;
    }

    // Do left and right edges
    for(y=1; y<hgt-1; y++)
    {
        int il = y*wid, ir = y*wid+wid-1;

        // Left side
        unsigned short C = Pix[il] << 2;
        C += Pix[il+1] << 2;
        C += Pix[il+wid] << 2;
        C += Pix[il-wid] << 2;
        C += Pix[il+wid+1];
        C += Pix[il+wid+1] << 1;
        C += Pix[il-wid+1];
        C += Pix[il-wid+1] << 1;
        P2[il] = (unsigned char)((C + 16) / 22);

        // Right side
        C = Pix[ir] << 2;
        C += Pix[ir-1] << 2;
        C += Pix[ir+wid] << 2;
        C += Pix[ir-wid] << 2;
        C += Pix[ir+wid-1];
        C += Pix[ir+wid-1] << 1;
        C += Pix[ir-wid-1];
        C += Pix[ir-wid-1] << 1;
        P2[ir] = (unsigned char)((C + 16) / 22);
        //P2[ir] = 255;
    }

    for(y=1; y<hgt-1; y++) {
        int ind = y*wid+1;
        for(int x=1; x<wid-1; x++, ind++) {
            // Sum of weights: 3 4 3  4 4 4  3 4 3 = 32
            unsigned short C = Pix[ind] << 2;
            C += Pix[ind+1] << 2;
            C += Pix[ind-1] << 2;
            C += Pix[ind+wid] << 2;
            C += Pix[ind-wid] << 2;
            C += Pix[ind+wid+1];
            C += Pix[ind+wid+1] << 1;
            C += Pix[ind+wid-1];
            C += Pix[ind+wid-1] << 1;
            C += Pix[ind-wid+1];
            C += Pix[ind-wid+1] << 1;
            C += Pix[ind-wid-1];
            C += Pix[ind-wid-1] << 1;
            P2[ind] = (unsigned char)((C + 16) >> 5);
        }
    }

    // Hook the new image into this ucImage.
    delete[] Pix;
    Pix = P2;
}

#endif
