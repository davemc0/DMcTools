//////////////////////////////////////////////////////////////////////
// tPixel.h - Generic pixel class suitable for graphics and stuff.
//
// Copyright David K. McAllister, June 2001.

#ifndef _tPixel_h
#define _tPixel_h

#include <Util/Assert.h>
#include <Math/MiscMath.h>

#ifdef DMC_USE_HALF_FLOAT
#include <Half/half.h>
#endif

#include <iostream>
#include <limits>

class basePixel
{
    // I declared these static since they don't depend on member data.
    // I was having trouble with them not being able to find a proper this.

    // This technique has the problem that it can only be used with these
    // six data types unless you change this class. I believe that's only
    // true if you depend on the pixel type casting operators in the tPixelN
    // derived classes. We'll see. What about a five-channel image?

    public:
    // Convert to type d from type s for pixels.
    // This encodes the assumptions of 0.X for N-bit fixed point.
    // Float and double map min -> max intensity to 0.0 -> 1.0, a la OpenGL.
    // These float to int converts are slow without SSE, so turn on SSE.
    // The float to int also do a Clamp.

    static void channel_cast(unsigned char &d, const unsigned char &s) {d = s;}
    static void channel_cast(unsigned short &d, const unsigned char &s) {unsigned short t=s; d=(t<<8)|t;}
    static void channel_cast(unsigned int &d, const unsigned char &s) {unsigned int t=s; d=(t<<24)|(t<<16)|(t<<8)|t;}
    static void channel_cast(half &d, const unsigned char &s) {d = s/float(0xff);}
    static void channel_cast(float &d, const unsigned char &s) {d = s/float(0xff);}
    static void channel_cast(double &d, const unsigned char &s) {d = s/double(0xff);}

    static void channel_cast(unsigned char &d, const unsigned short &s) {d = ((s>>7)+1)>>1;}
    static void channel_cast(unsigned short &d, const unsigned short &s) {d = s;}
    static void channel_cast(unsigned int &d, const unsigned short &s) {unsigned int t=s; d=(t<<16)|t;}
    static void channel_cast(half &d, const unsigned short &s) {d = s/float(0xffff);}
    static void channel_cast(float &d, const unsigned short &s) {d = s/float(0xffff);}
    static void channel_cast(double &d, const unsigned short &s) {d = s/double(0xffff);}

    static void channel_cast(unsigned char &d, const unsigned int &s) {d = ((s>>23)+1)>>1;}
    static void channel_cast(unsigned short &d, const unsigned int &s) {d = ((s>>15)+1)>>1;}
    static void channel_cast(unsigned int &d, const unsigned int &s) {d = s;}
    static void channel_cast(half &d, const unsigned int &s) {d = s/double(0xffffffff);}
    static void channel_cast(float &d, const unsigned int &s) {d = s/double(0xffffffff);}
    static void channel_cast(double &d, const unsigned int &s) {d = s/double(0xffffffff);}

    static void channel_cast(unsigned char &d, const float &s) {d = (unsigned char)Clamp(0.0f,s*float(0xff),float(0xff));}
    static void channel_cast(unsigned short &d, const float &s) {d = (unsigned short)Clamp(0.0f,s*float(0xffff),float(0xffff));}
    static void channel_cast(unsigned int &d, const float &s) {d = (unsigned int)Clamp(0.0,s*double(0xffffffff),double(0xffffffff));}
    static void channel_cast(half &d, const float &s) {d = half(s);}
    static void channel_cast(float &d, const float &s) {d = s;}
    static void channel_cast(double &d, const float &s) {d = double(s);}

    static void channel_cast(unsigned char &d, const half &s) {d = (unsigned char)Clamp(0.0f,float(s)*float(0xff),float(0xff));}
    static void channel_cast(unsigned short &d, const half &s) {d = (unsigned short)Clamp(0.0f,float(s)*float(0xffff),float(0xffff));}
    static void channel_cast(unsigned int &d, const half &s) {d = (unsigned int)Clamp(0.0,s*double(0xffffffff),double(0xffffffff));}
    static void channel_cast(half &d, const half &s) {d = s;}
    static void channel_cast(float &d, const half &s) {d = float(s);}
    static void channel_cast(double &d, const half &s) {d = double(s);}

