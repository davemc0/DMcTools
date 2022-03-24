//////////////////////////////////////////////////////////////////////
// tPixel.h - Generic pixel class suitable for graphics and stuff
//
// Copyright David K. McAllister, June 2001.
// Renovated in March 2007.

#pragma once

#include "Half/half.h"
#include "Math/BinaryRep.h"
#include "Math/MiscMath.h"
#include "Util/Assert.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <typeinfo>

// Various things we need to know about the pixel elements types
template <class Elem_T> struct element_traits {
    typedef int MathType;                     // For most channel types, do the math using signed int.
    typedef float FloatMathType;              // But sometimes even int types need float math, but not double.
    static const bool normalized = true;      // True for char and short. False otherwise.
    static const bool floating_point = false; // True for half, float, and double.
    static const size_t bytes = sizeof(Elem_T);
    static Elem_T one() { return std::numeric_limits<Elem_T>::max(); } // An intensity of 1.0 (it's either MAXINT or 1.0).
};

// For unsigned char and unsigned short the math type is a signed int, but for unsigned int we want it to be its own math type.
template <> struct element_traits<unsigned int> {
    typedef unsigned int MathType;
    typedef float FloatMathType;
    static const bool normalized = false;
    static const bool floating_point = false;
    static const size_t bytes = sizeof(unsigned int);
    static unsigned int one() { return std::numeric_limits<unsigned int>::max(); }
};

template <> struct element_traits<int> {
    typedef int MathType;
    typedef float FloatMathType;
    static const bool normalized = false;
    static const bool floating_point = false;
    static const size_t bytes = sizeof(int);
    static int one() { return std::numeric_limits<int>::max(); }
};

template <> struct element_traits<half> {
    typedef float MathType;
    typedef float FloatMathType;
    static const bool normalized = false;
    static const bool floating_point = true;
    static const size_t bytes = sizeof(half);
    static float one() { return 1; }
};

template <> struct element_traits<float> {
    typedef float MathType;
    typedef float FloatMathType;
    static const bool normalized = false;
    static const bool floating_point = true;
    static const size_t bytes = sizeof(float);
    static float one() { return 1; }
};

template <> struct element_traits<double> {
    typedef double MathType;
    typedef double FloatMathType;
    static const bool normalized = false;
    static const bool floating_point = true;
    static const size_t bytes = sizeof(double);
    static double one() { return 1; }
};

// Multiplication needs specialization for ints.
template <class Elem_T> DMC_DECL void mult_asgn(Elem_T& a, const Elem_T& b) { a *= b; }

// From Alvy Ray Smith's Principles of Image Compositing
template <> DMC_DECL void mult_asgn<unsigned char>(unsigned char& a, const unsigned char& b)
{
    int t = int(a) * int(b) + 0x80;
    a = (unsigned char)(((t >> 8) + t) >> 8);
}

template <> DMC_DECL void mult_asgn<unsigned short>(unsigned short& a, const unsigned short& b)
{
    int t = int(a) * int(b) + 0x8000;
    a = (unsigned short)(((t >> 16) + t) >> 16);
}

