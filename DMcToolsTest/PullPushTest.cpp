#include "Image/tImage.h"
#include "Image/PullPush.h"
#include "Util/Utils.h"

#include <iostream>
using namespace std;

#define LOOP 10000

bool PullPushTest(int argc, char **argv)
{
#if 0
    ucImage Bob(argv[1], 1);
    cerr << Bob.chan << endl;

    float *Im = new float[Bob.size];
    float *Wg = new float[Bob.size];

    memset(Im, 0, Bob.size * sizeof(float));
    memset(Wg, 0, Bob.size * sizeof(float));

    int i;
    for(i=0; i<LOOP; i++)
    {
        int j = LRand() % Bob.size;
        Im[j] = float(Bob.Pix[j]);
        Wg[j] = 1.0f;  float *Im = new float[Bob.size];

    }

    ucImage Joe(Bob.wid, Bob.hgt, 1);

    for(i=0; i<Bob.size; i++)
    {
        Joe.Pix[i] = (unsigned char)Im[i];
    }

    Joe.Save("pre.tif");

    PullPush(Im, Wg, Bob.wid, Bob.hgt);

    for(i=0; i<Bob.size; i++)
    {
        Joe.Pix[i] = (unsigned char)Im[i];
    }

    Joe.Save("out.tif");
#endif

    return true;
}