    static void channel_cast(unsigned char &d, const double &s) {d = (unsigned char)Clamp(0.0,s*double(0xff),double(0xff));}
    static void channel_cast(unsigned short &d, const double &s) {d = (unsigned short)Clamp(0.0,s*double(0xffff),double(0xffff));}
    static void channel_cast(unsigned int &d, const double &s) {d = (unsigned int)Clamp(0.0,s*double(0xffffffff),double(0xffffffff));}
    static void channel_cast(half &d, const double &s) {d = half(s);}
    static void channel_cast(float &d, const double &s) {d = float(s);}
    static void channel_cast(double &d, const double &s) {d = s;}

    // Return the maximum pixel value for this data type.
    // It takes an unused input value since we can't key off return type in MSVC.
    // These values are a la OpenGL.

    static unsigned char channel_max(unsigned char) {return 0xff;}
    static unsigned short channel_max(unsigned short) {return 0xffff;}
    static unsigned int channel_max(unsigned int) {return 0xffffffff;}
    static float channel_max(float) {return 1.0f;}
    static double channel_max(double) {return 1.0;}

    // These are Rec. 709 (HDTV)
    // #define LUM_RF 0.212671
    // #define LUM_GF 0.715160
    // #define LUM_BF 0.072169

    // These are NTSC
    // #define LUM_RF 0.2904
    // #define LUM_GF 0.6051
    // #define LUM_BF 0.1045

    // These are Trinitron
#define LUM_RF 0.2582
#define LUM_GF 0.6566
#define LUM_BF 0.0851

    static unsigned char _doLum(const unsigned char r, const unsigned char g, const unsigned char b)
    {
        unsigned short wr, wg, wb;
        channel_cast(wr, LUM_RF); channel_cast(wg, LUM_GF); channel_cast(wb, LUM_BF);
        return (unsigned char)((r*(unsigned int)(wr) + g*(unsigned int)(wg) + b*(unsigned int)(wb)) >> 16);
    }
    static unsigned short _doLum(const unsigned short r, const unsigned short g, const unsigned short b)
    {
        unsigned short wr, wg, wb;
        channel_cast(wr, LUM_RF); channel_cast(wg, LUM_GF); channel_cast(wb, LUM_BF);
        return (unsigned short)((r*(unsigned int)(wr) + g*(unsigned int)(wg) + b*(unsigned int)(wb)) >> 16);
    }
    static unsigned int _doLum(const unsigned int r, const unsigned int g, const unsigned int b)
    {
        unsigned short wr, wg, wb;
        channel_cast(wr, LUM_RF); channel_cast(wg, LUM_GF); channel_cast(wb, LUM_BF);
        return ((r>>17)*(unsigned int)(wr) + (g>>17)*(unsigned int)(wg) + (b>>17)*(unsigned int)(wb)) << 1;
    }
    static float _doLum(const float r, const float g, const float b)
    {
        float wr=LUM_RF, wg=LUM_GF, wb=LUM_BF;
        return r*wr + g*wg + b*wb;
    }
    static double _doLum(const double r, const double g, const double b)
    {
        double wr=LUM_RF, wg=LUM_GF, wb=LUM_BF;
        return r*wr + g*wg + b*wb;
    }
};