// Using basePixel has the problem that it can only be used with these six data types unless you change this class.
class basePixel {
public:
    // Convert pixel channel from In_T to Out_T.
    // unsigned char and unsigned short use 0.X for N-bit fixed point. int is non-normalized (i.e., just an integer).
    // Floating point map min -> max intensity to 0.0 -> 1.0, a la OpenGL.
    // These float to int converts are slow without SSE, so make sure you TURN ON SSE.
    // The float to int versions also do a clamp.
    template <class Out_T, class In_T> static void channel_cast(Out_T& d, const In_T& s)
    {
        if constexpr (element_traits<In_T>::bytes == element_traits<Out_T>::bytes && element_traits<In_T>::normalized == element_traits<Out_T>::normalized &&
                      element_traits<In_T>::floating_point == element_traits<Out_T>::floating_point) // both are same: Simple assign
            d = static_cast<Out_T>(s);                                                               // static_cast shouldn't be required but gcc complains.
        else if constexpr (!element_traits<In_T>::normalized && !element_traits<Out_T>::normalized)  // neither is normalized: Static cast
            d = static_cast<Out_T>(s);
        else if constexpr (!element_traits<In_T>::floating_point &&
                           !element_traits<Out_T>::floating_point) // one is normalized but neither is float: unsigned char -> int, etc.
            d = static_cast<Out_T>(s);
        else if constexpr (element_traits<In_T>::normalized && element_traits<Out_T>::floating_point) // Normalized int to float (non-normalized): Scale
            d = static_cast<Out_T>(s / static_cast<typename element_traits<Out_T>::FloatMathType>(element_traits<In_T>::one()));
        else if constexpr (element_traits<In_T>::floating_point && element_traits<Out_T>::normalized) // float (non-normalized) to normalized int: Scale and Clamp
            d = static_cast<Out_T>(clamp<In_T>(s * static_cast<In_T>(element_traits<Out_T>::one()), 0, static_cast<In_T>(element_traits<Out_T>::one())));
        else {           // both are normalized and integer: Complicated shift. Use specializations.
            ASSERT_R(0); // Should only arrive here with signed ints. Not yet implemented.
        }
    }

    // These are Rec. 709 (HDTV): 0.212671, 0.715160, 0.072169
    // These are NTSC: 0.2904, 0.6051, 0.1045
    // These are Trinitron
#define LUM_RF 0.2582
#define LUM_GF 0.6566
#define LUM_BF 0.0851

    static unsigned char doLum(const unsigned char r, const unsigned char g, const unsigned char b)
    {
        unsigned short wr, wg, wb;
        channel_cast(wr, LUM_RF);
        channel_cast(wg, LUM_GF);
        channel_cast(wb, LUM_BF);
        return (unsigned char)((r * (unsigned int)(wr) + g * (unsigned int)(wg) + b * (unsigned int)(wb)) >> 16);
    }
    static unsigned short doLum(const unsigned short r, const unsigned short g, const unsigned short b)
    {
        unsigned short wr, wg, wb;
        channel_cast(wr, LUM_RF);
        channel_cast(wg, LUM_GF);
        channel_cast(wb, LUM_BF);
        return (unsigned short)((r * (unsigned int)(wr) + g * (unsigned int)(wg) + b * (unsigned int)(wb)) >> 16);
    }
    static unsigned int doLum(const unsigned int r, const unsigned int g, const unsigned int b)
    {
        unsigned short wr, wg, wb;
        channel_cast(wr, LUM_RF);
        channel_cast(wg, LUM_GF);
        channel_cast(wb, LUM_BF);
        return ((r >> 17) * (unsigned int)(wr) + (g >> 17) * (unsigned int)(wg) + (b >> 17) * (unsigned int)(wb)) << 1;
    }
    static float doLum(const float r, const float g, const float b)
    {
        float wr = float(LUM_RF), wg = float(LUM_GF), wb = float(LUM_BF);
        return r * wr + g * wg + b * wb;
    }
    static double doLum(const double r, const double g, const double b)
    {
        double wr = LUM_RF, wg = LUM_GF, wb = LUM_BF;
        return r * wr + g * wg + b * wb;
    }
};

template <> DMC_DECL void basePixel::channel_cast(unsigned char& d, const unsigned short& s) { d = ((s >> 7) + 1) >> 1; }
template <> DMC_DECL void basePixel::channel_cast(unsigned short& d, const unsigned char& s)
{
    unsigned short t = s;
    d = (t << 8) | t;
}

