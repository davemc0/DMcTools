//////////////////////////////////////////////////////////////////////
// ColorMap.cpp - A color map implementation with templatized pixel type
//
// Maps a scalar to a color; uses arbitrary sized color maps
//
// Copyright David K. McAllister, 1998.

#include "Image/ColorMap.h"

#include "Image/tPixel.h"
#include "Math/MiscMath.h"
#include "Util/Assert.h"

#include <stdlib.h>

template <class Pixel_T> ColorMap<Pixel_T>::ColorMap(const size_t Size_) { C.resize(Size_); }
template ColorMap<f3Pixel>::ColorMap(const size_t Size_);

template <class Pixel_T> ColorMap<Pixel_T>::ColorMap(const ColorMap<Pixel_T>& O, const size_t Size_)
{
    if (Size_ < 1 || O.size() < 1) return;

    C.resize(Size_);

    float st = 1.0f / static_cast<float>(Size_ - 1);

    float v = 0;
    for (int i = 0; i < size(); i++) {
        C[i] = O(v);
        v += st;
    }
}
template ColorMap<f3Pixel>::ColorMap(const ColorMap<f3Pixel>& O, const size_t Size_);

template <class Pixel_T> Pixel_T ColorMap<Pixel_T>::operator()(const float s_) const
{
    float s = dmcm::Saturate(s_);
    return Sample01(s);
}
template f3Pixel ColorMap<f3Pixel>::operator()(const float s_) const;

template <class Pixel_T> Pixel_T ColorMap<Pixel_T>::SampleWrapped(const float s_) const
{
    float s = (s_ >= 0.0f) ? fmodf(s_, 1.0f) : (1.0f - fmodf(s_, 1.0f));
    return Sample01(s);
}
template f3Pixel ColorMap<f3Pixel>::SampleWrapped(const float s_) const;

template <class Pixel_T> ColorMap<Pixel_T>* ColorMap<Pixel_T>::Copy() const
{
    ColorMap<Pixel_T>* CMP = new ColorMap<Pixel_T>();
    *CMP = *this;

    return CMP;
}
template ColorMap<f3Pixel>* ColorMap<f3Pixel>::Copy() const;

template <class Pixel_T> void ColorMap<Pixel_T>::SetSpan(const size_t iLo_, const size_t iHi_, const Pixel_T& vLo_, const Pixel_T& vHi_)
{
    size_t iLo, iHi;
    Pixel_T vLo, vHi;
    if (iHi_ < iLo_) {
        iLo = iHi_;
        vLo = vHi_;
        iHi = iLo_;
        vHi = vLo_;
    } else {
        iLo = iLo_;
        vLo = vLo_;
        iHi = iHi_;
        vHi = vHi_;
    }
    RealSetSpan(iLo, iHi, vLo, vHi);
}
template void ColorMap<f3Pixel>::SetSpan(const size_t iLo_, const size_t iHi_, const f3Pixel& vLo_, const f3Pixel& vHi_);

template <class Pixel_T> void ColorMap<Pixel_T>::SetSpan(const float sLo_, const float sHi_, const Pixel_T& vLo_, const Pixel_T& vHi_)
{
    float sLo, sHi;
    Pixel_T vLo, vHi;
    if (sHi_ < sLo_) {
        sLo = sHi_;
        vLo = vHi_;
        sHi = sLo_;
        vHi = vLo_;
    } else {
        sLo = sLo_;
        vLo = vLo_;
        sHi = sHi_;
        vHi = vHi_;
    }

    sLo = dmcm::Saturate(sLo);
    sHi = dmcm::Saturate(sHi);

    size_t iLo = size_t(dmcm::Round(sLo * (float(size() - 1))));
    size_t iHi = size_t(dmcm::Round(sHi * (float(size() - 1))));

    RealSetSpan(iLo, iHi, vLo, vHi);
}
template void ColorMap<f3Pixel>::SetSpan(const float sLo_, const float sHi_, const f3Pixel& vLo_, const f3Pixel& vHi_);

template <class Pixel_T> Pixel_T ColorMap<Pixel_T>::Sample01(const float s) const
{
    ASSERT_R(s >= 0.0f && s <= 1.0f);
    ASSERT_D(size() > 0);

    float sS = s * float(size() - 1);
    size_t iLo = sS;
    float a = sS - float(iLo);
    size_t iHi = iLo + 1;
    if (iHi >= size()) iHi = size() - 1;
    Pixel_T V = (*this)[iHi] * a + (*this)[iLo] * (1.0f - a);
    return V;
}
template f3Pixel ColorMap<f3Pixel>::Sample01(const float s) const;

template <class Pixel_T> void ColorMap<Pixel_T>::RealSetSpan(const size_t iLo, const size_t iHi, const Pixel_T& vLo, const Pixel_T& vHi)
{
    ASSERT_R(iHi < size() && iLo <= iHi);
    if (iLo == iHi) {
        (*this)[iLo] = vLo;
    } else {
        float Tmp = 1.0f / float(iHi - iLo);
        Pixel_T vSp = vHi - vLo;
        for (size_t i = iLo; i <= iHi; i++) {
            float s = (i - iLo) * Tmp;
            (*this)[i] = vLo + s * vSp;
        }
    }
}
template void ColorMap<f3Pixel>::RealSetSpan(const size_t iLo, const size_t iHi, const f3Pixel& vLo, const f3Pixel& vHi);

template <class Pixel_T> void ColorMap<Pixel_T>::Dump()
{
    std::cerr << "CMap[" << size() << "]\n";
    for (int i = 0; i < size(); i++) std::cerr << (*this)[i] << std::endl;
}
template void ColorMap<f3Pixel>::Dump();

template <class Pixel_T> bool Equal(const ColorMap<Pixel_T>& Aa, const ColorMap<Pixel_T>& Bb)
{
    if (Aa.size() != Bb.size()) return false;

    size_t Sz = Aa.size();
    for (size_t i = 0; i < Sz; i++)
        if (Aa[i] != Bb[i]) return false;

    return true;
}
template bool Equal(const ColorMap<f3Pixel>& Aa, const ColorMap<f3Pixel>& Bb);
