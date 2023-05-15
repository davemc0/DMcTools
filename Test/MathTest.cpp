#include "Math/BinaryRep.h"
#include "Math/Random.h"
#include "Math/Vector.h"

#include <iostream>

void vectorTest()
{
    {
        f3vec a(1, 0, 0), b(0, 1, 0);
        f3vec c = cross(a, b);
        std::cerr << a << b << c << '\n';
    }
    {
        d3vec a(1.f, (double)0, (int)0), b(0, 1, 0);
        d3vec c = cross(a, b);
        std::cerr << a << b << c << '\n';
    }
}

void deconstructFloatTest()
{
    int tries = 0, fails = 0;
    while (1) {
        float frnd = abs(nfrand(0, 1));

        uint32_t fsign, fexp, fmant;
        deconstructFloat(frnd, fsign, fexp, fmant);

        int expVal = fexp + irand(10);
        int nBits = irand(8, 25);

        uint32_t fixed = floatToFixedGivenExp(frnd, expVal, nBits);

        float fout = fixedToFloatGivenExp(fixed, expVal, nBits);

        tries++;
        if (floatAsUint(frnd) >> 16 != floatAsUint(fout) >> 16) {
            fails++;
            printf("I %9.9g %d %d %006x ", frnd, fsign, fexp, fmant);
            printf("fixed=%06x nbits=%d expVal=%d\n", fixed, nBits, expVal);
            deconstructFloat(fout, fsign, fexp, fmant);
            printf("O %9.9g %d %d %006x %d/%d\n", fout, fsign, fexp, fmant, fails, tries);
        }
    }
}

void continuousFloatTest()
{
    int tries = 0, fails = 0;
    while (1) {
        float frnd = abs(nfrand(0, 1));

        uint32_t fcont = floatToContinuous(frnd);
        float fout = continuousToFloat(fcont);

        tries++;
        if (frnd != fout) {
            fails++;
            printf("I %9.9g %d\n", frnd, fcont);
            printf("O %9.9g %d %d,%d\n", fout, fcont, fails, tries);
        }
    }
}

void continuousFloatTest2()
{
    int tries = 0, fails = 0;
    for (float frnd = -1e38f; frnd < 1e5f; frnd *= 0.5f) {
        uint32_t fcont = floatToContinuous(frnd);
        float fout = continuousToFloat(fcont);

        tries++;
        if (true || frnd != fout) {
            fails++;
            printf("I %9.9g 0x%08x\n", frnd, fcont);
            printf("O %9.9g 0x%08x %d,%d\n", fout, fcont, fails, tries);
        }
    }
}

bool MathTest(int argc, char** argv)
{
    std::cerr << "Starting MathTest\n";

    vectorTest();
    continuousFloatTest2();
    // deconstructFloatTest();

    std::cerr << "Ending MathTest\n";

    return true;
}