template <class Elem_T, int Chan_> class tPixel : public basePixel {
    Elem_T els[Chan_]; // This is the data of the pixel.

public:
    typedef Elem_T ElType;                                                // The type of an element of the pixel.
    typedef typename element_traits<Elem_T>::MathType MathType;           // The type to use for most intermediate math results
    typedef typename element_traits<Elem_T>::FloatMathType FloatMathType; // The type to use for intermediate math results that must be float

    typedef tPixel<typename element_traits<Elem_T>::MathType, Chan_> MathPixType;           // The type to use for most intermediate pixel math
    typedef tPixel<typename element_traits<Elem_T>::FloatMathType, Chan_> FloatMathPixType; // The type to use for intermediate pixel math that must be float

    static const int Chan = Chan_;
    static const bool is_integer = std::numeric_limits<Elem_T>::is_integer; // True if the elements are not floating point.
    static const bool is_signed =
        std::numeric_limits<Elem_T>::is_signed; // True if the elements are signed (all except unsigned char, unsigned short, unsigned int).

    //////////////////////////////////////////////////////////////////////
    // Constructors

    // Need constructor from pointer to array of elements

    tPixel() {}
    // ~tPixel() {} // The existence of the destructor causes functions that return a pixel to not get inlined in MSVC 8. Huge performance problem.

    // Construct from any value replicated across all channels
    tPixel(const Elem_T e0)
    {
        for (int i = 0; i < Chan_; i++) (*this)[i] = e0;
    }

    // Construct from a list of elements.
    tPixel(const Elem_T e0, const Elem_T e1, const Elem_T e2, const Elem_T e3 = std::numeric_limits<Elem_T>::max())
    {
        (*this)[0] = e0;
        if constexpr (Chan_ > 1) (*this)[1] = e1;
        if constexpr (Chan_ > 2) (*this)[2] = e2;
        if constexpr (Chan_ > 3) (*this)[3] = e3;
    }

    // Construct a tPixel from any other tPixel
    // When src is 3 or 4 channels, we channel-wise copy, except when src=3, dst=4 we set alpha to 1.0.
    // When src is 1 or 2 channels, we replicate channel 0, except for src=2, dst=2 we channel-wise copy.
    template <class SrcEl_T, int SrcCh_> tPixel(const tPixel<SrcEl_T, SrcCh_>& s)
    {
        // std::cerr << "tPixel Copy " << SrcCh_ << "->" << Chan_ << std::endl;
        if constexpr (SrcCh_ == 1 || (SrcCh_ == 2 && Chan_ != 2)) {
            for (int i = 0; i < Chan_; i++) channel_cast((*this)[i], s[0]); // Replicate channel 0
        } else {
            if constexpr (SrcCh_ == 3 && Chan_ == 4) {
                for (int i = 0; i < 3; i++) channel_cast((*this)[i], s[i]);
                (*this)[3] = element_traits<Elem_T>::one(); // Set alpha to 1.0.
            } else {
                for (int i = 0; i < SrcCh_ && i < Chan_; i++) channel_cast((*this)[i], s[i]); // Channel-wise copy
            }
        }
    }

    //////////////////////////////////////////////////////////////////////
    // Access functions

    // Return an element of this pixel.
    DMC_DECL Elem_T& operator[](int p)
    {
        ASSERT_D(p >= 0 && p < Chan_);
        return els[p];
    }

    // Return a const element of this pixel.
    DMC_DECL const Elem_T& operator[](int p) const
    {
        ASSERT_D(p >= 0 && p < Chan_);
        return els[p];
    }

    // Convert channel 0 to Elem_T.
    // There are specializations for the one-channel tPixel classes.
    DMC_DECL operator Elem_T() const
    {
        ASSERT_DM(0, "Tried operator Elem_T() with more than one channel");
        // return 0;
    }

    // These are defined even for pixels with too few channels to work. Need error checking.
    // Writable.
    DMC_DECL Elem_T& r() { return (*this)[0]; }
    DMC_DECL Elem_T& g()
    {
        ASSERT_D(Chan_ > 1);
        return (*this)[1];
    }
    DMC_DECL Elem_T& b()
    {
        ASSERT_D(Chan_ > 2);
        return (*this)[2];
    }
    DMC_DECL Elem_T& a()
    {
        ASSERT_D(Chan_ > 3);
        return (*this)[3];
    }

    // Read-only.
    DMC_DECL const Elem_T r() const { return (*this)[0]; }
    DMC_DECL const Elem_T g() const
    {
        ASSERT_D(Chan_ > 1);
        return (*this)[1];
    }
    DMC_DECL const Elem_T b() const
    {
        ASSERT_D(Chan_ > 2);
        return (*this)[2];
    }
    DMC_DECL const Elem_T a() const
    {
        ASSERT_D(Chan_ > 3);
        return (*this)[3];
    }

    //////////////////////////////////////////////////////////////////////
    // Utility functions

    // Return the luminance of the r, g, b components
    Elem_T luminance() const
    {
        ASSERT_D(Chan_ >= 3);
        return doLum(r(), g(), b());
    }

    // Return the maximum channel value.
    Elem_T max_chan() const
    {
        Elem_T cmax = els[0];
        for (int i = 1; i < Chan_; i++) cmax = (Elem_T)std::max(cmax, els[i]);
        return cmax;
    }

    // Return the minimum channel value.
    Elem_T min_chan() const
    {
        Elem_T cmin = els[0];
        for (int i = 1; i < Chan_; i++) cmin = (Elem_T)std::min(cmin, els[i]);
        return cmin;
    }

    // Returns the sum of all the channels.
    // WARNING: This may not be what you want for alpha.
    Elem_T sum_chan() const
    {
        Elem_T csum = 0;
        for (int i = 0; i < Chan_; i++) csum += els[i];
        return csum;
    }

    //////////////////////////////////////////////////////////////////////
    // Pixel Operations

    // With a pixel, with assign

    tPixel<Elem_T, Chan_>& operator+=(const tPixel<Elem_T, Chan_>& p)
    {
        for (int i = 0; i < Chan_; i++) els[i] += p[i];
        return *this;
    }
    tPixel<Elem_T, Chan_>& operator-=(const tPixel<Elem_T, Chan_>& p)
    {
        for (int i = 0; i < Chan_; i++) els[i] -= p[i];
        return *this;
    }
    tPixel<Elem_T, Chan_>& operator*=(const tPixel<Elem_T, Chan_>& p)
    {
        for (int i = 0; i < Chan_; i++) mult_asgn(els[i], p[i]);
        return *this;
    }
    tPixel<Elem_T, Chan_>& operator/=(const tPixel<Elem_T, Chan_>& p)
    {
        for (int i = 0; i < Chan_; i++) els[i] /= p[i];
        return *this;
    }

    // With a constant, with assign
    // Can't template the scalar because it tries to route pixels into it. Bummer.

    tPixel<Elem_T, Chan_>& operator+=(const Elem_T s)
    {
        for (int i = 0; i < Chan_; i++) els[i] += s;
        return *this;
    }
    tPixel<Elem_T, Chan_>& operator-=(const Elem_T s)
    {
        for (int i = 0; i < Chan_; i++) els[i] -= s;
        return *this;
    }
    tPixel<Elem_T, Chan_>& operator*=(const Elem_T s)
    {
        for (int i = 0; i < Chan_; i++) mult_asgn(els[i], s);
        return *this;
    }
    tPixel<Elem_T, Chan_>& operator/=(const Elem_T s)
    {
        for (int i = 0; i < Chan_; i++) els[i] /= s;
        return *this;
    }
};

