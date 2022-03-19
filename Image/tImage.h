//////////////////////////////////////////////////////////////////////
// tImage.h - Generic templated image class suitable for graphics and stuff.
//
// Copyright David K. McAllister, June 2001.
// Renovated in March 2007.

// This image class creates an image with an arbitrary pixel type.
// The pixel type is usually tPixel<channel type, NChan>.
// The pixel class must support a few things like tPixel does to be used as a tImage.

// There is a baseImage class that supports very generic access to the real image class.
// If you don't know what kind of image you'll be loading and want your code to work with the image
// in its native format, call LoadtImage() and work with a baseImage*.
// Use virtual functions and dynamic_cast<> to operate on the image.

// If your code is templated on the image type or works with only one image type, then call tImage.Load(),
// and the loaded image will be converted to your tImage's type.

// Copy constructor and assignment operator are templated on the argument type, creating a full cross-product of image
// conversion functions. MSVC is pretty savvy about not doing slow conversions if the pixel types actually match.

// ImageAlgorithms.cpp has a lot of algorithms that operate on templated tImage types.
// ImageLoadSave.cpp has most of the image loading/saving stuff. tLoadSave.cpp has the rest.

// To implement RGBE images I should make rgbePixel a subclass of tPixel<unsigned char, 4> and then specialize the constructor to f3Pixel.
// Then change the RGBE image load/save to use that.

// Naming convention: Container-related functions are all lower_case.
// Image-related functions are UpperCase.

#pragma once

#include "Image/LoadSaveParams.h"
#include "Image/RGBE.h"
#include "Image/tPixel.h"

#include <iostream>
#include <string>

// BaseImage is used to give commonality to all images. This lets them be passed around as a baseImage*.
// The functions in the base class should be sufficient to allow all operations that don't depend on the image's datatype.
// For example, calling OpenGL's glTexImage2D() should be doable with just the baseImage.
// Other things it's been wanted for: accessing members to print their values, get the pixel data pointer for loading and saving.
class baseImage {
public:
    //////////////////////////////////////////////////////////////////////
    // Info about a pixel

    // Number of channels in a pixel.
    virtual int chan_virtual() const = 0;

    // Size of one element of one pixel.
    virtual int size_element_virtual() const = 0;

    // Size of one pixel.
    virtual int size_pixel_virtual() const = 0;

    // Tells whether the elements are integer or floating point.
    virtual bool is_integer_virtual() const = 0;

    // Tells whether the elements are signed (true for float types).
    virtual bool is_signed_virtual() const = 0;

    //////////////////////////////////////////////////////////////////////
    // Info about the image

    // Width of the image.
    virtual int w_virtual() const = 0;

    // Height of the image.
    virtual int h_virtual() const = 0;

    // Number of pixels in the image.
    virtual int size_virtual() const = 0;

    // Size of image data in elements.
    virtual int size_els_virtual() const = 0;

    // Size of image data in bytes.
    virtual int size_bytes_virtual() const = 0;

    virtual ~baseImage() {};

    // A pointer to the pixel data, without knowing its kind
    virtual void* pv_virtual(const int i = 0) = 0;
    virtual const void* pv_virtual(const int i = 0) const = 0;

    // A pointer to the pixel data, without knowing its kind
    virtual const void* pv_virtual(const int x, const int y) const = 0;

    // Set whether this owns its raster and must delete it on destruction or not
    virtual void ownPix_virtual(const bool ownPix_) = 0;

    // Load the file, convert its type to this one, and fill this tImage
    virtual void Load(const std::string& fname, LoadSaveParams SP = LoadSaveParams()) = 0;

    // Save the image to a file
    virtual void Save(const std::string& fname, LoadSaveParams SP = LoadSaveParams()) const = 0; //{ ASSERT_RM(0, "Save not defined for this tImage type."); }

    // Make a copy of the actual image and return a pointer to its baseImage.
    virtual baseImage* Copy() const = 0;
};

// Load an image file into whatever kind of tImage is most appropriate.
// Returns a pointer to the baseImage. dynamic_cast will tell you what kind it really is.
// Throws a DMcError on failure.
baseImage* LoadtImage(const std::string& fname, LoadSaveParams SP = LoadSaveParams());

