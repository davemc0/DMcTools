#ifndef _dmc_rgbe_h
#define _dmc_rgbe_h

#include <Image/Pixel.h>

#include <math.h>

#define DMC_EXPON_BIAS 128 // bias signed exponent to unsigned

// A subclass for RGBE pixels.
class rgbePixel : public tPixel<unsigned char, 4>
{
public:
    // Default constructor.
    rgbePixel() {}
    
    // Construct it with all elements specified.
    rgbePixel(const unsigned char _e0, const unsigned char _e1,
        const unsigned char _e2, const unsigned char _e3)
    {
        (*this)[0] = _e0;
        (*this)[1] = _e1;
        (*this)[2] = _e2;
        (*this)[3] = _e3;
    }

    // Copy constructor from base class.
    // XXX Is this a good idea?
    rgbePixel(const tPixel<unsigned char,4> &p)
    {
        std::cerr << "Questionable code!\n";
        ASSERT_R(0);
        for(int i=0; i<chan(); i++) (*this)[i] = p[i];
    }
    
    // Convert from f3Pixel.
    rgbePixel(const f3Pixel &p)
    {
		double maxc = p.max_chan();
		
		if(maxc <= 1e-32) {
			r() = g() = b() = 0;
			e() = 0;
		} else {
			int ex;
			double mant = frexp(maxc, &ex);
            // Mantissa is between 0.5 and 1.0.
            // Dividing by maxc normalizes the color channels by the max of the 3,
            // so one of them will get a value of 1.0, which becomes mant*256.
            // The other channels will get something less than mant*256.
            float val = mant * 255.9999 / maxc;
			
			r() = (unsigned char)(p.r() * val);
			g() = (unsigned char)(p.g() * val);
			b() = (unsigned char)(p.b() * val);
			e() = ex + DMC_EXPON_BIAS; // To store the signed exponent in an unsigned char.
			// cerr << "toRGBE "<<int(r()) << " " << int(g()) << " " << int(b()) << " " << int(e()) << endl;
		}
	}

    // Convert to f3Pixel.
	operator f3Pixel() const
	{
		f3Pixel p;

		if(e() == 0)
			p.r() = p.g() = p.b() = 0.f;
		else {
			float f = (float)ldexp(1.0, (int(e())-(DMC_EXPON_BIAS+8))); // Why the +8?
            // Since we just clamp when converting to RGBE, the value is probably closer
            // to the stored value + 0.5, so we bias it here.
			p.r() = (r() + 0.5f)*f;
			p.g() = (g() + 0.5f)*f;
			p.b() = (b() + 0.5f)*f;
		}

		return p;
	}
    
    // Writable.
    unsigned char &r() {return (*this)[0];}
    unsigned char &g() {return (*this)[1];}
    unsigned char &b() {return (*this)[2];}
    unsigned char &e() {return (*this)[3];}
    
    // Read-only.
    unsigned char r() const {return (*this)[0];}
    unsigned char g() const {return (*this)[1];}
    unsigned char b() const {return (*this)[2];}
    unsigned char e() const {return (*this)[3];}
    
    // Read-only.
    unsigned char red() const {return (*this)[0];}
    unsigned char green() const {return (*this)[1];}
    unsigned char blue() const {return (*this)[2];}
    unsigned char exponent() const {return (*this)[3];}
    
    // Luminance.
    unsigned char Luminance() const
    {
        ASSERT_R(0); // Not implemented.
        return (unsigned char)(0);
    }
};

// typedef rgbePixel RGBE;

#endif