// Specializations to convert channel 0 to Elem_T.
template <> DMC_DECL tPixel<unsigned char, 1>::operator unsigned char() const { return els[0]; }
template <> DMC_DECL tPixel<unsigned short, 1>::operator unsigned short() const { return els[0]; }
template <> DMC_DECL tPixel<unsigned int, 1>::operator unsigned int() const { return els[0]; }
template <> DMC_DECL tPixel<half, 1>::operator half() const { return els[0]; }
template <> DMC_DECL tPixel<float, 1>::operator float() const { return els[0]; }
template <> DMC_DECL tPixel<double, 1>::operator double() const { return els[0]; }

// Apply an arbitrary function to each channel.
template <class Elem_T, int Chan_, class Func_T> DMC_DECL tPixel<Elem_T, Chan_> func(const tPixel<Elem_T, Chan_>& p, Func_T fnc)
{
    tPixel<Elem_T, Chan_> r;
    for (int i = 0; i < Chan_; i++) r[i] = fnc(p[i]);
    return r;
}

// Unary minus, channel-wise.
template <class Elem_T, int Chan_> DMC_DECL tPixel<Elem_T, Chan_> operator-(const tPixel<Elem_T, Chan_>& p)
{
    ASSERT_D(!(tPixel<Elem_T, Chan_>::is_signed));
    tPixel<Elem_T, Chan_> r;
    for (int i = 0; i < Chan_; i++) r[i] = -p[i];
    return r;
}

