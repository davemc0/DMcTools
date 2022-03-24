//////////////////////////////////////////////////////////////////////
// BinaryRep.cpp - Functions that deal with binary representation of floats and ints
//
// Changes copyright David K. McAllister, Feb. 2020.

#include "Math/BinaryRep.h"

#ifdef _WIN32
#include <intrin.h>
#endif

uint32_t clz(uint32_t v)
{
#if defined(_WIN32)
    static bool hasLzcnt = false, checkedLzcnt = false;

    if (!checkedLzcnt) {
        int cpuInfo[4];
        __cpuid(cpuInfo, 0x80000001);
        if (cpuInfo[2] & 0x20) hasLzcnt = true;
        checkedLzcnt = true;
    }

    if (hasLzcnt)
        return __lzcnt(v);
    else {
        // Since __lzcnt intrinsic is not supported on older Intel machines we use this SW version if needed.
        // Based on https://stackoverflow.com/questions/23856596/how-to-count-leading-zeros-in-a-32-bit-unsigned-integer/23862121#23862121
        if (!v) return 32;
        static const char debruijn32[32] = {0, 31, 9, 30, 3, 8,  13, 29, 2,  5,  7,  21, 12, 24, 28, 19,
                                            1, 10, 4, 14, 6, 22, 25, 20, 11, 15, 23, 26, 16, 27, 17, 18};
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        v++;
        return debruijn32[v * 0x076be629 >> 27];
    }
#elif defined(__GNUC__)
    if (!v) return 32;

    return __builtin_clz(v);
#else
#error Not yet implemented
#endif
}
