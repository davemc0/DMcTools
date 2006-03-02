//////////////////////////////////////////////////////////////////////
// EnvMap.h - Many representations of environment maps
//
// Copyright David K. McAllister, 2004.

#ifndef _EnvMap_h
#define _EnvMap_h

#include <Image/tImage.h>

#include <vector>

using namespace std;

#define DMC_CUBE_MAP 1
#define DMC_PYRAMID_MAP 2
#define DMC_PARABOLOID_MAP 3
#define DMC_LATLONG_MAP 4
#define DMC_SPHERE_MAP 5

template<class _PixelType>
class EnvMap
{
private:
    tImage<_PixelType> Images[6];
    int MapType;

public:
    virtual template<_PixType> _PixType sample1(const t3Vector<float> Dir) const = 0; // Constant sample the image.
    virtual template<_PixType> _PixType sample2(const t3Vector<float> Dir) const = 0; // Bilinear sample the image.
    virtual template<_PixType> _PixType sample4(const t3Vector<float> Dir) const = 0; // Bicubic sample the image.

    virtual template<_PixType> _PixType IntegratedSample(const t3Vector<float> Dir, const float Expon) const = 0;

    virtual void Load(const char *fname) const = 0;
    
    // Detects type from filename. Won't modify number of channels.
    virtual void Save(const char *fname) const = 0;
};

#endif