// Equal. Doesn't use epsilon.
template <class Elem_T, int Chan_> DMC_DECL bool operator==(const tPixel<Elem_T, Chan_>& p1, const tPixel<Elem_T, Chan_>& p2)
{
    bool eq = true;
    // Don't terminate early so that optimization will work better.
    for (int i = 0; i < Chan_; i++) eq = eq && (p1[i] == p2[i]);
    return eq;
}

// Not Equal. Doesn't use epsilon.
template <class Elem_T, int Chan_> DMC_DECL bool operator!=(const tPixel<Elem_T, Chan_>& p1, const tPixel<Elem_T, Chan_>& p2) { return !(p1 == p2); }

// With two pixels
template <class Elem_T, int Chan_> DMC_DECL tPixel<Elem_T, Chan_> operator+(const tPixel<Elem_T, Chan_>& p1, const tPixel<Elem_T, Chan_>& p2)
{
    tPixel<Elem_T, Chan_> r = p1;
    return r += p2;
}
template <class Elem_T, int Chan_> DMC_DECL tPixel<Elem_T, Chan_> operator-(const tPixel<Elem_T, Chan_>& p1, const tPixel<Elem_T, Chan_>& p2)
{
    tPixel<Elem_T, Chan_> r = p1;
    return r -= p2;
}
template <class Elem_T, int Chan_> DMC_DECL tPixel<Elem_T, Chan_> operator*(const tPixel<Elem_T, Chan_>& p1, const tPixel<Elem_T, Chan_>& p2)
{
    tPixel<Elem_T, Chan_> r = p1;
    return r *= p2;
}
template <> DMC_DECL tPixel<float, 1> operator*(const tPixel<float, 1>& p1, const tPixel<float, 1>& p2)
{
    tPixel<float, 1> r(p1[0] * p2[0]);
    return r;
}
template <class Elem_T, int Chan_> DMC_DECL tPixel<Elem_T, Chan_> operator/(const tPixel<Elem_T, Chan_>& p1, const tPixel<Elem_T, Chan_>& p2)
{
    tPixel<Elem_T, Chan_> r = p1;
    return r /= p2;
}

// With a left scalar
template <class Elem_T, int Chan_> DMC_DECL tPixel<Elem_T, Chan_> operator+(const Elem_T& s, const tPixel<Elem_T, Chan_>& p)
{
    tPixel<Elem_T, Chan_> r = s;
    return r += p;
}
template <class Elem_T, int Chan_> DMC_DECL tPixel<Elem_T, Chan_> operator-(const Elem_T& s, const tPixel<Elem_T, Chan_>& p)
{
    tPixel<Elem_T, Chan_> r = s;
    return r -= p;
}
template <class Elem_T, int Chan_> DMC_DECL tPixel<Elem_T, Chan_> operator*(const Elem_T& s, const tPixel<Elem_T, Chan_>& p)
{
    tPixel<Elem_T, Chan_> r = s;
    return r *= p;
}
template <class Elem_T, int Chan_> DMC_DECL tPixel<Elem_T, Chan_> operator/(const Elem_T& s, const tPixel<Elem_T, Chan_>& p)
{
    tPixel<Elem_T, Chan_> r = s;
    return r /= p;
}

