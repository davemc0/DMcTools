//////////////////////////////////////////////////////////////////////
// SpaceFillCurve.h - Space-filling curve conversions, esp. Morton and Hilbert
//
// Copyright David K. McAllister, 2012.

// TODO: Need the code to convert back from Morton to three ints.

#pragma once

#include "Math/AABB.h"

#include <vector>

typedef enum { CURVE_MORTON, CURVE_HILBERT, CURVE_RASTER, CURVE_BOUSTRO, CURVE_TILED2, CURVE_COUNT } SFCurveType;
char const* SFCurveNames[] = {"CURVE_MORTON", "CURVE_HILBERT", "CURVE_RASTER", "CURVE_BOUSTRO", "CURVE_TILED2"};

// Public, generic interfaces
// Statically choose the SFCurveType (performant)
template <typename intcode_t, SFCurveType CT> intcode_t toSFCurveCode(i3vec x) {}
template <typename intcode_t, SFCurveType CT> i3vec toSFCurveCoords(intcode_t x) {}

// Dynamically choose the SFCurveType
template <typename intcode_t> intcode_t toSFCurveCode(i3vec x, SFCurveType CT)
{
    switch (CT) {
    case CURVE_MORTON: return toSFCurveCode<intcode_t, CURVE_MORTON>(x);
    case CURVE_HILBERT: return toSFCurveCode<intcode_t, CURVE_HILBERT>(x);
    case CURVE_RASTER: return toSFCurveCode<intcode_t, CURVE_RASTER>(x);
    case CURVE_BOUSTRO: return toSFCurveCode<intcode_t, CURVE_BOUSTRO>(x);
    case CURVE_TILED2: return toSFCurveCode<intcode_t, CURVE_TILED2>(x);
    }
    return 0;
}
template <typename intcode_t> i3vec toSFCurveCoords(intcode_t x, SFCurveType CT)
{
    switch (CT) {
    case CURVE_MORTON: return toSFCurveCoords<intcode_t, CURVE_MORTON>(x);
    case CURVE_HILBERT: return toSFCurveCoords<intcode_t, CURVE_HILBERT>(x);
    case CURVE_RASTER: return toSFCurveCoords<intcode_t, CURVE_RASTER>(x);
    case CURVE_BOUSTRO: return toSFCurveCoords<intcode_t, CURVE_BOUSTRO>(x);
    case CURVE_TILED2: return toSFCurveCoords<intcode_t, CURVE_TILED2>(x);
    }
    return i3vec(0);
}

// The curve order (number of bits in each dimension)
template <typename intcode_t> const int curveOrder() { return sizeof(intcode_t) > sizeof(uint32_t) ? 20 : 10; }

// Returns the curve index
template <typename intcode_t> intcode_t computeMortonCodeF(i3vec v) {}

template <> uint32_t computeMortonCodeF(i3vec v)
{
    v.x = (v.x | (v.x << 16)) & 0x030000FF;
    v.x = (v.x | (v.x << 8)) & 0x0300F00F;
    v.x = (v.x | (v.x << 4)) & 0x030C30C3;
    v.x = (v.x | (v.x << 2)) & 0x09249249;

    v.y = (v.y | (v.y << 16)) & 0x030000FF;
    v.y = (v.y | (v.y << 8)) & 0x0300F00F;
    v.y = (v.y | (v.y << 4)) & 0x030C30C3;
    v.y = (v.y | (v.y << 2)) & 0x09249249;

    v.z = (v.z | (v.z << 16)) & 0x030000FF;
    v.z = (v.z | (v.z << 8)) & 0x0300F00F;
    v.z = (v.z | (v.z << 4)) & 0x030C30C3;
    v.z = (v.z | (v.z << 2)) & 0x09249249;

    return (v.z << 2) | (v.y << 1) | v.x;
}

