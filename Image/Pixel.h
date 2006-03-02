//////////////////////////////////////////////////////////////////////
// Pixel.h - Specific pixel classes suitable for making images.
//
// Copyright David K. McAllister, June 2001.

// Still have the problem that doing arithmetic on two pixels returns
// an item of the base class, so user code may has to cast it back if a member
// function of the derived class is desired.

// Color * double fails: class doesn't define operator

// Should use member function specialization to fix my worries about overflowing
// fixed point pixels for things like +=.

// Some day I would like to replace these with partial specializations of tPixel,
// rather than child classes thereof. It would be cleaner.
// The obstacle is that MSVC doesn't support partial specilization.

#ifndef _Pixel_h
#define _Pixel_h

#include <Image/tPixel.h>
#ifdef DMC_USE_HALF_FLOAT
#include <Half/half.h>
#endif

// WARNING: For derived class operations (casting and channel access),
// OpenGL-like is assumed:
// A 1 channel image is luminance (grayscale).
// A 2 channel image is luminance and alpha.
// A 3 channel image is RGB.
// A 4 channel image is RGBA.
// But for tPixel base class operations, all channels treated identically.

template<class _ElType> class t1Pixel;
template<class _ElType> class t2Pixel;
template<class _ElType> class t3Pixel;
template<class _ElType> class t4Pixel;

// A subclass for all one-channel pixels.
template<class _ElType> class t1Pixel : public tPixel<_ElType, 1>
{
public:
    // Default constructor.
    t1Pixel() {}
 
    // Copy constructor from base class.
    t1Pixel(const tPixel<_ElType,1> &p)
    {
        for(int i=0; i<chan(); i++) (*this)[i] = p[i];
    }

    // Only one-channel pixels have this:
    // Treat the pixel as its first element.
    operator _ElType()
    {
        return (*this)[0];
    }

    operator _ElType() const
    {
        return (*this)[0];
    }

    // Convert any kind of pixel to this kind.
    template<class _ElTypeSrc> t1Pixel(const t1Pixel<_ElTypeSrc> &p)
    {
        pixel_cast((*this)[0], p[0]);
    }
    template<class _ElTypeSrc> t1Pixel(const t2Pixel<_ElTypeSrc> &p)
    {
        pixel_cast((*this)[0], p[0]);
    }
    template<class _ElTypeSrc> t1Pixel(const t3Pixel<_ElTypeSrc> &p)
    {
        pixel_cast((*this)[0], p.Luminance());
    }
    template<class _ElTypeSrc> t1Pixel(const t4Pixel<_ElTypeSrc> &p)
    {
        pixel_cast((*this)[0], p.Luminance());
    }

    // Writable luminance component.
    _ElType &l() {return (*this)[0];}
    
    // Read-only luminance component.
    _ElType l() const {return (*this)[0];}
    
    // Read-only luminance component.
    _ElType lum() const {return (*this)[0];}
};

// A subclass for all two-channel pixels.
template<class _ElType> class t2Pixel : public tPixel<_ElType, 2>
{
public:
    // Default constructor.
    t2Pixel() {}
    
    // Construct it with all elements specified.
    t2Pixel(const _ElType _e0, const _ElType _e1)
    {
        (*this)[0] = _e0;
        (*this)[1] = _e1;
    }
 
    // Copy constructor from base class.
    t2Pixel(const tPixel<_ElType,2> &p)
    {
        for(int i=0; i<chan(); i++) (*this)[i] = p[i];
    }

    // Convert any kind of pixel to this kind.
    template<class _ElTypeSrc> t2Pixel(const t1Pixel<_ElTypeSrc> &p)
    {
        pixel_cast((*this)[0], p[0]);
        (*this)[1] = pixel_max(_ElType(0));
    }
    template<class _ElTypeSrc> t2Pixel(const t2Pixel<_ElTypeSrc> &p)
    {
        pixel_cast((*this)[0], p[0]);
        pixel_cast((*this)[1], p[1]);
    }
    template<class _ElTypeSrc> t2Pixel(const t3Pixel<_ElTypeSrc> &p)
    {
        pixel_cast((*this)[0], p.Luminance());
        (*this)[1] = pixel_max(_ElType(0));
    }
    template<class _ElTypeSrc> t2Pixel(const t4Pixel<_ElTypeSrc> &p)
    {
        pixel_cast((*this)[0], p.Luminance());
        pixel_cast((*this)[1], p[3]);
    }

    // Writable.
    _ElType &l() {return (*this)[0];}
    _ElType &a() {return (*this)[1];}
    
    // Read-only.
    _ElType l() const {return (*this)[0];}
    _ElType a() const {return (*this)[1];}
    
    // Read-only.
    _ElType lum() const {return (*this)[0];}
    _ElType alpha() const {return (*this)[1];}
};