template <class Pixel_T> class tImage : public baseImage {
    Pixel_T* Pix; // The actual pixels.
    int wid, hgt;
    bool ownPix; // True if this owns Pix and must delete it, otherwise just abandons it.

public:
    //////////////////////////////////////////////////////////////////////
    // Constructors

    // Default constructor with optional dimensions.
    // Doesn't initialize the pixels.
    tImage(const int wid_ = 0, const int hgt_ = 0)
    {
        Pix = NULL;
        wid = hgt = 0;
        SetSize(wid_, hgt_, false);
    }

    // Constructor with dimensions that clears image to fillVal.
    // Initializes the pixels to fillVal.
    tImage(const int wid_, const int hgt_, const Pixel_T& fillVal)
    {
        Pix = NULL;
        wid = hgt = 0;
        SetSize(wid_, hgt_, false);
        fill(fillVal);
    }

    // Construct an image from a file.
    tImage(const std::string& fname)
    {
        Pix = NULL;
        wid = hgt = 0;
        Load(fname);
    }

    // Construct an image from an existing raster of Pixel_T.
    // This image becomes responsible for the delete [].
    tImage(Pixel_T* p, const int wid_, const int hgt_, const bool ownPix_)
    {
        Pix = NULL;
        SetImage(p, wid_, hgt_, ownPix_);
    }

private:
    // The internals of copying, which are shared between copy constructor, operator=, and Copy.
    template <class SrcPixel_T> void do_copy(const tImage<SrcPixel_T>& SrcIm)
    {
        if (SrcIm.size() > 0) {
            Pix = NULL;
            wid = hgt = 0;
            SetSize(SrcIm.w(), SrcIm.h(), false);
            int sz = SrcIm.size();
            for (int i = 0; i < sz; i++)
                (*this)[i] = static_cast<Pixel_T>(SrcIm[i]); // Doesn't call the tPixel copy constructor if Pixel_T == SrcPixel_T. Cool!
        } else {
            Pix = NULL;
            wid = hgt = 0;
            ownPix = false;
        }
    }

public:
    // Copy constructor.
    // Converts images from other pixel types to this pixel type.
    // Generates a full cross product of hard-wired image conversions.
    template <class SrcPixel_T> tImage(const tImage<SrcPixel_T>& SrcIm) { do_copy(SrcIm); }

    // Prevent member-wise copy if SrcPixel_T == Pixel_T.
    tImage(const tImage<Pixel_T>& SrcIm) { do_copy(SrcIm); }

    // Copy assignment.
    // Converts images from other pixel types to this pixel type.
    template <class SrcPixel_T> tImage<Pixel_T>& operator=(const tImage<SrcPixel_T>& SrcIm)
    {
        if ((void*)this != (void*)&SrcIm) {
            // std::cerr << "Conversion Assignment\n";
            do_copy(SrcIm);
        } else {
            // std::cerr << "Identical assignment\n";
        }
        return *this;
    }

    // Prevent member-wise assign if SrcPixel_T == Pixel_T.
    tImage<Pixel_T>& operator=(const tImage<Pixel_T>& SrcIm)
    {
        if ((void*)this != (void*)&SrcIm) {
            // std::cerr << "Conversion Assignment\n";
            do_copy(SrcIm);
        } else {
            // std::cerr << "Identical assignment\n";
        }
        return *this;
    }

    // Create a copy of this image and return it. This is a virtual function so that copies can be made when only the base class is known.
    tImage<Pixel_T>* Copy() const
    {
        // std::cerr << "tImage Copy()\n";
        tImage<Pixel_T>* Created = new tImage<Pixel_T>(*this);
        return Created;
    }

    // Destroy an image.
    ~tImage()
    {
        // std::cerr << "deleting tImage: " << this << ": " << Pix << std::endl;
        if (Pix && ownPix) delete[] Pix;
        Pix = NULL;
        // std::cerr << "done deleting tImage\n";
    }

    // Set whether this owns its raster and must delete it on destruction or not
    void ownPix_virtual(const bool ownPix_) { ownPix = ownPix_; }

    //////////////////////////////////////////////////////////////////////
    // Info about a pixel
    // These functions need to be const since they call other const functions.
    // The static functions don't require an actual this, but merely a concrete type.
    // The _virtual ones override pure virtual functions, and virtual functions can't be static.
    // The _virtual ones require a this pointer so that they can be used to determine type of a baseImage at run-time.
    // For each static function I also made a virtual function.

    // The type of the pixels in this image.
    typedef Pixel_T PixType;

    // Number of channels in a pixel. This requires that the pixel type support this type member.
    static int chan() { return Pixel_T::Chan; }
    int chan_virtual() const { return Pixel_T::Chan; }

    // Size of one element of one pixel.
    static int size_element() { return sizeof(typename Pixel_T::ElType); }
    int size_element_virtual() const { return sizeof(typename Pixel_T::ElType); }

    // Size of one pixel.
    static int size_pixel() { return sizeof(Pixel_T); }
    int size_pixel_virtual() const { return sizeof(Pixel_T); }

    // Tells whether the elements are integer or floating point.
    static bool is_integer() { return Pixel_T::is_integer; }
    bool is_integer_virtual() const { return Pixel_T::is_integer; }

    // Tells whether the elements are signed (true for float types).
    static bool is_signed() { return Pixel_T::is_signed; }
    bool is_signed_virtual() const { return Pixel_T::is_signed; }

    //////////////////////////////////////////////////////////////////////
    // Info about the image

    // Width of the image.
    int w() const { return wid; }
    int w_virtual() const { return wid; }

    // Height of the image.
    int h() const { return hgt; }
    int h_virtual() const { return hgt; }

    // Number of pixels in the image.
    int size() const { return wid * hgt; }
    int size_virtual() const { return wid * hgt; }

    // Size of image data in elements.
    int size_els() const { return size() * chan(); }
    int size_els_virtual() const { return size() * chan(); }

    // Size of image data in bytes.
    int size_bytes() const { return size() * size_pixel(); }
    int size_bytes_virtual() const { return size() * size_pixel(); }

    //////////////////////////////////////////////////////////////////////
    // Access functions

    // Return the index of this pixel in raster fashion.
    int ind(const int x, const int y) const { return y * w() + x; }

    // Returns const pixel x,y.
    const Pixel_T& operator()(const int x, const int y) const
    {
        ASSERT_D(x >= 0 && x < w());
        ASSERT_D(y >= 0 && y < h());
        return *pp(ind(x, y));
    }

    // Returns pixel x,y.
    Pixel_T& operator()(const int x, const int y)
    {
        ASSERT_D(x >= 0 && x < w());
        ASSERT_D(y >= 0 && y < h());
        return *pp(ind(x, y));
    }

    // Returns const pixel i.
    const Pixel_T& operator[](const int i) const
    {
        ASSERT_D(i >= 0 && i < size());
        return *pp(i);
    }

    // Returns pixel i.
    Pixel_T& operator[](const int i)
    {
        ASSERT_D(i >= 0 && i < size());
        return *pp(i);
    }

    // Returns a const pointer to this pixel.
    const Pixel_T* pp(const int x, const int y) const
    {
        ASSERT_D(x >= 0 && x < w());
        ASSERT_D(y >= 0 && y < h());
        ASSERT_D(Pix != NULL);
        return pp(ind(x, y));
    }

    // Returns a pointer to this pixel.
    Pixel_T* pp(const int x, const int y)
    {
        ASSERT_D(x >= 0 && x < w());
        ASSERT_D(y >= 0 && y < h());
        ASSERT_D(Pix != NULL);
        return pp(ind(x, y));
    }

    // Returns a const pointer to this pixel.
    const Pixel_T* pp(const int i = 0) const
    {
        ASSERT_D(i >= 0 && i < size());
        ASSERT_D(Pix != NULL);
        return &(Pix[i]);
    }

    // Returns a pointer to this pixel.
    Pixel_T* pp(const int i = 0)
    {
        ASSERT_D(i >= 0 && i < size());
        ASSERT_D(Pix != NULL);
        return &(Pix[i]);
    }

    // A const pointer to the pixel data, without knowing its kind
    const void* pv(const int i = 0) const
    {
        ASSERT_D(i >= 0 && i < size());
        return &(Pix[i]);
    }
    const void* pv_virtual(const int i = 0) const
    {
        ASSERT_D(i >= 0 && i < size());
        return &(Pix[i]);
    }

    // A pointer to the pixel data, without knowing its kind
    void* pv(const int i = 0)
    {
        ASSERT_D(i >= 0 && i < size());
        return &(Pix[i]);
    }
    void* pv_virtual(const int i = 0)
    {
        ASSERT_D(i >= 0 && i < size());
        return &(Pix[i]);
    }

    // A pointer to the pixel data, without knowing its kind
    const void* pv(const int x, const int y) const
    {
        ASSERT_D(x >= 0 && x < w() && y >= 0 && y < h());
        return pp(ind(x, y));
    }
    const void* pv_virtual(const int x, const int y) const
    {
        ASSERT_D(x >= 0 && x < w() && y >= 0 && y < h());
        return pp(ind(x, y));
    }

    // Paint pixel x,y if it has valid coords.
    void Set(const Pixel_T& p, const int x, const int y)
    {
        if (x >= 0 && x < w() && y >= 0 && y < h()) *pp(ind(x, y)) = p;
    }

    //////////////////////////////////////////////////////////////////////
    // Utility functions

    // Both channel-wise extrema at once.
    void GetMinMax(Pixel_T& cmin, Pixel_T& cmax) const
    {
        ASSERT_R(size() > 0);
        cmax = (*this)[0];
        cmin = (*this)[0];
        int sz = size();
        for (int i = 1; i < sz; i++) {
            cmax = Max(cmax, (*this)[i]);
            cmin = Min(cmin, (*this)[i]);
        }
    }

    // Max over all pixels for each channel.
    Pixel_T max_chan() const
    {
        ASSERT_R(size() > 0);
        Pixel_T cmax = (*this)[0];
        int sz = size();
        for (int i = 1; i < sz; i++) cmax = Max(cmax, (*this)[i]);
        return cmax;
    }

    // Min over all pixels for each channel.
    Pixel_T min_chan() const
    {
        ASSERT_R(size() > 0);
        Pixel_T cmin = (*this)[0];
        int sz = size();
        for (int i = 1; i < sz; i++) cmin = Min(cmin, (*this)[i]);
        return cmin;
    }

    // Returns the channel-wise sum of all the pixels.
    typename Pixel_T::MathPixType sum_chan() const
    {
        ASSERT_R(size() > 0);
        typename Pixel_T::MathPixType csum(0);
        int sz = size();
        for (int i = 0; i < sz; i++) csum += (*this)[i];
        return csum;
    }

    // Returns the channel-wise average of all the pixels.
    Pixel_T mean_chan() const { return sum_chan() / Pixel_T::MathPixType(size()); }

    // Clear the image to the given color.
    void fill(const Pixel_T p = Pixel_T(0))
    {
        int sz = size();
        for (int i = 0; i < sz; i++) (*this)[i] = p;
    }

    // Test whether the image is currently empty.
    bool empty() const
    {
        if (size() < 1) {
            ASSERT_D(Pix == NULL);
            return true;
        } else {
            ASSERT_D(Pix);
            return false;
        }
    }

    // Clear the image.
    void clear() { SetSize(0, 0, false); }

    // Change the size of this image.
    // Call with w=h=0 to clear the image.
    void SetSize(const int wid_, const int hgt_, const bool doFill = false)
    {
        wid = wid_;
        hgt = hgt_;
        if (w() <= 0 || h() <= 0) wid = hgt = 0;

        if (Pix && ownPix) delete[] Pix;
        Pix = NULL;
        ownPix = false;

        if (size() > 0) {
            // std::cerr << "Allocating\n";
            try {
                Pix = new Pixel_T[size()];
                ownPix = true;
            }
            catch (...) {
                ASSERT_RM(0, "memory alloc failed");
            }
            ASSERT_RM(Pix, "memory alloc failed");
            if (doFill) fill();
        }
        ASSERT_D(size() == 0 || Pix != NULL);
    }

    // Hooks the given raster of pixels into this image object.
    // This tImage owns the data if ownPix is true.
    // WARNING: Doesn't delete [] its previous data.
    // WARNING: Image data must really be this pixel type to be deleted.
    void SetImage(Pixel_T* p, const int wid_, const int hgt_, const bool ownPix_)
    {
        ASSERT_D(wid_ >= 0 && hgt_ >= 0);
        Pix = p;
        wid = wid_;
        hgt = hgt_;
        ownPix = ownPix_;

        // Sanity check.
        if (Pix == NULL) wid = hgt = 0;
        if (size() <= 0) Pix = NULL;
        ASSERT_D(size() == 0 || Pix != NULL);
    }

    void Load(const std::string& fname, LoadSaveParams SP = LoadSaveParams());       // Load an image from a file.
    void Save(const std::string& fname, LoadSaveParams SP = LoadSaveParams()) const; // Detects type from filename; won't modify this tImage.

    //////////////////////////////////////////////////////////////////////
    // Image Operators

    // With an image, with assign

    tImage<Pixel_T>& operator+=(const tImage<Pixel_T>& p)
    {
        ASSERT_R(size() == p.size());
        int sz = size();
        for (int i = 0; i < sz; i++) (*this)[i] += p[i];
        return *this;
    }
    tImage<Pixel_T>& operator-=(const tImage<Pixel_T>& p)
    {
        ASSERT_R(size() == p.size());
        int sz = size();
        for (int i = 0; i < sz; i++) (*this)[i] -= p[i];
        return *this;
    }
    tImage<Pixel_T>& operator*=(const tImage<Pixel_T>& p)
    {
        ASSERT_R(size() == p.size());
        int sz = size();
        for (int i = 0; i < sz; i++) (*this)[i] *= p[i];
        return *this;
    }
    tImage<Pixel_T>& operator/=(const tImage<Pixel_T>& p)
    {
        ASSERT_R(size() == p.size());
        int sz = size();
        for (int i = 0; i < sz; i++) (*this)[i] /= p[i];
        return *this;
    }

    // With a constant pixel, with assign

    tImage<Pixel_T>& operator+=(const Pixel_T& s)
    {
        int sz = size();
        for (int i = 0; i < sz; i++) (*this)[i] += s;
        return *this;
    }
    tImage<Pixel_T>& operator-=(const Pixel_T& s)
    {
        int sz = size();
        for (int i = 0; i < sz; i++) (*this)[i] -= s;
        return *this;
    }
    tImage<Pixel_T>& operator*=(const Pixel_T& s)
    {
        int sz = size();
        for (int i = 0; i < sz; i++) (*this)[i] *= s;
        return *this;
    }
    tImage<Pixel_T>& operator/=(const Pixel_T& s)
    {
        int sz = size();
        for (int i = 0; i < sz; i++) (*this)[i] /= s;
        return *this;
    }

#if 1
    // With a constant pixel
    tImage<Pixel_T> operator/(const Pixel_T& v)
    {
        tImage<Pixel_T> r(w(), h());
        int sz = size();
        for (int i = 0; i < sz; i++) r[i] = (*this)[i] / v;
        return r;
    }
#endif
};