// Returns the curve index
// x,y,z are the integer coordinates in 3D.
template <> uint64_t computeMortonCodeF(i3vec v)
{
    uint64_t x = v.x, y = v.y, z = v.z;

    x = (x | (x << 32)) & 0x000F00000000FFFF;
    x = (x | (x << 16)) & 0x000F0000FF0000FF;
    x = (x | (x << 8)) & 0x000F00F00F00F00F;
    x = (x | (x << 4)) & 0x00C30C30C30C30C3;
    x = (x | (x << 2)) & 0x0249249249249249;

    y = (y | (y << 32)) & 0x000F00000000FFFF;
    y = (y | (y << 16)) & 0x000F0000FF0000FF;
    y = (y | (y << 8)) & 0x000F00F00F00F00F;
    y = (y | (y << 4)) & 0x00C30C30C30C30C3;
    y = (y | (y << 2)) & 0x0249249249249249;

    z = (z | (z << 32)) & 0x000F00000000FFFF;
    z = (z | (z << 16)) & 0x000F0000FF0000FF;
    z = (z | (z << 8)) & 0x000F00F00F00F00F;
    z = (z | (z << 4)) & 0x00C30C30C30C30C3;
    z = (z | (z << 2)) & 0x0249249249249249;

    return (z << 2) | (y << 1) | x;
}

// Declarations of actual implementations
template <typename intcode_t> intcode_t toMortonCode(const i3vec v);
template <typename intcode_t> intcode_t toHilbertCode(const i3vec v);
template <typename intcode_t> intcode_t toRasterCode(const i3vec v);
template <typename intcode_t> intcode_t toBoustroCode(const i3vec v);
template <typename intcode_t> intcode_t toTiled2Code(const i3vec v);

template <typename intcode_t> i3vec toMortonCoords(const intcode_t p);
template <typename intcode_t> i3vec toHilbertCoords(const intcode_t p);
template <typename intcode_t> i3vec toRasterCoords(const intcode_t p);
template <typename intcode_t> i3vec toBoustroCoords(const intcode_t p);
template <typename intcode_t> i3vec toTiled2Coords(const intcode_t p);

// Convert from two template arguments to one
template <> uint32_t toSFCurveCode<uint32_t, CURVE_MORTON>(const i3vec v) { return toMortonCode<uint32_t>(v); }
template <> uint32_t toSFCurveCode<uint32_t, CURVE_HILBERT>(const i3vec v) { return toHilbertCode<uint32_t>(v); }
template <> uint32_t toSFCurveCode<uint32_t, CURVE_RASTER>(const i3vec v) { return toRasterCode<uint32_t>(v); }
template <> uint32_t toSFCurveCode<uint32_t, CURVE_BOUSTRO>(const i3vec v) { return toBoustroCode<uint32_t>(v); }
template <> uint32_t toSFCurveCode<uint32_t, CURVE_TILED2>(const i3vec v) { return toTiled2Code<uint32_t>(v); }

template <> uint64_t toSFCurveCode<uint64_t, CURVE_MORTON>(const i3vec v) { return toMortonCode<uint64_t>(v); }
template <> uint64_t toSFCurveCode<uint64_t, CURVE_HILBERT>(const i3vec v) { return toHilbertCode<uint64_t>(v); }
template <> uint64_t toSFCurveCode<uint64_t, CURVE_RASTER>(const i3vec v) { return toRasterCode<uint64_t>(v); }
template <> uint64_t toSFCurveCode<uint64_t, CURVE_BOUSTRO>(const i3vec v) { return toBoustroCode<uint64_t>(v); }
template <> uint64_t toSFCurveCode<uint64_t, CURVE_TILED2>(const i3vec v) { return toTiled2Code<uint64_t>(v); }

