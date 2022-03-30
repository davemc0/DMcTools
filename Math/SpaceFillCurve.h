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
template <typename intcode_t, SFCurveType CT> intcode_t toSFCurveCode(const i3vec& x) {}
template <typename intcode_t, SFCurveType CT> i3vec toSFCurveCoords(const intcode_t& x) {}

// Dynamically choose the SFCurveType
template <typename intcode_t> intcode_t toSFCurveCode(const i3vec& x, SFCurveType CT)
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
template <typename intcode_t> i3vec toSFCurveCoords(const intcode_t& x, SFCurveType CT)
{
    switch (CT) {
    case CURVE_MORTON: return toSFCurveCoords<intcode_t, CURVE_MORTON>(x);
    case CURVE_HILBERT: return toSFCurveCoords<intcode_t, CURVE_HILBERT>(x);
    case CURVE_RASTER: return toSFCurveCoords<intcode_t, CURVE_RASTER>(x);
    case CURVE_BOUSTRO: return toSFCurveCoords<intcode_t, CURVE_BOUSTRO>(x);
    case CURVE_TILED2: return toSFCurveCoords<intcode_t, CURVE_TILED2>(x);
    }
}

// Convert a 3D float vector to 3 integer code for later conversion to Morton
template <typename Vec_T> class FixedPointifier {
public:
    FixedPointifier(const tAABB<Vec_T>& world, const unsigned int nbits_) : m_nbits(nbits_)
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
    const unsigned int m_nbits;
    Vec_T m_scale, m_scaleInv;
    Vec_T m_bias;
};

// Returns the curve index
// x,y,z are the integer coordinates in 3D.
template <typename intcode_t> intcode_t computeHilbertCode(intcode_t x, intcode_t y, intcode_t z)
{
    const unsigned int nbits = sizeof(intcode_t) > sizeof(unsigned int) ? 20 : 10; // The curve order
    const intcode_t transform[8] = {0, 1, 7, 6, 3, 2, 4, 5};

    intcode_t s = 0;

    for (unsigned int i = nbits - 1; i >= 0; i--) {
        unsigned int xi = unsigned int((x) >> i) & 1;
        unsigned int yi = unsigned int((y) >> i) & 1;
        unsigned int zi = unsigned int((z) >> i) & 1;

        unsigned int index = (xi << 2) + (yi << 1) + zi;
        s = (s << 3) + transform[index];

        if (index == 0) {
            std::swap(y, z); // Swap y and z
        } else if (index == 1) {
            std::swap(x, y); // Swap x and y
        } else if (index == 2) {
            intcode_t temp = (z) ^ (-1);
            z = (y) ^ (-1);
            y = temp; // Swap and complement z and y
        } else if (index == 3) {
            intcode_t temp = (x) ^ (-1);
            x = (y) ^ (-1);
            y = temp; // Swap and complement x and y
        } else if (index == 4) {
            x = (x) ^ (-1);
            z = (z) ^ (-1); // Complement z and x
        } else if (index == 5) {
            std::swap(x, y); // Swap x and y
        } else if (index == 6) {
            x = (x) ^ (-1);
            z = (z) ^ (-1); // Complement z and x
        } else if (index == 7) {
            intcode_t temp = (x) ^ (-1);
            x = (y) ^ (-1);
            y = temp; // Swap and complement x and y
        }
    }

    return s;
}

// Returns the curve index
// x,y,z are the integer coordinates in 3D.
template <typename intcode_t> intcode_t computeMortonCode(intcode_t x, intcode_t y, intcode_t z)
{
    intcode_t codex = 0, codey = 0, codez = 0;

    const int nbits2 = sizeof(intcode_t) > sizeof(int) ? 40 : 20; // Nbits2 is 2X the curve order.

    for (int i = 0, andbit = 1; i < nbits2; i += 2, andbit <<= 1) {
        codex |= (x & andbit) << i;
        codey |= (y & andbit) << i;
        codez |= (z & andbit) << i;
    }

    return (codez << 2) | (codey << 1) | codex;
}

// Returns the curve index
// x,y,z are the integer coordinates in 3D.
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
template <typename intcode_t> intcode_t toMortonCode(const i3vec& x);
template <typename intcode_t> intcode_t toHilbertCode(const i3vec& x);
template <typename intcode_t> intcode_t toRasterCode(const i3vec& x);
template <typename intcode_t> intcode_t toBoustroCode(const i3vec& x);
template <typename intcode_t> intcode_t toTiled2Code(const i3vec& x);

template <typename intcode_t> i3vec toMortonCoords(const intcode_t& x);
template <typename intcode_t> i3vec toHilbertCoords(const intcode_t& x);
template <typename intcode_t> i3vec toRasterCoords(const intcode_t& x);
template <typename intcode_t> i3vec toBoustroCoords(const intcode_t& x);
template <typename intcode_t> i3vec toTiled2Coords(const intcode_t& x);