// Apply an arbitrary function to each channel of each pixel and put the result in image r.
template <class Pixel_T, class Func_T> DMC_HDECL void ifunc(tImage<Pixel_T>& r, const tImage<Pixel_T>& p, Func_T fnc)
{
    r.SetSize(p.w(), p.h());

    int sz = p.size();
    for (int i = 0; i < sz; i++)
        for (int j = 0; j < Pixel_T::chan(); j++) r[i][j] = fnc(p[i][j]);
}

// Apply an arbitrary function to each channel of each pixel.
// Modifies the image in place.
template <class Pixel_T, class Func_T> DMC_HDECL void func(tImage<Pixel_T>& p, Func_T fnc)
{
    int sz = p.size();
    for (int i = 0; i < sz; i++)
        for (int j = 0; j < Pixel_T::chan(); j++) p[i][j] = fnc(p[i][j]);
}

// Unary minus.
template <class Pixel_T> DMC_HDECL tImage<Pixel_T> operator-(const tImage<Pixel_T>& p)
{
    ASSERT_R(Pixel_T::is_signed); // WARNING: Doesn't work for unsigned pixel types.

    tImage<Pixel_T> r(p.w(), p.h());
    int sz = p.size();
    for (int i = 0; i < sz; i++) r[i] = -p[i];
    return r;
}