// With a right scalar
template <class Elem_T, int Chan_> DMC_DECL tPixel<Elem_T, Chan_> operator+(const tPixel<Elem_T, Chan_>& p, const Elem_T& s)
{
    tPixel<Elem_T, Chan_> r = p;
    return r += s;
}
template <class Elem_T, int Chan_> DMC_DECL tPixel<Elem_T, Chan_> operator-(const tPixel<Elem_T, Chan_>& p, const Elem_T& s)
{
    tPixel<Elem_T, Chan_> r = p;
    return r -= s;
}
template <class Elem_T, int Chan_> DMC_DECL tPixel<Elem_T, Chan_> operator*(const tPixel<Elem_T, Chan_>& p, const Elem_T& s)
{
    tPixel<Elem_T, Chan_> r = p;
    return r *= s;
}
template <class Elem_T, int Chan_> DMC_DECL tPixel<Elem_T, Chan_> operator/(const tPixel<Elem_T, Chan_>& p, const Elem_T& s)
{
    tPixel<Elem_T, Chan_> r = p;
    return r /= s;
}

// Linearly interpolate between pixels p1 and p2.
// If weight==0, returns p1. If weight==1, returns p2.
template <class Pixel_T> DMC_DECL Pixel_T linInterp(const Pixel_T& p1, const Pixel_T& p2, typename Pixel_T::FloatMathType weight)
{
    return static_cast<typename Pixel_T::FloatMathPixType>(p1) +
        (static_cast<typename Pixel_T::FloatMathPixType>(p2) - static_cast<typename Pixel_T::FloatMathPixType>(p1)) * weight;
}

// Channel-wise max.
template <class Elem_T, int Chan_> DMC_DECL tPixel<Elem_T, Chan_> Max(const tPixel<Elem_T, Chan_>& p1, const tPixel<Elem_T, Chan_>& p2)
{
    tPixel<Elem_T, Chan_> r;
    for (int i = 0; i < Chan_; i++) r[i] = std::max(p1[i], p2[i]);
    return r;
}

// Channel-wise min.
template <class Elem_T, int Chan_> DMC_DECL tPixel<Elem_T, Chan_> Min(const tPixel<Elem_T, Chan_>& p1, const tPixel<Elem_T, Chan_>& p2)
{
    tPixel<Elem_T, Chan_> r;
    for (int i = 0; i < Chan_; i++) r[i] = std::min(p1[i], p2[i]);
    return r;
}

// Channel-wise absolute value.
template <class Elem_T, int Chan_> DMC_DECL tPixel<Elem_T, Chan_> Abs(const tPixel<Elem_T, Chan_>& p)
{
    tPixel<Elem_T, Chan_> r;
    for (int i = 0; i < Chan_; i++) r[i] = abs(p[i]);
    return r;
}

// Component-wise multiply
template <class Elem_T, int Chan_> DMC_DECL tPixel<Elem_T, Chan_> CompMult(const tPixel<Elem_T, Chan_>& p1, const tPixel<Elem_T, Chan_>& p2)
{
    tPixel<Elem_T, Chan_> r;
    for (int i = 0; i < Chan_; i++) r[i] = p1[i] * p2[i];
    return r;
}

// Component-wise divide
template <class Elem_T, int Chan_> DMC_DECL tPixel<Elem_T, Chan_> CompDiv(const tPixel<Elem_T, Chan_>& p1, const tPixel<Elem_T, Chan_>& p2)
{
    tPixel<Elem_T, Chan_> r;
    for (int i = 0; i < Chan_; i++) r[i] = p1[i] / p2[i];
    return r;
}