template <> i3vec toSFCurveCoords<uint32_t, CURVE_MORTON>(const uint32_t p) { return toMortonCoords<uint32_t>(p); }
template <> i3vec toSFCurveCoords<uint32_t, CURVE_HILBERT>(const uint32_t p) { return toHilbertCoords<uint32_t>(p); }
template <> i3vec toSFCurveCoords<uint32_t, CURVE_RASTER>(const uint32_t p) { return toRasterCoords<uint32_t>(p); }
template <> i3vec toSFCurveCoords<uint32_t, CURVE_BOUSTRO>(const uint32_t p) { return toBoustroCoords<uint32_t>(p); }
template <> i3vec toSFCurveCoords<uint32_t, CURVE_TILED2>(const uint32_t p) { return toTiled2Coords<uint32_t>(p); }

template <> i3vec toSFCurveCoords<uint64_t, CURVE_MORTON>(const uint64_t p) { return toMortonCoords<uint64_t>(p); }
template <> i3vec toSFCurveCoords<uint64_t, CURVE_HILBERT>(const uint64_t p) { return toHilbertCoords<uint64_t>(p); }
template <> i3vec toSFCurveCoords<uint64_t, CURVE_RASTER>(const uint64_t p) { return toRasterCoords<uint64_t>(p); }
template <> i3vec toSFCurveCoords<uint64_t, CURVE_BOUSTRO>(const uint64_t p) { return toBoustroCoords<uint64_t>(p); }
template <> i3vec toSFCurveCoords<uint64_t, CURVE_TILED2>(const uint64_t p) { return toTiled2Coords<uint64_t>(p); }

// Actual implementations
template <typename intcode_t> intcode_t toMortonCode(i3vec v)
{
    intcode_t x = v.x, y = v.y, z = v.z;
    intcode_t codex = 0, codey = 0, codez = 0;

    const int nbits2 = 2 * curveOrder<intcode_t>();

    for (int i = 0, andbit = 1; i < nbits2; i += 2, andbit <<= 1) {
        codex |= (intcode_t)(x & andbit) << i;
        codey |= (intcode_t)(y & andbit) << i;
        codez |= (intcode_t)(z & andbit) << i;
    }

    return (codez << 2) | (codey << 1) | codex;
}

// Hilbert coordinate transpose functions by John Skilling. Public domain.
// See https://stackoverflow.com/questions/499166/mapping-n-dimensional-value-to-a-point-on-hilbert-curve
// The uint32_t is the N-D coordinate type. Could be something else.

void transposeFromHilbertCoords(uint32_t* X, int nbits, int dim)
{
    uint32_t N = 2 << (nbits - 1), P, Q, t;

    // Gray decode by H ^ (H/2)
    t = X[dim - 1] >> 1;
    // Corrected error in Skilling's paper on the following line. The appendix had i >= 0 leading to negative array index.
    for (int i = dim - 1; i > 0; i--) X[i] ^= X[i - 1];
    X[0] ^= t;

    // Undo excess work
    for (Q = 2; Q != N; Q <<= 1) {
        P = Q - 1;
        for (int i = dim - 1; i >= 0; i--)
            if (X[i] & Q) // Invert
                X[0] ^= P;
            else { // Exchange
                t = (X[0] ^ X[i]) & P;
                X[0] ^= t;
                X[i] ^= t;
            }
    }
}

void transposeToHilbertCoords(uint32_t* X, int nbits, int dim)
{
    uint32_t M = 1 << (nbits - 1), P, Q, t;

    // Inverse undo
    for (Q = M; Q > 1; Q >>= 1) {
        P = Q - 1;
        for (int i = 0; i < dim; i++)
            if (X[i] & Q) // Invert
                X[0] ^= P;
            else { // Exchange
                t = (X[0] ^ X[i]) & P;
                X[0] ^= t;
                X[i] ^= t;
            }
    }

    // Gray encode
    for (int i = 1; i < dim; i++) X[i] ^= X[i - 1];
    t = 0;
    for (Q = M; Q > 1; Q >>= 1)
        if (X[dim - 1] & Q) t ^= Q - 1;
    for (int i = 0; i < dim; i++) X[i] ^= t;
}