// Equal.
template <class Pixel_T> DMC_HDECL bool operator==(const tImage<Pixel_T>& p1, const tImage<Pixel_T>& p2)
{
    if (p1.w() != p2.w() || p1.h() != p2.h()) return false;
    int sz = p1.size();
    for (int i = 0; i < sz; i++)
        if (p1[i] != p2[i]) return false;
    return true;
}

// Not equal.
template <class Pixel_T> DMC_HDECL bool operator!=(const tImage<Pixel_T>& p1, const tImage<Pixel_T>& p2) { return !(p1 == p2); }

// With two images

// Didn't do the suggested local copy and += for optimization.
// WARNING: After you use one of these you will probably assign it using operator=,
// which does a member-wise (unsafe) copy if src and dest pixel types match.
template <class Pixel_T> DMC_HDECL tImage<Pixel_T> operator+(const tImage<Pixel_T>& p1, const tImage<Pixel_T>& p2)
{
    ASSERT_R(p1.size() == p2.size());
    tImage<Pixel_T> r(p1.w(), p1.h());
    int sz = p1.size();
    for (int i = 0; i < sz; i++) r[i] = p1[i] + p2[i];
    return r;
}
template <class Pixel_T> DMC_HDECL tImage<Pixel_T> operator-(const tImage<Pixel_T>& p1, const tImage<Pixel_T>& p2)
{
    ASSERT_R(p1.size() == p2.size());
    tImage<Pixel_T> r(p1.w(), p1.h());
    int sz = p1.size();
    for (int i = 0; i < sz; i++) r[i] = p1[i] - p2[i];
    return r;
}
template <class Pixel_T> DMC_HDECL tImage<Pixel_T> operator*(const tImage<Pixel_T>& p1, const tImage<Pixel_T>& p2)
{
    ASSERT_R(p1.size() == p2.size());
    tImage<Pixel_T> r(p1.w(), p1.h());
    int sz = p1.size();
    for (int i = 0; i < sz; i++) r[i] = p1[i] * p2[i];
    return r;
}
// WARNING: Be careful for divide by zero.
template <class Pixel_T> DMC_HDECL tImage<Pixel_T> operator/(const tImage<Pixel_T>& p1, const tImage<Pixel_T>& p2)
{
    ASSERT_R(p1.size() == p2.size());
    tImage<Pixel_T> r(p1.w(), p1.h());
    int sz = p1.size();
    for (int i = 0; i < sz; i++) r[i] = p1[i] / p2[i];
    return r;
}