// A subclass for all three-channel pixels.
template<class _ElType> class t3Pixel : public tPixel<_ElType, 3>
{
public:
    // Default constructor.
    t3Pixel() {}
    
    // Construct it with all elements specified.
    t3Pixel(const _ElType _e0, const _ElType _e1, const _ElType _e2)
    {
        (*this)[0] = _e0;
        (*this)[1] = _e1;
        (*this)[2] = _e2;
    }
    
    // Copy constructor from base class.
    t3Pixel(const tPixel<_ElType,3> &p)
    {
		for(int i=0; i<tPixel<_ElType,3>::chan(); i++) (*this)[i] = p[i];
    }

    // Convert any kind of pixel to this kind.
    template<class _ElTypeSrc> t3Pixel(const t1Pixel<_ElTypeSrc> &p)
    {
        pixel_cast((*this)[0], p[0]);
        pixel_cast((*this)[1], p[0]);
        pixel_cast((*this)[2], p[0]);
    }
    template<class _ElTypeSrc> t3Pixel(const t2Pixel<_ElTypeSrc> &p)
    {
        pixel_cast((*this)[0], p[0]);
        pixel_cast((*this)[1], p[0]);
        pixel_cast((*this)[2], p[0]);
    }
    template<class _ElTypeSrc> t3Pixel(const t3Pixel<_ElTypeSrc> &p)
    {
        pixel_cast((*this)[0], p[0]);
        pixel_cast((*this)[1], p[1]);
        pixel_cast((*this)[2], p[2]);
    }
    template<class _ElTypeSrc> t3Pixel(const t4Pixel<_ElTypeSrc> &p)
    {
        pixel_cast((*this)[0], p[0]);
        pixel_cast((*this)[1], p[1]);
        pixel_cast((*this)[2], p[2]);
    }
    
    // Writable.
    _ElType &r() {return (*this)[0];}
    _ElType &g() {return (*this)[1];}
    _ElType &b() {return (*this)[2];}
    
    // Read-only.
    _ElType r() const {return (*this)[0];}
    _ElType g() const {return (*this)[1];}
    _ElType b() const {return (*this)[2];}
    
    // Read-only.
    _ElType red() const {return (*this)[0];}
    _ElType green() const {return (*this)[1];}
    _ElType blue() const {return (*this)[2];}
    
    // Luminance.
    _ElType Luminance() const {return _doLum(r(), g(), b());}
};

