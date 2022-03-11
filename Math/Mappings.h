/////////////////////////////////////////////////////////////
// Mappings.h - Mappings between 3D vectors and 2D textures
//
// Copyright David K. McAllister, April 2010

// All mappings treat the 2D coordinates x,y as a u,v on -1..1 and z as an image index. z==0 is back, z==1 is front.
// For cube map z = +X:0 -X:1 +Y:2 -Y:3 +Z:4 -Z:5
// All To2D functions require v3 to be pre-normalized, unless a NN (non-normalized) version is called.
// All To3D output vectors will be normalized, unless a NN (non-normalized) version is called.

#pragma once

#include "Math/MiscMath.h"
#include "Math/Vector.h"

#include <algorithm>

template <class vec3_T> DMC_DECL vec3_T ParaboloidTo2DNN(const vec3_T& v3)
{
    typename vec3_T::ElType d = dmcm::Abs(v3.z) + v3.length();
    return vec3_T(v3.x / d, v3.y / d, v3.z < 0);
}

template <class vec3_T> DMC_DECL vec3_T ParaboloidTo2D(const vec3_T& v3)
{
    typename vec3_T::ElType d = dmcm::Abs(v3.z) + 1;
    return vec3_T(v3.x / d, v3.y / d, v3.z < 0);
}

template <class vec3_T> DMC_DECL vec3_T ParaboloidTo3DNN(const vec3_T& v2)
{
    return vec3_T(2 * v2.x, 2 * v2.y, v2.z ? (1 - v2.x * v2.x - v2.y * v2.y) : -(1 - v2.x * v2.x - v2.y * v2.y)); // Faster on Intel
    // return vec3_T(2*v2.x, 2*v2.y, dmcm::CopySign((1 - v2.x*v2.x - v2.y*v2.y), 0.5-v2.z));
}

template <class vec3_T> DMC_DECL vec3_T ParaboloidTo3D(const vec3_T& v2)
{
    typename vec3_T::ElType d = v2.x * v2.x + v2.y * v2.y + 1;
    // return vec3_T(2*v2.x/d, 2*v2.y/d, (1 - v2.x*v2.x - v2.y*v2.y) / dmcm::CopySign(d, 0.5-v2.z));
    return vec3_T(2 * v2.x / d, 2 * v2.y / d, (v2.z ? -(1 - v2.x * v2.x - v2.y * v2.y) : (1 - v2.x * v2.x - v2.y * v2.y)) / d); // Faster on Intel
}

template <class vec3_T> DMC_DECL vec3_T OctahedronTo2DNN(const vec3_T& v3)
{
    typename vec3_T::ElType d = dmcm::Abs(v3.x) + dmcm::Abs(v3.y) + dmcm::Abs(v3.z);
    if (v3.z >= 0)
        return vec3_T(v3.x / d, v3.y / d, 0); // +Z
    else
        return vec3_T((v3.x + dmcm::CopySign(1, v3.x) * -v3.z) / d, (v3.y + dmcm::CopySign(1, v3.y) * -v3.z) / d, 0); // -Z
}

template <class vec3_T> DMC_DECL vec3_T OctahedronTo2D(const vec3_T& v3) { return OctahedronTo2DNN(v3); }

template <class vec3_T> DMC_DECL vec3_T OctahedronTo3DNN(const vec3_T& v2)
{
    typename vec3_T::ElType z = 1 - dmcm::Abs(v2.x) - dmcm::Abs(v2.y);
    if (z >= 0)
        return vec3_T(v2.x, v2.y, z); // +Z
    else
        return vec3_T(dmcm::CopySign(1, v2.x) * (1 - dmcm::Abs(v2.y)), dmcm::CopySign(1, v2.y) * (1 - dmcm::Abs(v2.x)), z); // -Z
}

template <class vec3_T> DMC_DECL vec3_T OctahedronTo3D(const vec3_T& v2) { return OctahedronTo3DNN(v2).normalized(); }