// With a left scalar and a right image

template <class Pixel_T> DMC_HDECL tImage<Pixel_T> operator+(const Pixel_T& v, const tImage<Pixel_T>& p)
{
    tImage<Pixel_T> r(p.w(), p.h());
    int sz = p.size();
    for (int i = 0; i < sz; i++) r[i] = v + p[i];
    return r;
}
template <class Pixel_T> DMC_HDECL tImage<Pixel_T> operator-(const Pixel_T& v, const tImage<Pixel_T>& p)
{
    tImage<Pixel_T> r(p.w(), p.h());
    int sz = p.size();
    for (int i = 0; i < sz; i++) r[i] = v - p[i];
    return r;
}
template <class Pixel_T> DMC_HDECL tImage<Pixel_T> operator*(const Pixel_T& v, const tImage<Pixel_T>& p)
{
    tImage<Pixel_T> r(p.w(), p.h());
    int sz = p.size();
    for (int i = 0; i < sz; i++) r[i] = v * p[i];
    return r;
}
template <class Pixel_T> DMC_HDECL tImage<Pixel_T> operator/(const Pixel_T& v, const tImage<Pixel_T>& p)
{
    tImage<Pixel_T> r(p.w(), p.h());
    int sz = p.size();
    for (int i = 0; i < sz; i++) r[i] = v / p[i];
    return r;
}