template<class _ElType, int _Chan>
class tPixel : public basePixel
// Subclassing this didn't work on SGI because it adds an extra element to the sizeof the pixel. Bleh!
{
    _ElType els[_Chan]; // This is the data of the pixel.

public:
    // This is only used when passing the number of channels in this pixel
    // as a template argument. Normally use chan().
    enum {Chan = _Chan};
    typedef _ElType ElType; // The type of an element of the pixel.
    // The type to use for intermediate math results.
    // Should be specialized for each kind.
    // XXX How do I specialize this?
    typedef double MathType;

    //////////////////////////////////////////////////////////////////////
    // Constructors

    // Default constructor.
    tPixel() {//cerr << "base def" << _Chan << endl;
    }

    // Initialize all channels to value s.
    tPixel(const double &s)
    {
        //cerr << "base repl" << _Chan << endl;
        for(int i=0; i<chan(); i++) els[i] = _ElType(s);
    }

    // Copy constructor.
    tPixel(const tPixel<_ElType,_Chan> &p)
    {
        //cerr << "base cp" << _Chan << endl;
        for(int i=0; i<chan(); i++) els[i] = p[i];
    }

    // Copy assignment.
    tPixel<_ElType,_Chan> &operator=(const tPixel<_ElType,_Chan> &p)
    {
        //cerr << "op=" << _Chan << endl;
        if(this != &p) {
            for(int i=0; i<chan(); i++) els[i] = p[i];
        }
        return *this;
    }

    // Destructor.
    ~tPixel() {//cerr << "base ~" << _Chan << endl;
    }

    //////////////////////////////////////////////////////////////////////
    // Access functions

    // Return an element of this pixel.
    _ElType &operator[](int p)
    {
        ASSERT_D(p >= 0 && p < _Chan);
        return els[p];
    }

    // Return a const element of this pixel.
    const _ElType &operator[](int p) const
    {
        ASSERT_D(p >= 0 && p < _Chan);
        return els[p];
    }

    operator _ElType() { ASSERT_R(0); }
    operator _ElType() const { ASSERT_R(0); }
#if 0
    // cast operator to array of _ElType
    operator _ElType*()
    {
        return (_ElType *)this;
    }

    // cast operator to array of const _ElType
    operator const _ElType*() const
    {
        return (_ElType *)this;
    }
#endif

    //////////////////////////////////////////////////////////////////////
    // Utility functions

    // Return the number of channels in this pixel.
    static int chan()
    {
        // static because it doesn't access this.
        // That's why there's no const afterwards.
        return _Chan;
    }

    // True if the elements are not floating point.
    static bool is_integer()
    {
        // static because it doesn't access this.
        // That's why there's no const afterwards.
        return std::numeric_limits<_ElType>::is_integer;
    }

    // True if the elements are signed (all except unsigned char,unsigned short,unsigned int).
    static bool is_signed()
    {
        // static because it doesn't access this.
        // That's why there's no const afterwards.
        return std::numeric_limits<_ElType>::is_signed;
    }

    // Return the maximum channel value.
    _ElType max_chan() const
    {
        _ElType cmax = els[0];
        for(int i=1; i<chan(); i++) cmax = Max(cmax, els[i]);
        return cmax;
    }

    // Return the minimum channel value.
    _ElType min_chan() const
    {
        _ElType cmin = els[0];
        for(int i=1; i<chan(); i++) cmin = Min(cmin, els[i]);
        return cmin;
    }

    // Returns the sum of all the channels.
    // WARNING: This may not be what you want for alpha.
    _ElType sum_chan() const
    {
        _ElType csum = 0;
        for(int i=0; i<chan(); i++) csum += els[i];
        return csum;
    }

    //////////////////////////////////////////////////////////////////////
    // Pixel Operations

    // With a pixel, with assign

    tPixel<_ElType,_Chan> &operator+=(const tPixel<_ElType,_Chan> &p)
    {
        for(int i=0; i<chan(); i++) els[i] += p[i];
        return *this;
    }
    tPixel<_ElType,_Chan> &operator-=(const tPixel<_ElType,_Chan> &p)
    {
        for(int i=0; i<chan(); i++) els[i] -= p[i];
        return *this;
    }
    tPixel<_ElType,_Chan> &operator*=(const tPixel<_ElType,_Chan> &p)
    {
        for(int i=0; i<chan(); i++) els[i] *= p[i];
        return *this;
    }
    tPixel<_ElType,_Chan> &operator/=(const tPixel<_ElType,_Chan> &p)
    {
        for(int i=0; i<chan(); i++) els[i] /= p[i];
        return *this;
    }

    // With a constant, with assign
    // Can't template the scalar because it tries to route pixels into it. Bummer.

    tPixel<_ElType,_Chan> &operator+=(const _ElType s)
    {
        for(int i=0; i<chan(); i++) els[i] += s;
        return *this;
    }
    tPixel<_ElType,_Chan> &operator-=(const _ElType s)
    {
        for(int i=0; i<chan(); i++) els[i] -= s;
        return *this;
    }
    tPixel<_ElType,_Chan> &operator*=(const _ElType s)
    {
        for(int i=0; i<chan(); i++) els[i] *= s;
        return *this;
    }
    tPixel<_ElType,_Chan> &operator/=(const _ElType s)
    {
        for(int i=0; i<chan(); i++) els[i] /= s;
        return *this;
    }
};

// Apply an arbitrary function to each channel.
template<class _ElType, int _Chan, class _Pred>
tPixel<_ElType, _Chan> func(const tPixel<_ElType, _Chan> &p, _Pred _fnc)
{
    tPixel<_ElType,_Chan> r;
    for(int i=0; i<_Chan; i++) r[i] = _fnc(p[i]);
    return r;
}

