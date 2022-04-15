#include "Math/BinaryRep.h"
#include "Math/Random.h"

#include <iostream>

bool MathTest(int argc, char** argv)
{
    std::cerr << "Starting MathTest\n";

    int tries = 0, fails = 0;
    while (1) {
        float frnd = abs(NRandf(0, 1));

        uint32_t fsign, fexp, fmant;
        deconstructFloat(frnd, fsign, fexp, fmant);

        int expVal = fexp + LRand(10);
        int nBits = LRand(8, 25);

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

    std::cerr << "Ending MathTest\n";

    return true;
}
