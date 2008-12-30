//////////////////////////////////////////////////////////////////////
// ColorMap.h - A color map implementation
//
// Maps a scalar to a color. Map can be arbitrary size. Pixel type is templated.

#ifndef dmc_colormap_h
#define dmc_colormap_h

#include "Util/Assert.h"
#include <vector>

template<class Pixel_T>
class ColorMap : public std::vector<Pixel_T>
{
    // Returns a linearly interpolated color.
    // s must be on 0.0 -> 1.0.
    // 0.0 maps exactly to (*this)[0].
    // 1.0 maps exactly to (*this)[Size-1].
    Pixel_T Sample01(const float s) const
    {
        ASSERT_R(s >= 0.0f && s <= 1.0f);
		float sS = s * float(std::vector<Pixel_T>::size()-1);
        size_t iLo = sS;
        float a = sS - float(iLo);
        size_t iHi = iLo + 1;
        if(iHi >= std::vector<Pixel_T>::size()) iHi = std::vector<Pixel_T>::size() - 1;
        Pixel_T V = (*this)[iHi] * a + (*this)[iLo] * (1.0f - a);
        return V;
    }

    // Set a portion of the map by linearly blending from vLo to vHigh
    // replacing the range (*this)[iLo] through (*this)[iHi].
    void RealSetSpan(const size_t iLo, const size_t iHi, const Pixel_T &vLo, const Pixel_T &vHi)
    {
        ASSERT_R(iLo >= 0 && iHi < std::vector<Pixel_T>::size() && iLo <= iHi);
        if(iLo == iHi) {
            (*this)[iLo] = vLo;
        } else {
            float Tmp = 1.0f / float(iHi-iLo);
            Pixel_T vSp = vHi - vLo;
            for(size_t i=iLo; i<=iHi; i++) {
                float s = (i-iLo) * Tmp;
                (*this)[i] = vLo + s * vSp;
            }
        }
    }
    
public:
    ColorMap() { }
    
    ColorMap(const size_t Size_)
    {
        std::vector<Pixel_T>::resize(Size_); 
    }
   
    // Returns a linearly interpolated color.
    // s gets clamped to 0.0 -> 1.0.
    Pixel_T operator()(const float s_) const
    {
        float s = Saturate(s_);
        return Sample01(s);
    }
    
    // Returns a linearly interpolated color.
    // s is wrapped to 0.0 -> 1.0.
    Pixel_T SampleWrapped(const float s_) const
    {
        float s = (s_ >= 0.0f) ? fmodf(s_, 1.0f) : (1.0f - fmodf(s_, 1.0f));
        return Sample01(s);
    }
    
    // Create a copy of this colormap and return a pointer to it
    DMC_INLINE ColorMap<Pixel_T> *Copy() const
    {
        ColorMap<Pixel_T> *C = new ColorMap<Pixel_T>();
		*C = *this;

        return C;
    }

    // Set a portion of the map by linearly blending from vLo to vHigh
    // replacing the range (*this)[iLo] through (*this)[iHi].
    void SetSpan(const size_t iLo_, const size_t iHi_, const Pixel_T &vLo_, const Pixel_T &vHi_)
    {
        size_t iLo, iHi;
        Pixel_T vLo, vHi;
        if(iHi_ < iLo_) {
            iLo = iHi_; vLo = vHi_;
            iHi = iLo_; vHi = vLo_;
        } else {
            iLo = iLo_; vLo = vLo_;
            iHi = iHi_; vHi = vHi_;
        }
        RealSetSpan(iLo, iHi, vLo, vHi);
    }

    // Set a portion of the map by linearly blending from vLo to vHigh
    // replacing the range sLo through sHi, which are on 0..1.
    void SetSpan(const float sLo_, const float sHi_, const Pixel_T &vLo_, const Pixel_T &vHi_)
    {
        float sLo, sHi;
        Pixel_T vLo, vHi;
        if(sHi_ < sLo_) {
            sLo = sHi_; vLo = vHi_;
            sHi = sLo_; vHi = vLo_;
        } else {
            sLo = sLo_; vLo = vLo_;
            sHi = sHi_; vHi = vHi_;
        }

        sLo = Saturate(sLo);
        sHi = Saturate(sHi);

        size_t iLo = size_t(Round(sLo * (float(std::vector<Pixel_T>::size()-1))));
        size_t iHi = size_t(Round(sHi * (float(std::vector<Pixel_T>::size()-1))));

        RealSetSpan(iLo, iHi, vLo, vHi);
    }
};

// True if the colormaps are identical.
template<class Pixel_T>
bool Equal(const ColorMap<Pixel_T> &Aa, const ColorMap<Pixel_T> &Bb)
{
    if(Aa.size() != Bb.size())
        return false;

    size_t Sz = Aa.size();
    for(size_t i=0; i<Sz; i++)
        if(Aa[i] != Bb[i])
            return false;

    return true;
}

#endif