template <class vec3_T> DMC_DECL vec3_T PyramidTo2DNN(const vec3_T& v3)
{
    typename vec3_T::ElType d = dmcm::Abs(v3.z) + std::max(dmcm::Abs(v3.x), dmcm::Abs(v3.y));
    return vec3_T(v3.x / d, v3.y / d, v3.z < 0);
}

template <class vec3_T> DMC_DECL vec3_T PyramidTo2D(const vec3_T& v3) { return PyramidTo2DNN(v3); }

template <class vec3_T> DMC_DECL vec3_T PyramidTo3DNN(const vec3_T& v2)
{
    typename vec3_T::ElType d = 1 - std::max(dmcm::Abs(v2.x), dmcm::Abs(v2.y));
    // return vec3_T(v2.x, v2.y, CopySign(d, 0.5-v2.z));
    return vec3_T(v2.x, v2.y, v2.z > 0 ? -d : d); // Faster on Intel.
}

template <class vec3_T> DMC_DECL vec3_T PyramidTo3D(const vec3_T& v2) { return PyramidTo3DNN(v2).normalized(); }

template <class vec3_T> DMC_DECL vec3_T CubeTo2D(const vec3_T& v3) {}

template <class vec3_T> DMC_DECL vec3_T CubeTo3D(const vec3_T& v2) {}

template <class vec3_T> DMC_DECL vec3_T SphereTo2D(const vec3_T& v3) {}

template <class vec3_T> DMC_DECL vec3_T SphereTo3D(const vec3_T& v2) {}

template <class vec3_T> DMC_DECL vec3_T HemisphereTo2D(const vec3_T& v3) {}

template <class vec3_T> DMC_DECL vec3_T HemisphereTo3D(const vec3_T& v2) {}

// Does the simple linear mapping from u,v to integer pixel coords
// u,v is on -1..1. x,y is on 0..imsize-1.
// Ignorant of face number.
template <class Fl_T> DMC_DECL void UVToXY(int& x, int& y, const Fl_T u, const Fl_T v, const int imsize)
{
    x = static_cast<int>((u + 1) * 0.5 * static_cast<Fl_T>(imsize - 1) + 0.5);
    y = static_cast<int>((v + 1) * 0.5 * static_cast<Fl_T>(imsize - 1) + 0.5);
}

// Does the simple linear mapping from integer pixel coords to u,v
// u,v is on -1..1. x,y is on 0..imsize-1.
// Ignorant of face number.
template <class Fl_T> DMC_DECL void XYToUV(Fl_T& u, Fl_T& v, const int x, const int y, const int imsize)
{
    u = static_cast<Fl_T>(x) * 2.0 / static_cast<Fl_T>(imsize - 1) - 1.0;
    v = static_cast<Fl_T>(y) * 2.0 / static_cast<Fl_T>(imsize - 1) - 1.0;
}

// Does the simple linear mapping from u,v to integer pixel coords
// u,v is on -1..1. x,y is on 0..imsize-1.
// Offsets the x value based on face number
template <class vec3_T> DMC_DECL void UVToXY(int& x, int& y, const vec3_T& v2, const int imsize)
{
    int z = static_cast<int>(v2.z) * imsize;
    x = static_cast<int>((v2.x + 1) * 0.5 * static_cast<typename vec3_T::ElType>(imsize - 1) + 0.5) + z;
    y = static_cast<int>((v2.y + 1) * 0.5 * static_cast<typename vec3_T::ElType>(imsize - 1) + 0.5);
}

// Does the simple linear mapping from integer pixel coords to u,v
// u,v is on -1..1. x,y is on 0..imsize-1.
// Offsets the x value based on face number
template <class vec3_T> DMC_DECL void XYToUV(vec3_T& v2, const int x, const int y, const int imsize)
{
    v2.x = static_cast<typename vec3_T::ElType>(x % imsize) * 2.0 / static_cast<typename vec3_T::ElType>(imsize - 1) - 1.0;
    v2.y = static_cast<typename vec3_T::ElType>(y) * 2.0 / static_cast<typename vec3_T::ElType>(imsize - 1) - 1.0;
    v2.z = static_cast<typename vec3_T::ElType>(x / imsize);
}