// With a right scalar and a left image

template <class Pixel_T> DMC_HDECL tImage<Pixel_T> operator+(const tImage<Pixel_T>& p, const Pixel_T& v)
{
    tImage<Pixel_T> r(p.w(), p.h());
    int sz = p.size();
    for (int i = 0; i < sz; i++) r[i] = p[i] + v;
    return r;
}
template <class Pixel_T> DMC_HDECL tImage<Pixel_T> operator-(const tImage<Pixel_T>& p, const Pixel_T& v)
{
    tImage<Pixel_T> r(p.w(), p.h());
    int sz = p.size();
    for (int i = 0; i < sz; i++) r[i] = p[i] - v;
    return r;
}
template <class Pixel_T> DMC_HDECL tImage<Pixel_T> operator*(const tImage<Pixel_T>& p, const Pixel_T& v)
{
    tImage<Pixel_T> r(p.w(), p.h());
    int sz = p.size();
    for (int i = 0; i < sz; i++) r[i] = p[i] * v;
    return r;
}

#if 0
// I can't remember why I commented this out. Maybe because I'm using a member version for some reason.
template<class Pixel_T>
DMC_HDECL tImage<Pixel_T> operator/(const tImage<Pixel_T> &p, const Pixel_T &v)
{
    tImage<Pixel_T> r(p.w(), p.h());
    int sz = p.size();
    for(int i=0; i<sz; i++) r[i] = p[i] / v;
    return r;
}
#endif