// A subclass for all four-channel pixels.
template<class _ElType> class t4Pixel : public tPixel<_ElType, 4>
{
public:
    // Default constructor.
    t4Pixel() {}
    
    // Construct it with all elements specified.
    t4Pixel(const _ElType _e0, const _ElType _e1, const _ElType _e2, const _ElType _e3)
    {
        (*this)[0] = _e0;
        (*this)[1] = _e1;
        (*this)[2] = _e2;
        (*this)[3] = _e3;
    }

    // Copy constructor from base class.
    t4Pixel(const tPixel<_ElType,4> &p)
    {
        for(int i=0; i<chan(); i++) (*this)[i] = p[i];
    }
    
    // Convert any kind of pixel to this kind.
    // These have to come from the tPixelN<_El> classes, rather than the
    // tPixel<_El,N> class because otherwise initialization via a replicated
    // constant gets confused. It doesn't know which kind of tPixel<_El,N>
    // to make, since any of them can be made and any can be accepted.
    template<class _ElTypeSrc> t4Pixel(const t1Pixel<_ElTypeSrc> &p)
    {
        pixel_cast((*this)[0], p[0]);
        pixel_cast((*this)[1], p[0]);
        pixel_cast((*this)[2], p[0]);
        (*this)[3] = pixel_max(_ElType(0));
    }
    template<class _ElTypeSrc> t4Pixel(const t2Pixel<_ElTypeSrc> &p)
    {
        pixel_cast((*this)[0], p[0]);
        pixel_cast((*this)[1], p[0]);
        pixel_cast((*this)[2], p[0]);
        pixel_cast((*this)[3], p[1]);
    }
    template<class _ElTypeSrc> t4Pixel(const t3Pixel<_ElTypeSrc> &p)
    {
        pixel_cast((*this)[0], p[0]);
        pixel_cast((*this)[1], p[1]);
        pixel_cast((*this)[2], p[2]);
        (*this)[3] = pixel_max(_ElType(0));
    }
    template<class _ElTypeSrc> t4Pixel(const t4Pixel<_ElTypeSrc> &p)
    {
        pixel_cast((*this)[0], p[0]);
        pixel_cast((*this)[1], p[1]);
        pixel_cast((*this)[2], p[2]);
        pixel_cast((*this)[3], p[3]);
    }
    
    // Writable.
    _ElType &r() {return (*this)[0];}
    _ElType &g() {return (*this)[1];}
    _ElType &b() {return (*this)[2];}
    _ElType &a() {return (*this)[3];}
    
    // Read-only.
    _ElType r() const {return (*this)[0];}
    _ElType g() const {return (*this)[1];}
    _ElType b() const {return (*this)[2];}
    _ElType a() const {return (*this)[3];}
    
    // Read-only.
    _ElType red() const {return (*this)[0];}
    _ElType green() const {return (*this)[1];}
    _ElType blue() const {return (*this)[2];}
    _ElType alpha() const {return (*this)[3];}
    
    // Luminance.
    _ElType Luminance() const {return _doLum(r(), g(), b());}
};

typedef t1Pixel<unsigned char> uc1Pixel;
typedef t2Pixel<unsigned char> uc2Pixel;
typedef t3Pixel<unsigned char> uc3Pixel;
typedef t4Pixel<unsigned char> uc4Pixel;

typedef t1Pixel<unsigned short> us1Pixel;
typedef t2Pixel<unsigned short> us2Pixel;
typedef t3Pixel<unsigned short> us3Pixel;
typedef t4Pixel<unsigned short> us4Pixel;

typedef t1Pixel<short> s1Pixel;
typedef t2Pixel<short> s2Pixel;
typedef t3Pixel<short> s3Pixel;
typedef t4Pixel<short> s4Pixel;

typedef t1Pixel<unsigned int> ui1Pixel;
typedef t2Pixel<unsigned int> ui2Pixel;
typedef t3Pixel<unsigned int> ui3Pixel;
typedef t4Pixel<unsigned int> ui4Pixel;

#ifdef DMC_USE_HALF_FLOAT
typedef t1Pixel<half> h1Pixel;
typedef t2Pixel<half> h2Pixel;
typedef t3Pixel<half> h3Pixel;
typedef t4Pixel<half> h4Pixel;
#endif

typedef t1Pixel<float> f1Pixel;
typedef t2Pixel<float> f2Pixel;
typedef t3Pixel<float> f3Pixel;
typedef t4Pixel<float> f4Pixel;

// typedef f3Pixel Color;

#endif