// Convert from two template arguments to one
template <> uint32_t toSFCurveCode<uint32_t, CURVE_MORTON>(const i3vec& x) { return toMortonCode<uint32_t>(x); }
template <> uint32_t toSFCurveCode<uint32_t, CURVE_HILBERT>(const i3vec& x) { return toHilbertCode<uint32_t>(x); }
template <> uint32_t toSFCurveCode<uint32_t, CURVE_RASTER>(const i3vec& x) { return toRasterCode<uint32_t>(x); }
template <> uint32_t toSFCurveCode<uint32_t, CURVE_BOUSTRO>(const i3vec& x) { return toBoustroCode<uint32_t>(x); }
template <> uint32_t toSFCurveCode<uint32_t, CURVE_TILED2>(const i3vec& x) { return toTiled2Code<uint32_t>(x); }

template <> uint64_t toSFCurveCode<uint64_t, CURVE_MORTON>(const i3vec& x) { return toMortonCode<uint64_t>(x); }
template <> uint64_t toSFCurveCode<uint64_t, CURVE_HILBERT>(const i3vec& x) { return toHilbertCode<uint64_t>(x); }
template <> uint64_t toSFCurveCode<uint64_t, CURVE_RASTER>(const i3vec& x) { return toRasterCode<uint64_t>(x); }
template <> uint64_t toSFCurveCode<uint64_t, CURVE_BOUSTRO>(const i3vec& x) { return toBoustroCode<uint64_t>(x); }
template <> uint64_t toSFCurveCode<uint64_t, CURVE_TILED2>(const i3vec& x) { return toTiled2Code<uint64_t>(x); }

template <> i3vec toSFCurveCoords<uint32_t, CURVE_MORTON>(const uint32_t& x) { return toMortonCoords<uint32_t>(x); }
template <> i3vec toSFCurveCoords<uint32_t, CURVE_HILBERT>(const uint32_t& x) { return toHilbertCoords<uint32_t>(x); }
template <> i3vec toSFCurveCoords<uint32_t, CURVE_RASTER>(const uint32_t& x) { return toRasterCoords<uint32_t>(x); }
template <> i3vec toSFCurveCoords<uint32_t, CURVE_BOUSTRO>(const uint32_t& x) { return toBoustroCoords<uint32_t>(x); }
template <> i3vec toSFCurveCoords<uint32_t, CURVE_TILED2>(const uint32_t& x) { return toTiled2Coords<uint32_t>(x); }

template <> i3vec toSFCurveCoords<uint64_t, CURVE_MORTON>(const uint64_t& x) { return toMortonCoords<uint64_t>(x); }
template <> i3vec toSFCurveCoords<uint64_t, CURVE_HILBERT>(const uint64_t& x) { return toHilbertCoords<uint64_t>(x); }
template <> i3vec toSFCurveCoords<uint64_t, CURVE_RASTER>(const uint64_t& x) { return toRasterCoords<uint64_t>(x); }
template <> i3vec toSFCurveCoords<uint64_t, CURVE_BOUSTRO>(const uint64_t& x) { return toBoustroCoords<uint64_t>(x); }
template <> i3vec toSFCurveCoords<uint64_t, CURVE_TILED2>(const uint64_t& x) { return toTiled2Coords<uint64_t>(x); }

// Actual implementations
template <typename intcode_t> intcode_t toMortonCode(const i3vec& v)
{
    intcode_t x = v.x, y = v.y, z = v.z;
    intcode_t codex = 0, codey = 0, codez = 0;

    const int nbits2 = sizeof(intcode_t) > sizeof(int) ? 40 : 20; // Nbits2 is 2X the curve order.

    for (int i = 0, andbit = 1; i < nbits2; i += 2, andbit <<= 1) {
        codex |= (intcode_t)(x & andbit) << i;
        codey |= (intcode_t)(y & andbit) << i;
        codez |= (intcode_t)(z & andbit) << i;
    }

    return (codez << 2) | (codey << 1) | codex;
}

template <typename intcode_t> intcode_t toHilbertCode(const i3vec& x) { return 0; }
template <typename intcode_t> intcode_t toRasterCode(const i3vec& x) { return 0; }
template <typename intcode_t> intcode_t toBoustroCode(const i3vec& x) { return 0; }
template <typename intcode_t> intcode_t toTiled2Code(const i3vec& x) { return 0; }

template <typename intcode_t> i3vec toMortonCoords(const intcode_t& x) { return i3vec(0); }
template <typename intcode_t> i3vec toHilbertCoords(const intcode_t& x) { return i3vec(0); }
template <typename intcode_t> i3vec toRasterCoords(const intcode_t& x) { return i3vec(0); }
template <typename intcode_t> i3vec toBoustroCoords(const intcode_t& x) { return i3vec(0); }
template <typename intcode_t> i3vec toTiled2Coords(const intcode_t& x) { return i3vec(0); }