template <typename intcode_t> intcode_t toHilbertCode(i3vec v)
{
    const int nbits = curveOrder<intcode_t>();

    transposeToHilbertCoords((uint32_t*)v.data(), nbits, 3);
    intcode_t s = toMortonCode<intcode_t>(v);

    return s;
}

template <typename intcode_t> intcode_t toRasterCode(i3vec v)
{
    const int nbits = curveOrder<intcode_t>();
    intcode_t s = ((intcode_t)v.z << (intcode_t)(2 * nbits)) | ((intcode_t)v.y << (intcode_t)nbits) | (intcode_t)v.x;

    return s;
}
template <typename intcode_t> intcode_t toBoustroCode(i3vec v) { return 0; }
template <typename intcode_t> intcode_t toTiled2Code(i3vec v) { return 0; }

template <typename intcode_t> i3vec toMortonCoords(intcode_t p)
{
    // From https://github.com/Forceflow/libmorton/blob/main/include/libmorton/morton3D.h
    i3vec v(0);
    unsigned int checkbits = static_cast<unsigned int>(floor((sizeof(intcode_t) * 8.0f / 3.0f)));
    for (unsigned int i = 0; i <= checkbits; ++i) {
        intcode_t selector = 1;
        unsigned int shift_selector = 3 * i;
        unsigned int shiftback = 2 * i;
        v.x |= (p & (selector << shift_selector)) >> (shiftback);
        v.y |= (p & (selector << (shift_selector + 1))) >> (shiftback + 1);
        v.z |= (p & (selector << (shift_selector + 2))) >> (shiftback + 2);
    }
    return v;
}

template <typename intcode_t> i3vec toHilbertCoords(intcode_t p)
{
    const int nbits = curveOrder<intcode_t>();

    i3vec v = toMortonCoords<intcode_t>(p);
    transposeFromHilbertCoords((uint32_t*)v.data(), nbits, 3);

    return v;
}

template <typename intcode_t> i3vec toRasterCoords(intcode_t p)
{
    const int nbits = curveOrder<intcode_t>();
    const intcode_t nbitmask = ((intcode_t)1 << (intcode_t)nbits) - (intcode_t)1;

    i3vec v((int)(p & nbitmask), (int)((p >> (intcode_t)nbits) & nbitmask), (int)((p >> (intcode_t)(2 * nbits)) & nbitmask));

    return v;
}

template <typename intcode_t> i3vec toBoustroCoords(intcode_t p) { return i3vec(0); }
template <typename intcode_t> i3vec toTiled2Coords(intcode_t p) { return i3vec(0); }

// Convert a 3D float vector to 3 integer code for later conversion to Morton
template <typename Vec_T> class FixedPointifier {
public:
    FixedPointifier(const tAABB<Vec_T>& world, const uint32_t nbits_) : m_nbits(nbits_)
    {
        const float keyMaxf = (float)((1 << m_nbits) - 1);

        m_scale = keyMaxf / world.extent();
        m_scaleInv = world.extent() / keyMaxf;
        m_bias = world.lo();
    }

    i3vec floatToFixed(const Vec_T& P)
    {
        const int keyMaxi = (1 << m_nbits) - 1;

        Vec_T v = m_scale * (P - m_bias);
        i3vec key = v;
        key = clamp(key, i3vec(0), i3vec(keyMaxi)); // Key.clamp(i3vec(0), i3vec(keyMaxi)) is const and doesn't do what we want.

        return key;
    }

    Vec_T fixedToFloat(const i3vec& P)
    {
        const float keyMaxf = (float)((1 << m_nbits) - 1);
        Vec_T v = m_bias + f3vec(P) * m_scaleInv;

        return v;
    }

private:
    const uint32_t m_nbits;
    Vec_T m_scale, m_scaleInv;
    Vec_T m_bias;
};
