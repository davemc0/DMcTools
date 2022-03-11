//////////////////////////////////////////////////////////////////////
// ColorMap.h - A color map implementation with templatized pixel type
//
// Maps a scalar to a color; uses arbitrary sized color maps
//
// Copyright David K. McAllister, 1998.

#pragma once

#include <vector>

template <class Pixel_T> class ColorMap {
public:
    // Empty one of an empty size
    ColorMap() {}

    // Empty one of a given size
    ColorMap(const size_t Size_);

    // Copy of a given size, made by resampling O. If O size is 0, this size is 0.
    ColorMap(const ColorMap& O, const size_t Size_);

    // Returns a linearly interpolated color.
    // s gets clamped to 0.0 -> 1.0.
    Pixel_T operator()(const float s_) const;

    // Returns a linearly interpolated color.
    // s is wrapped to 0.0 -> 1.0.
    Pixel_T SampleWrapped(const float s_) const;

    // Create a copy of this colormap and return a pointer to it
    ColorMap<Pixel_T>* Copy() const;

    // Set a portion of the map by linearly blending from vLo to vHigh
    // replacing the range (*this)[iLo] through (*this)[iHi].
    void SetSpan(const size_t iLo_, const size_t iHi_, const Pixel_T& vLo_, const Pixel_T& vHi_);

    // Set a portion of the map by linearly blending from vLo to vHigh
    // replacing the range sLo through sHi, which are on 0..1.
    void SetSpan(const float sLo_, const float sHi_, const Pixel_T& vLo_, const Pixel_T& vHi_);

    std::vector<Pixel_T> C;

    size_t size() const { return C.size(); }
    const typename Pixel_T& operator[](const size_t i) const { return C[i]; }
    typename Pixel_T& operator[](const size_t i) { return C[i]; }

    void Dump(); // Dumps ColorMap to stderr

private:
    // Returns a linearly interpolated color.
    // s must be on 0.0 -> 1.0.
    // 0.0 maps exactly to (*this)[0].
    // 1.0 maps exactly to (*this)[Size-1].
    Pixel_T Sample01(const float s) const;

    // Set a portion of the map by linearly blending from vLo to vHigh
    // replacing the range (*this)[iLo] through (*this)[iHi].
    void RealSetSpan(const size_t iLo, const size_t iHi, const Pixel_T& vLo, const Pixel_T& vHi);
};

// True if the colormaps are identical.
template <class Pixel_T> bool Equal(const ColorMap<Pixel_T>& Aa, const ColorMap<Pixel_T>& Bb);