// Linearly interpolate between images p1 and p2.
// I.e., if weight==0, returns p1. If weight==1, returns p2.
template <class Pixel_T> DMC_HDECL tImage<Pixel_T> LinearInterp(const tImage<Pixel_T>& p1, const tImage<Pixel_T>& p2, typename Pixel_T::FloatMathType weight)
{
    ASSERT_R(p1.size() == p2.size());
    tImage<Pixel_T> r(p1.w(), p1.h());
    int sz = p1.size();
    for (int i = 0; i < p1.size(); i++) r[i] = LinearInterp(p1[i], p2[i], weight);
    return r;
}

// Pixel-wise max.
template <class Pixel_T> DMC_HDECL tImage<Pixel_T> Max(const tImage<Pixel_T>& p1, const tImage<Pixel_T>& p2)
{
    ASSERT_R(p1.size() == p2.size());
    tImage<Pixel_T> r(p1.w(), p1.h());
    int sz = p1.size();
    for (int i = 0; i < p1.size(); i++) r[i] = Max(p1[i], p2[i]);
    return r;
}

// Pixel-wise min.
template <class Pixel_T> DMC_HDECL tImage<Pixel_T> Min(const tImage<Pixel_T>& p1, const tImage<Pixel_T>& p2)
{
    ASSERT_R(p1.size() == p2.size());
    tImage<Pixel_T> r(p1.w(), p1.h());
    int sz = p1.size();
    for (int i = 0; i < p1.size(); i++) r[i] = Min(p1[i], p2[i]);
    return r;
}

// Pixel-wise absolute value.
template <class Pixel_T> DMC_HDECL tImage<Pixel_T> Abs(const tImage<Pixel_T>& p)
{
    tImage<Pixel_T> r(p.w(), p.h());
    int sz = p.size();
    for (int i = 0; i < sz; i++) r[i] = Abs(p[i]);
    return r;
}

// Flip this image vertically in place.
// This really should be in ImageAlgorithms, but I used to use it a lot.
template <class Pixel_T> DMC_HDECL void VFlip(tImage<Pixel_T>& p)
{
    const int wid = p.w(), hgt = p.h();

    for (int y = 0; y < hgt / 2; y++) {
        for (int x = 0; x < wid; x++) {
            Pixel_T tmp = p(x, y);
            p(x, y) = p(x, hgt - y - 1);
            p(x, hgt - y - 1) = tmp;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
// Specializations

#ifdef DMC_USE_HALF_FLOAT
typedef tImage<h1Pixel> h1Image;
typedef tImage<h2Pixel> h2Image;
typedef tImage<h3Pixel> h3Image;
typedef tImage<h4Pixel> h4Image;
#endif

typedef tImage<f1Pixel> f1Image;
typedef tImage<f2Pixel> f2Image;
typedef tImage<f3Pixel> f3Image; // Overload this for tone mapping.
typedef tImage<f4Pixel> f4Image;

typedef tImage<uc1Pixel> uc1Image;
typedef tImage<uc2Pixel> uc2Image;
typedef tImage<uc3Pixel> uc3Image;
typedef tImage<uc4Pixel> uc4Image;

typedef tImage<us1Pixel> us1Image;
typedef tImage<us2Pixel> us2Image;
typedef tImage<us3Pixel> us3Image;
typedef tImage<us4Pixel> us4Image;

typedef tImage<ss1Pixel> ss1Image;
typedef tImage<ss2Pixel> ss2Image;
typedef tImage<ss3Pixel> ss3Image;
typedef tImage<ss4Pixel> ss4Image;

typedef tImage<ui1Pixel> ui1Image;
typedef tImage<ui2Pixel> ui2Image;
typedef tImage<ui3Pixel> ui3Image;
typedef tImage<ui4Pixel> ui4Image;

typedef ui1Image zImage;
typedef tImage<rgbePixel> rgbeImage; // Overload this to get a constructor.
