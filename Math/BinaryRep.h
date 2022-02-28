//////////////////////////////////////////////////////////////////////
// BinaryRep.h - Functions that deal with binary representation of floats and ints
//
// Copyright David K. McAllister, Feb. 2020.

#pragma once

#include "Util/toolconfig.h"

#include <cmath>
#include <stdlib.h>

#ifdef _WIN32
#include <intrin.h>
#endif

uint32_t clz(uint32_t b);

DMC_DECL uint32_t ctz(uint32_t b)
{
#if defined(__GNUC__)
    if (!b) return 32;
    return __builtin_ctz(b);
#elif defined(_WIN32)
    if (!b) return 32;
    return 31 - clz((uint32_t)((int)b & -(int)b));
#else
#error
#endif
}

DMC_DECL uint64_t ctz(uint64_t b)
{
#if defined(__GNUC__)
    if (!b) return 64;
    return __builtin_clzll(b);
#elif defined(_WIN64)
    return __lzcnt64(b);
#else
#error
#endif
}

DMC_DECL uint32_t floatAsUint(float a)
{
    return *(uint32_t*)&a;
    union Conv {
        float f;
        uint32_t u;
    };
    Conv un;
    un.f = a;
    return un.u;
}

DMC_DECL float uintAsFloat(uint32_t u)
{
    union Conv {
        float f;
        uint32_t u;
    };
    Conv un;
    un.u = u;
    return un.f;
}

DMC_DECL uint32_t fixedToUint(const uint32_t fixed, const uint32_t nBits)
{
    int sfixed = fixed;
    int shBits = 32 - nBits;
    int v = sfixed << shBits >> shBits; // Sign extends sfixed

    return (uint32_t)v;
}

// Bit length of signed int v -> use fixedToUint to extend
DMC_DECL uint32_t signedIntBits(uint32_t v)
{
    uint32_t l0 = clz(v);
    uint32_t l1 = clz(~v);
    return 33 - (l1 < l0 ? l0 : l1);
}

// Bit length of unsigned int v -> can zero-extend
DMC_DECL uint32_t unsignedIntBits(uint32_t v) { return 32 - clz(v); }

DMC_DECL bool isDenorm(float f)
{
    uint32_t ui = floatAsUint(f);
    return ((ui & 0x7f800000) == 0) && f != 0.f;
}

DMC_DECL float constructFloat(uint32_t fsign, uint32_t fexp, uint32_t fmant) { return uintAsFloat((fsign << 31) | (fexp << 23) | fmant); }

DMC_DECL void deconstructFloat(float f, uint32_t& fsign, uint32_t& fexp, uint32_t& fmant)
{
    uint32_t fb = floatAsUint(f);
    fsign = fb >> 31;
    fexp = (fb >> 23) & 0xff;
    fmant = fb & 0x007fffff;
}

DMC_DECL uint32_t floatToBitsCode(float f)
{
    uint32_t bits = floatAsUint(f);
    if (bits & 0x80000000)
        return ~bits;
    else
        return bits | 0x80000000;
}

DMC_DECL float bfloatToFloat(const uint32_t bfbits, const uint32_t nBits) { return uintAsFloat(bfbits << (32 - nBits)); }

DMC_DECL uint32_t roundFloatToInt(const float f, const bool roundUp, const uint32_t nBits)
{
    uint32_t fbits = floatAsUint(f);
    uint32_t shifted = fbits >> (32 - nBits);
    float fout = bfloatToFloat(shifted, nBits);

    if ((roundUp && fout < f) || (!roundUp && fout > f)) shifted++;

    return shifted;
}

// Force a float to have given exponent, reducing precision as needed
DMC_DECL uint32_t floatToFixedGivenExp(const float f, const uint32_t expVal, const uint32_t nBits)
{
    if (f == 0.f) return 0;
    uint32_t fsign, fexp, fmant;
    deconstructFloat(f, fsign, fexp, fmant);

    uint32_t shBits = expVal - fexp;
    uint32_t fixed = (fmant | 0x800000) >> shBits; // Move the mantissa to make expVal be the exponent
    if (shBits > 31) fixed = 0;

    return fixed >> (24 - nBits);
}

DMC_DECL float fixedToFloatGivenExp(const uint32_t fixed, const uint32_t expVal, const uint32_t nBits)
{
    if (fixed == 0) return 0.f;

    uint32_t nonzeroBits = 32 - clz(fixed);
    uint32_t shBits = 24 - nonzeroBits;
    uint32_t mant = (fixed << shBits) & 0x7fffff;

    uint32_t fexp = shBits - (nBits - nonzeroBits);

    return constructFloat(0, fexp, mant);
}

DMC_DECL float fastExp2(int a) { return uintAsFloat((uint32_t)(std::min(std::max(a + 127, 1), 254) << 23)); }
DMC_DECL float fastMax(float a, float b) { return (a + b + abs(a - b)) * 0.5f; }
DMC_DECL float fastMin(float a, float b) { return (a + b - abs(a - b)) * 0.5f; }