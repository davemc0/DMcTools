//////////////////////////////////////////////////////////////////////
// RGBE.h - The RGB Exponent color representation
//
// Copyright David K. McAllister, 2000.

#pragma once

#include "Image/tPixel.h"

#include <cmath>

// A subclass for RGBE pixels.
class rgbePixel : public tPixel<unsigned char, 4> {
    static const int DMC_EXPON_BIAS = 128; // bias signed exponent to unsigned

public:
    // Default constructor.
    rgbePixel() {}

    // Convert from f3Pixel.
    rgbePixel(const f3Pixel& p)
    {
        float maxc = p.max_chan();

        if (maxc <= 1e-32) {
            r() = g() = b() = 0;
            e() = 0;
        } else {
            int ex;
            float mant = frexp(maxc, &ex);
            // Mantissa is between 0.5 and 1.0.
            // Dividing by maxc normalizes the color channels by the max of the 3,
            // so one of them will get a value of 1.0, which becomes mant*256.
            // The other channels will get something less than mant*256.
            float val = mant * 255.9999f / maxc;

            r() = (unsigned char)(p.r() * val);
            g() = (unsigned char)(p.g() * val);
            b() = (unsigned char)(p.b() * val);
            e() = ex + DMC_EXPON_BIAS; // To store the signed exponent in an unsigned char.
            // cerr << "toRGBE "<<int(r()) << " " << int(g()) << " " << int(b()) << " " << int(e()) << endl;
        }
    }

    // Convert to f3Pixel.
    // operator f3Pixel() const {} This doesn't get called for some reason. Is it because copy constructors have a higher priority? If so, how do I override?

    f3Pixel make_f3Pixel() const
    {
        f3Pixel p;

        if (e() == 0)
            p.r() = p.g() = p.b() = 0.f;
        else {
            float f = (float)ldexp(1.0, (int(e()) - (DMC_EXPON_BIAS + 8))); // Why the +8?
            // Since we just clamp when converting to RGBE, the value is probably closer
            // to the stored value + 0.5, so we bias it here.
            p.r() = (r() + 0.5f) * f;
            p.g() = (g() + 0.5f) * f;
            p.b() = (b() + 0.5f) * f;
        }

        return p;
    }

    rgbePixel& operator=(f3Pixel& f)
    {
        rgbePixel r(f);
        *this = r;

        return *this;
    }

    // Writable.
    unsigned char& e() { return (*this)[3]; }

    // Read-only.
    unsigned char e() const { return (*this)[3]; }

    // Luminance.
    unsigned char luminance() const
    {
        ASSERT_R(0); // Not implemented.
        // return (unsigned char)(0);
    }
};