// Channel-wise clamp.
template <class Elem_T, int Chan_>
DMC_DECL tPixel<Elem_T, Chan_> Clamp(const tPixel<Elem_T, Chan_>& cmin, const tPixel<Elem_T, Chan_>& d, const tPixel<Elem_T, Chan_>& cmax)
{
    tPixel<Elem_T, Chan_> r;
    for (int i = 0; i < Chan_; i++) r[i] = clamp(d[i], cmin[i], cmax[i]);
    return r;
}

// Sum of squared channel differences.
template <class Elem_T, int Chan_> DMC_DECL typename tPixel<Elem_T, Chan_>::MathType DiffSqr(const tPixel<Elem_T, Chan_>& A, const tPixel<Elem_T, Chan_>& B)
{
    typename tPixel<Elem_T, Chan_>::MathPixType AM = static_cast<typename tPixel<Elem_T, Chan_>::MathPixType>(A);
    typename tPixel<Elem_T, Chan_>::MathPixType BM = static_cast<typename tPixel<Elem_T, Chan_>::MathPixType>(B);
    typename tPixel<Elem_T, Chan_>::MathPixType D = AM - BM;
    typename tPixel<Elem_T, Chan_>::MathType DifSum = 0;
    for (int i = 0; i < Chan_; i++) DifSum += sqr(D[i]);
    return DifSum;
}

template <class Elem_T, int Chan_> DMC_DECL bool isFinite(const tPixel<Elem_T, Chan_>& p)
{
    for (int i = 0; i < Chan_; i++)
        if (!isFinite(p[i]) || isNaN(p[i])) return false;
    return true;
}

// Print out the pixel.
template <class Elem_T, int Chan_> DMC_DECL std::ostream& operator<<(std::ostream& out, const tPixel<Elem_T, Chan_>& p)
{
    out << '[';
    for (int i = 0; i < Chan_; i++) {
        if constexpr (std::numeric_limits<Elem_T>::digits <= 8)
            out << int(p[i]);
        else
            out << p[i];
        out << ((i == Chan_ - 1) ? "]" : ", ");
    }
    return out;
}

// Read in the pixel.
template <class Elem_T, int Chan_> DMC_DECL std::istream& operator>>(std::istream& is, tPixel<Elem_T, Chan_>& p)
{
    char st;
    for (int i = 0; i < Chan_; i++) { is >> st >> p[i]; }
    is >> st;
    return is;
}

typedef tPixel<unsigned char, 1> uc1Pixel;
typedef tPixel<unsigned char, 2> uc2Pixel;
typedef tPixel<unsigned char, 3> uc3Pixel;
typedef tPixel<unsigned char, 4> uc4Pixel;

typedef tPixel<unsigned short, 1> us1Pixel;
typedef tPixel<unsigned short, 2> us2Pixel;
typedef tPixel<unsigned short, 3> us3Pixel;
typedef tPixel<unsigned short, 4> us4Pixel;

typedef tPixel<short, 1> ss1Pixel;
typedef tPixel<short, 2> ss2Pixel;
typedef tPixel<short, 3> ss3Pixel;
typedef tPixel<short, 4> ss4Pixel;

typedef tPixel<unsigned int, 1> ui1Pixel;
typedef tPixel<unsigned int, 2> ui2Pixel;
typedef tPixel<unsigned int, 3> ui3Pixel;
typedef tPixel<unsigned int, 4> ui4Pixel;

#ifdef DMC_USE_HALF_FLOAT
typedef tPixel<half, 1> h1Pixel;
typedef tPixel<half, 2> h2Pixel;
typedef tPixel<half, 3> h3Pixel;
typedef tPixel<half, 4> h4Pixel;
#endif

typedef tPixel<float, 1> f1Pixel;
typedef tPixel<float, 2> f2Pixel;
typedef tPixel<float, 3> f3Pixel;
typedef tPixel<float, 4> f4Pixel;

typedef tPixel<double, 1> d1Pixel;
typedef tPixel<double, 2> d2Pixel;
typedef tPixel<double, 3> d3Pixel;
typedef tPixel<double, 4> d4Pixel;
