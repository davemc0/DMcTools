#ifndef _rtrt_color_h
#define _rtrt_color_h

#include <Image/Pixel.h>

#if 1
class Color : f3Pixel
{
public:
        Color(int rr, int gg, int bb)
        {
                r() = rr;
                g() = gg;
                b() = bb;
        }

        float luminance() {return Luminance();}

        Color(f3Pixel p)
        {
                r() = p.r();
                g() = p.g();
                b() = p.b();
        }

        Color() {}
};
#else
 typedef f3Pixel Color;
#endif

#endif