// Unary minus.
template<class _ElType, int _Chan>
tPixel<_ElType,_Chan> operator-(const tPixel<_ElType,_Chan> &p)
{
    tPixel<_ElType,_Chan> r;
    for(int i=0; i<_Chan; i++) r[i] = -p[i];
    return r;
}

// Less than.
// Compares sum of the two pixels.
template<class _ElType, int _Chan>
bool operator<(const tPixel<_ElType,_Chan> &p1, const tPixel<_ElType,_Chan> &p2)
{
    _ElType s1 = p1.sum_chan();
    _ElType s2 = p2.sum_chan();
    return s1 < s2;
}

// Less than.
// Compares sum of the two pixels.
template<class _ElType, int _Chan>
bool operator<=(const tPixel<_ElType,_Chan> &p1, const tPixel<_ElType,_Chan> &p2)
{
    _ElType s1 = p1.sum_chan();
    _ElType s2 = p2.sum_chan();
    return s1 <= s2;
}

// Less than. True if all components are less than a constant.
template<class _ElType, int _Chan, class _InElType>
bool operator<(const tPixel<_ElType,_Chan> &p, const _InElType &s)
{
    bool ls = true;
    for(int i=0; i<_Chan; i++) ls = ls && (p[i] < _ElType(s));
    return ls;
}

// Equal.
// Doesn't use epsilon. WARNING: This is a different metric
// than used by less_than, so it may screw up sorts.
template<class _ElType, int _Chan>
bool operator==(const tPixel<_ElType,_Chan> &p1, const tPixel<_ElType,_Chan> &p2)
{
    bool eq = true;
    // Don't terminate early so that optimization will work better.
    for(int i=0; i<_Chan; i++) eq = eq && (p1[i] == p2[i]);
    return eq;
}

// Not equal.
// Doesn't use epsilon. WARNING: This is a different metric
// than used by less_than, so it may screw up sorts.
template<class _ElType, int _Chan>
bool operator!=(const tPixel<_ElType,_Chan> &p1, const tPixel<_ElType,_Chan> &p2)
{
    return !(p1 == p2);
}

// With two pixels
template<class _ElType, int _Chan>
tPixel<_ElType,_Chan> operator+(const tPixel<_ElType,_Chan> &p1, const tPixel<_ElType,_Chan> &p2)
{
    tPixel<_ElType,_Chan> r = p1;
    return r += p2;
}
template<class _ElType, int _Chan>
tPixel<_ElType,_Chan> operator-(const tPixel<_ElType,_Chan> &p1, const tPixel<_ElType,_Chan> &p2)
{
    tPixel<_ElType,_Chan> r = p1;
    return r -= p2;
}
template<class _ElType, int _Chan>
tPixel<_ElType,_Chan> operator*(const tPixel<_ElType,_Chan> &p1, const tPixel<_ElType,_Chan> &p2)
{
    tPixel<_ElType,_Chan> r = p1;
    return r *= p2;
}
template<class _ElType, int _Chan>
tPixel<_ElType,_Chan> operator/(const tPixel<_ElType,_Chan> &p1, const tPixel<_ElType,_Chan> &p2)
{
    tPixel<_ElType,_Chan> r = p1;
    return r /= p2;
}

// With a left scalar
template<class _ElType, int _Chan>
tPixel<_ElType,_Chan> operator+(const _ElType &s, const tPixel<_ElType,_Chan> &p)
{
    tPixel<_ElType,_Chan> r = s;
    return r += p;
}
template<class _ElType, int _Chan>
tPixel<_ElType,_Chan> operator-(const _ElType &s, const tPixel<_ElType,_Chan> &p)
{
    tPixel<_ElType,_Chan> r = s;
    return r -= p;
}
template<class _ElType, int _Chan>
tPixel<_ElType,_Chan> operator*(const _ElType &s, const tPixel<_ElType,_Chan> &p)
{
    tPixel<_ElType,_Chan> r = s;
    return r *= p;
}
template<class _ElType, int _Chan>
tPixel<_ElType,_Chan> operator/(const _ElType &s, const tPixel<_ElType,_Chan> &p)
{
    tPixel<_ElType,_Chan> r = s;
    return r /= p;
}

// With a right scalar
template<class _ElType, int _Chan>
tPixel<_ElType,_Chan> operator+(const tPixel<_ElType,_Chan> &p, const _ElType &s)
{
    tPixel<_ElType,_Chan> r = p;
    return r += s;
}
template<class _ElType, int _Chan>
tPixel<_ElType,_Chan> operator-(const tPixel<_ElType,_Chan> &p, const _ElType &s)
{
    tPixel<_ElType,_Chan> r = p;
    return r -= s;
}
template<class _ElType, int _Chan>
tPixel<_ElType,_Chan> operator*(const tPixel<_ElType,_Chan> &p, const _ElType &s)
{
    tPixel<_ElType,_Chan> r = p;
    return r *= s;
}
template<class _ElType, int _Chan>
tPixel<_ElType,_Chan> operator/(const tPixel<_ElType,_Chan> &p, const _ElType &s)
{
    tPixel<_ElType,_Chan> r = p;
    return r /= s;
}

// Linearly interpolate between pixels p1 and p2.
// If weight==0, returns p1. If weight==1, returns p2.
// WARNING: This should get overloaded for unsigned _ElTypes.
template<class _ElType, int _Chan>
tPixel<_ElType,_Chan> Interpolate(const tPixel<_ElType,_Chan> &p1,
                                  const tPixel<_ElType,_Chan> &p2,
                                  _ElType weight)
{
    return p1 + (p2 - p1) * weight;
}

// Channel-wise max.
template<class _ElType, int _Chan>
tPixel<_ElType,_Chan> Max(const tPixel<_ElType,_Chan> &p1, const tPixel<_ElType,_Chan> &p2)
{
    tPixel<_ElType,_Chan> r;
    for(int i=0; i<_Chan; i++) r[i] = Max(p1[i], p2[i]);
    return r;
}

// Channel-wise min.
template<class _ElType, int _Chan>
tPixel<_ElType,_Chan> Min(const tPixel<_ElType,_Chan> &p1, const tPixel<_ElType,_Chan> &p2)
{
    tPixel<_ElType,_Chan> r;
    for(int i=0; i<_Chan; i++) r[i] = Min(p1[i], p2[i]);
    return r;
}

// Channel-wise absolute value.
template<class _ElType, int _Chan>
tPixel<_ElType,_Chan> Abs(const tPixel<_ElType,_Chan> &p)
{
    tPixel<_ElType,_Chan> r;
    for(int i=0; i<_Chan; i++) r[i] = Abs(p[i]);
    return r;
}

// Component-wise multiply
template<class _ElType, int _Chan>
tPixel<_ElType,_Chan> CompMult(const tPixel<_ElType,_Chan> &p1, const tPixel<_ElType,_Chan> &p2)
{
    tPixel<_ElType,_Chan> r;
    for(int i=0; i<_Chan; i++) r[i] = p1[i] * p2[i];
    return r;
}

// Component-wise divide
template<class _ElType, int _Chan>
tPixel<_ElType,_Chan> CompDiv(const tPixel<_ElType,_Chan> &p1, const tPixel<_ElType,_Chan> &p2)
{
    tPixel<_ElType,_Chan> r;
    for(int i=0; i<_Chan; i++) r[i] = p1[i] / p2[i];
    return r;
}

// Channel-wise clamp.
template<class _ElType, int _Chan>
tPixel<_ElType,_Chan> Clamp(const tPixel<_ElType,_Chan> &cmin, const tPixel<_ElType,_Chan> &d,
                            const tPixel<_ElType,_Chan> &cmax)
{
    tPixel<_ElType,_Chan> r;
    for(int i=0; i<_Chan; i++) r[i] = Clamp(cmin[i], d[i], cmax[i]);
    return r;
}

// Print out the pixel.
template<class _ElType, int _Chan>
inline std::ostream &operator<<(std::ostream &out, const tPixel<_ElType,_Chan> &p)
{
    out << '[';
    for(int i=0; i<p.chan(); i++) {
        if(std::numeric_limits<_ElType>::digits <= 8)
            out << int(p[i]);
        else
            out << p[i];
        out << ((i==p.chan()-1)?"]":", ");
    }
    return out;
}

// Read in the pixel.
template<class _ElType, int _Chan>
inline std::istream& operator>>(std::istream& is, tPixel<_ElType,_Chan>& p)
{
    char st;
    for(int i=0; i<_Chan; i++) {
        is >> st >> p[i];
    }
    is >> st;
    return is;
}

#endif
