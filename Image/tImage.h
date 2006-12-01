//////////////////////////////////////////////////////////////////////
// tImage.h - Generic templated image class suitable for graphics and stuff.
//
// Copyright David K. McAllister, June 2001.

// Have one file with named pixel types as derivatives: uc3Pixel, Color == f3Pixel, us3Pixel...
// One generic image class.
// All pixels must be generated with tPixel: tPixel<unsigned char, 1> is a one-channel image.
// How to handle overflow of += operator, etc?

// Design goals:
// * Have a ucImage that's as much like the current one as possible
// * Allow app coding of image operations wo/ knowledge of num channels
// * Allow loading and saving - return an image of the file kind, convert it to what I want.
// * Allow images of RGBE.
// * Allow operations on images to be generic and stored in one file in DMcTools

// TO DO:
// Make LoadXXX and SaveXXX route from ucImage to ImageLoadSave.
// Do similar things for tImage. How to convert num channels?
// Convert Quant to parameterized pixels: Will it use tImage?
// Implement the Kodak usImage thing.

// Approaches:
/*
Accepted:
Non-parent class (ucImage) with Pix, wid, hgt, chan, Load, Save for uchar.
ucImage has a cast to every tImage<ucNPixel> type keyed off of chan variable.
Used for all unsigned char images only.
Float and other images don't use this mechanism.
tImage<> has a constructor from any kind of image.
How to parameterize this constructor for tone mapping?
- make f3Image a subclass and overload the constructor.
What about converting to/from RGBE?
- Make rgbeImage a subclass and overload the constructor.
File of filter and other code, all templated: ImageAlgorithms.cpp

 Rejected:
 Parent class (dmcImage) with Pix, wid, hgt
 Separate Load and Save for everything.
 Makes a tImage of the right kind and returns it as pointer to dmcImage
 Use dynamic cast to get the type you want.
 ucImage is a derived class of dmcImage
*/

/*
This class is made both for compatibility with all my existing code,
and for loading and saving unsigned char images.

 Non-parent class (ImageLoadSave) with Pix, wid, hgt, chan, Load, Save for uchar.
 ImageLoadSave has a cast to every tImage<ucNPixel> type keyed off of chan variable.
 Used for all unsigned char images only.
 Float and other images don't use this mechanism.
 tImage<> has a constructor from any kind of image.
 How to parameterize this constructor for tone mapping?
 - make f3Image a subclass and overload the constructor.
 What about converting to/from RGBE?
 - Make rgbeImage a subclass and overload the constructor.
 To do Load/Save/tImage(char *fname) for tImage,
 use specializations to load and convert.
 
  Spiderman's Master Plan:
  Keep ImageLoadSave exactly as it now is.
  Use ImageLoadSave to Load and Save in tImage.
  
   Alternative:
   Make a thin ImageLoadSave class. Re-implement ImageLoadSave on top of it,
   and use it for tImage.
*/

// Naming convention: Container-related functions are all lower_case.
// Image-related functions are UpperCase.

#ifndef _tImage_h
#define _tImage_h

#include <Image/Pixel.h>
#include <Image/RGBE.h>

// This class is used to give commonality to all images.
// This lets them be passed around as a baseImage *.
// The functions defined here basically allow you to do your own run-time type checking.
class baseImage
{
public:
    // Maybe I shouldn't make these pure virtual so that images of built-in types work.
    
    //////////////////////////////////////////////////////////////////////
    // Info about a pixel
    
    // Number of channels in a pixel.
    virtual int chan() const = 0;
    
    // Size of one element of one pixel.
    virtual int size_element() const = 0;
    
    // Size of one pixel.
    virtual int size_pixel() const = 0;
    
    // Tells whether the elements are floating point.
    virtual bool is_integer() const = 0;
    
    // Tells whether the elements are signed (true for float types).
    virtual bool is_signed() const = 0;
    
    //////////////////////////////////////////////////////////////////////
    // Info about the image
    
    // Width of the image.
    virtual int w() const = 0;
    
    // Height of the image.
    virtual int h() const = 0;
    
    // Number of pixels in the image.
    virtual int size() const = 0;
    
    // Size of image data in elements.
    virtual int size_els() const = 0;
    
    // Size of image data in bytes.
    virtual int size_bytes() const = 0;

    virtual ~baseImage() {}
};

// For loading an image into whatever kind of tImage is most appropriate.
// Defined in ImageLoadSave.cpp.
// XXX Broken: Points to image data, not image struct.
// Also, how am I supposed to tell what kind it loaded, anyway?
baseImage *LoadtImage(const char *fname);

template<class _PixType>
class tImage : public baseImage
{
    _PixType *Pix; // To the actual pixels.
    int wid, hgt;
    
public:
    //////////////////////////////////////////////////////////////////////
    // Constructors
    
    // Default constructor with optional dimensions.
    // Optionally clear image to 0.
    tImage(const int _wid = 0, const int _hgt = 0, const bool doFill = false)
    {
        SetSize(_wid, _hgt, doFill, false);
    }
    
    // Copy constructor.
    // Converts images from other pixel types to this pixel type.
    // Generates a full cross product of hard-wired image conversions.
    template<class _PixTypeSrc>
        tImage(const tImage<_PixTypeSrc> &SrcIm)
    {
        // cerr << "copy\n";
        if(SrcIm.size() > 0) {
            SetSize(SrcIm.w(), SrcIm.h(), false, false);
            for(int i=0; i<SrcIm.size(); i++)
                (*this)[i] = static_cast<_PixType>(SrcIm[i]);
        } else {
            Pix = NULL;
            wid = hgt = 0;
        }
    }
    
    // Construct an image from a file.
    tImage(const char *fname)
    {
        Pix = NULL;
        wid = hgt = 0;
        Load(fname);
    }
    
    // Construct an image from an existing raster of _PixType.
    // This image becomes responsible for the delete [].
    tImage(_PixType *p, const int _wid, const int _hgt)
    {
        Pix = NULL;
        SetImage(p, _wid, _hgt);
    }
    
#if 1
    // Copy assignment.
    tImage<_PixType> &operator=(const tImage<_PixType> &SrcIm)
        //template<class _PixTypeSrc>
        // tImage<_PixType> &operator=(const tImage<_PixTypeSrc> &SrcIm)
    {
        // cerr << "Assignment\n";
        if((void*)this != (void*)&SrcIm) {
            if(SrcIm.size() > 0) {
                SetSize(SrcIm.w(), SrcIm.h(), false, false);
                for(int i=0; i<SrcIm.size(); i++)
                    (*this)[i] = static_cast<_PixType>(SrcIm[i]);
            } else {
                Pix = NULL;
                wid = hgt = 0;
            }
        } else {
            // cerr << "Fuh assignment\n";
        }
        return *this;
    }
#endif
    
    // Assign all pixels to value s.
    tImage<_PixType> &operator=(const _PixType &s)
    {
        fill(s);
        return *this;
    }
    
    // Destroy an image.
    ~tImage()
    {
        // cerr << "deleting tImage: " << this << ": " << Pix << endl;
        if(Pix) delete [] Pix;
    }
    
    //////////////////////////////////////////////////////////////////////
    // Info about a pixel
    // These should all be static since they don't require an actual this;
    // but merely a concrete type.
    // However, they need to be const since they call other const functions.
    // Also, they can't be static since they override pure virtual functions,
    // and virtual functions can't be static.
    // Decided to not make them static. They require a this pointer so that
    // they can be used to determine type of a baseImage at run-time.
    
    // The type of the pixels in this image.
    typedef _PixType PixType;
    
    // Number of channels in a pixel.
    int chan() const
    {
        // This requires that the pixel type support this function.
        return _PixType::chan();
    }
    
    // Size of one element of one pixel.
    int size_element() const
    {
        return sizeof(typename _PixType::ElType);
    }
    
    // Size of one pixel.
    int size_pixel() const
    {
        return sizeof(_PixType);
    }
    
    // Tells whether the elements are floating point.
    bool is_integer() const
    {
        return _PixType::is_integer();
    }
    
    // Tells whether the elements are signed (true for float types).
    bool is_signed() const
    {
        return false; // XXX _PixType::is_signed();
    }
    
    //////////////////////////////////////////////////////////////////////
    // Info about the image
    
    // Width of the image.
    int w() const
    {
        return wid;
    }
    
    // Height of the image.
    int h() const
    {
        return hgt;
    }
    
    // Number of pixels in the image.
    int size() const
    {
        return w() * h();
    }
    
    // Size of image data in elements.
    int size_els() const
    {
        return size() * chan();
    }
    
    // Size of image data in bytes.
    int size_bytes() const
    {
        return size() * size_pixel();
    }
    
    //////////////////////////////////////////////////////////////////////
    // Access functions
    
    // Return the index of this pixel in raster fashion.
    int ind(const int x, const int y) const
    {
        return y*w() + x;
    }
    
    // Returns const pixel x,y.
    const _PixType & operator() (const int x, const int y) const
    {
        ASSERT_D(x>=0 && x<w());
        ASSERT_D(y>=0 && y<h());
        return *pp(ind(x,y));
    }
    
    // Returns pixel x,y.
    _PixType & operator() (const int x, const int y)
    {
        ASSERT_D(x>=0 && x<w());
        ASSERT_D(y>=0 && y<h());
        return *pp(ind(x,y));
    }
    
    // Returns const pixel i.
    const _PixType & operator[] (const int i) const
    {
        ASSERT_D(i>=0 && i<size());
        return *pp(i);
    }
    
    // Returns pixel i.
    _PixType & operator[] (const int i)
    {
        ASSERT_D(i>=0 && i<size());
        return *pp(i);
    }
    
    // Returns a const pointer to this pixel.
    const _PixType *pp(const int x, const int y) const
    {
        ASSERT_D(x>=0 && x<w());
        ASSERT_D(y>=0 && y<h());
        return pp(ind(x,y));
    }
    
    // Returns a pointer to this pixel.
    _PixType *pp(const int x, const int y)
    {
        ASSERT_D(x>=0 && x<w());
        ASSERT_D(y>=0 && y<h());
        return pp(ind(x,y));
    }
    
    // Returns a const pointer to this pixel.
    const _PixType *pp(const int i=0) const
    {
        ASSERT_D(i>=0 && i<size());
        return &(Pix[i]);
    }
    
    // Returns a pointer to this pixel.
    _PixType *pp(const int i=0)
    {
        ASSERT_D(i>=0 && i<size());
        return &(Pix[i]);
    }
        
    // Paint pixel x,y if it has valid coords.
    void Set(const _PixType &p, const int x, const int y)
    {
        if(x>=0 && x<w() && y>=0 && y<h())
            *pp(ind(x,y)) = p;
    }

    //////////////////////////////////////////////////////////////////////
    // Utility functions
    
    // Both channel-wise extrema at once.
    void GetMinMax(_PixType &cmin, _PixType &cmax) const
    {
        ASSERT_R(size() > 0);
        cmax = (*this)[0];
        cmin = (*this)[0];
        for(int i=1; i<size(); i++) {
            cmax = Max(cmax, (*this)[i]);
            cmin = Min(cmin, (*this)[i]);
        }
    }
    
    // Max over all pixels for each channel.
    _PixType max_chan() const
    {
        ASSERT_R(size() > 0);
        _PixType cmax = (*this)[0];
        for(int i=1; i<size(); i++) cmax = Max(cmax, (*this)[i]);
        return cmax;
    }
    
    // Min over all pixels for each channel.
    _PixType min_chan() const
    {
        ASSERT_R(size() > 0);
        _PixType cmin = (*this)[0];
        for(int i=1; i<size(); i++) cmin = Min(cmin, (*this)[i]);
        return cmin;
    }
    
    // Returns the channel-wise sum of all the pixels.
    // WARNING: This is likely to overflow for fixed point pixel types.
    _PixType sum_chan() const
    {
        ASSERT_R(size() > 0);
        _PixType csum = 0;
        for(int i=0; i<size(); i++) csum += (*this)[i];
        return csum;
    }
    
    // Returns the channel-wise average of all the pixels.
    // WARNING: This is likely to overflow for fixed point pixel types.
    _PixType mean_chan() const
    {
        return sum_chan() / _PixType(size());
    }
    
    // Transfer contents of Img to this image and vice-versa.
    // This is a fast way to assign an image that avoids the huge
    // memory allocate and copy.
    void swap(tImage<_PixType> &Img)
    {
        // Squirrel away my old stuff.
        int myw = w();
        int myh = h();
        _PixType *MyPix = pp();
        
        // Take on Img's stuff.
        SetImage(Img.pp(), Img.w(), Img.h());
        
        // Give Img my stuff.
        Img.SetImage(MyPix, myw, myh);
    }
    
    // Clear the image to the given color.
    void fill(const _PixType p = _PixType(0))
    {
        for(int i = 0; i < size(); i++) (*this)[i] = p;
    }
    
    // Test whether the image is currently empty.
    bool empty()
    {
        if(size() < 1) {
            ASSERT(Pix == NULL);
            return true;
        } else {
            ASSERT(Pix);
            return false;
        }
    }
    
    // Change the size of this image.
    // Call with no args to wipe the image to empty.
    void SetSize(const int _wid = 0, const int _hgt = 0,
        const bool doFill = false, const bool doDel = true)
    {
        // cerr << this << ": " << w() << "x" << h() << " -> " << _wid << "x" << _hgt << endl;
        wid = _wid; hgt = _hgt;
        if(w() <= 0 || h() <= 0) wid = hgt = 0;
        
        if(Pix && doDel) delete [] Pix;
        Pix = NULL;
        
        if(size() > 0) {
            Pix = new _PixType[size()];
            ASSERT_RM(Pix, "memory alloc failed");
            // cerr << "Pix = " << Pix << endl;
            if(doFill) fill();
        }
    }
    
    // Hooks the given raster of pixels into this image object.
    // This image class now owns the data and must delete [] it.
    // WARNING: Doesn't delete [] its previous data (so that swap() works).
    // WARNING: Image data must really be this pixel type to be deleted.
    // Call with no args to detach the current raster without deleting it.
    // This is how to leave a hijacked image intact.
    void SetImage(_PixType *p = NULL, const int _wid = 0, const int _hgt = 0)
    {
        Pix = p;
        wid = _wid;
        hgt = _hgt;
        
        // Sanity check.
        if(Pix == NULL) wid = hgt = 0;
        if(size() <= 0) Pix = NULL;
    }
    
    // Flip this image vertically.
    void VFlip()
    {
        for(int y=0; y<h()/2; y++) {
            for(int x = 0; x < w(); x++) {
                _PixType tmp = (*this)(x,y);
                (*this)(x,y) = (*this)(x,hgt-y-1);
                (*this)(x,hgt-y-1) = tmp;
            }
        }
    }
    
    // Copy channel number src_ch of image SrcIm to channel dest_ch of this image.
    // Things work fine if SrcIm is *this.
    // Have to template it on the incoming pixel type so that it can be a different number
    // of channels than the output image.
    // Channel numbers start with 0.
    template<class _PixTypeSrc>
        void CopyChan(const tImage<_PixTypeSrc> &SrcIm, const int src_ch, const int dest_ch)
    {
        ASSERT_R(w() == SrcIm.w() && h() == SrcIm.h());
        ASSERT_R(src_ch < SrcIm.chan() && dest_ch < chan());
        
        for(int i=0; i<size(); i++) (*this)[i][dest_ch] = SrcIm[i][src_ch];
    }
    
    // Copy a rectangle from SrcIm of size bwid x bhgt with upper-right
    // corner srcx,srcy to upper-right corner dstx,dsty in *this.
    // The two images may be different types.
    // The images may be the same, but the result is undefined if the quads overlap.
    template<class _PixTypeSrc>
        void CopyChunk(const tImage<_PixTypeSrc> &SrcIm, const int srcx, const int srcy,
        const int dstx, const int dsty, const int bwid, const int bhgt)
    {
        if(bwid <= 0 || bhgt <= 0 || size() <= 0 || SrcIm.size() <= 0)
            return;
        
        // Compute transformation from src to dst.
        int stodx = dstx - srcx;
        int stody = dsty - srcy;

        // Clip box against src.
        int bw = bwid;
        if(bw+srcx >= SrcIm.w()) bw = SrcIm.w() - srcx;
        int bh = bhgt;
        if(bh+srcy >= SrcIm.h()) bh = SrcIm.h() - srcy;
        
        int bsx = srcx;
        if(bsx < 0) {bw += bsx; bsx = 0;}
        int bsy = srcy;
        if(bsy < 0) {bh += bsy; bsy = 0;}

        // Transform box to dst space.
        int bdx = bsx + stodx;
        int bdy = bsy + stody;

        // Clip b against dst.
        if(bw+dstx >= w()) bw = w() - dstx;
        if(bh+dsty >= h()) bh = h() - dsty;
        
        if(bdx < 0) {bw += bdx; bdx = 0;}
        if(bdy < 0) {bh += bdy; bdy = 0;}

        if(bw <= 0 || bh <= 0)
            return;

        // Transform box back to src space.
        bsx = bdx - stodx;
        bsy = bdy - stody;

        int srcxmax = bsx + bw;
        int srcymax = bsy + bh;

        for(int sy = bsy, dy = bdy; sy < srcymax; sy++, dy++)
            for(int sx = bsx, dx = bdx; sx < srcxmax; sx++, dx++)
                (*this)(dx, dy) = static_cast<_PixType>(SrcIm(sx, sy));
    }

    // Copy a rectangle from SrcIm of size bwid x bhgt with upper-right
    // corner srcx,srcy to upper-right corner dstx,dsty in *this.
    // The two images may be different types.
    // Uses a color key to copy non-key pixels.
    // The images may be the same, but the result is undefined if the quads overlap.
    template<class _PixTypeSrc>
        void KeyedBlit(const tImage<_PixTypeSrc> &SrcIm, const int srcx, const int srcy,
        const int dstx, const int dsty, const int bwid, const int bhgt, _PixTypeSrc Key)
    {
        if(bwid <= 0 || bhgt <= 0 || size() <= 0 || SrcIm.size() <= 0)
            return;
        
        // Compute transformation from src to dst.
        int stodx = dstx - srcx;
        int stody = dsty - srcy;

        // Clip box against src.
        int bw = bwid;
        if(bw+srcx >= SrcIm.w()) bw = SrcIm.w() - srcx;
        int bh = bhgt;
        if(bh+srcy >= SrcIm.h()) bh = SrcIm.h() - srcy;
        
        int bsx = srcx;
        if(bsx < 0) {bw += bsx; bsx = 0;}
        int bsy = srcy;
        if(bsy < 0) {bh += bsy; bsy = 0;}

        // Transform box to dst space.
        int bdx = bsx + stodx;
        int bdy = bsy + stody;

        // Clip b against dst.
        if(bw+dstx >= w()) bw = w() - dstx;
        if(bh+dsty >= h()) bh = h() - dsty;
        
        if(bdx < 0) {bw += bdx; bdx = 0;}
        if(bdy < 0) {bh += bdy; bdy = 0;}

        if(bw <= 0 || bh <= 0)
            return;

        // Transform box back to src space.
        bsx = bdx - stodx;
        bsy = bdy - stody;

        int srcxmax = bsx + bw;
        int srcymax = bsy + bh;

        for(int sy = bsy, dy = bdy; sy < srcymax; sy++, dy++)
            for(int sx = bsx, dx = bdx; sx < srcxmax; sx++, dx++)
                if(SrcIm(sx, sy) != Key)
                    (*this)(dx, dy) = static_cast<_PixType>(SrcIm(sx, sy));
    }
    
    // Load an image from a file.
    void Load(const char *fname)
    {
        // You actually use the specialized versions of this function
        // at the end of this file.
        ASSERT_R(0);
    }
    
    // Detects type from filename. Won't modify number of channels.
    void Save(const char *fname) const
    {
        // You actually use the specialized versions of this function
        // at the end of this file.
        ASSERT_R(0);
    }
    
    //////////////////////////////////////////////////////////////////////
    // Image Operators
    
    // With an image, with assign
    
    tImage<_PixType> &operator+=(const tImage<_PixType> &p)
    {
        ASSERT_R(size() == p.size());
        for(int i=0; i<size(); i++) (*this)[i] += p[i];
        return *this; 
    }
    tImage<_PixType> &operator-=(const tImage<_PixType> &p)
    {
        ASSERT_R(size() == p.size());
        for(int i=0; i<size(); i++) (*this)[i] -= p[i];
        return *this; 
    }
    tImage<_PixType> &operator*=(const tImage<_PixType> &p)
    {
        ASSERT_R(size() == p.size());
        for(int i=0; i<size(); i++) (*this)[i] *= p[i];
        return *this; 
    }
    tImage<_PixType> &operator/=(const tImage<_PixType> &p)
    {
        ASSERT_R(size() == p.size());
        for(int i=0; i<size(); i++) (*this)[i] /= p[i];
        return *this; 
    }
    
    // With a constant pixel, with assign
    
    tImage<_PixType> &operator+=(const _PixType &s)
    {
        for(int i=0; i<size(); i++) (*this)[i] += s;
        return *this; 
    }
    tImage<_PixType> &operator-=(const _PixType &s)
    {
        for(int i=0; i<size(); i++) (*this)[i] -= s;
        return *this; 
    }
    tImage<_PixType> &operator*=(const _PixType &s)
    {
        for(int i=0; i<size(); i++) (*this)[i] *= s;
        return *this; 
    }
    tImage<_PixType> &operator/=(const _PixType &s)
    {
        for(int i=0; i<size(); i++) (*this)[i] /= s;
        return *this; 
    }
#if 1
    // With a constant pixel
    
    tImage<_PixType> operator/(const _PixType &v)
    {
        tImage<_PixType> r(w(), h());
        for(int i=0; i<size(); i++) r[i] = (*this)[i] / v;
        return r; 
    }
#endif
};

#if 0
// Apply an arbitrary function to each pixel.
template<class _PixType, class _Pred>
tImage<_PixType> func(const tImage<_PixType> &p, _Pred _fnc)
{
    _PixType r;
    for(int i=0; i<p.size(); i++) r[i] = _fnc(p[i]);
    return r; 
}
#endif

#if 1
// Apply an arbitrary function to each channel of each pixel.
template<class _PixType, class _Pred>
void ifunc(tImage<_PixType> &r, const tImage<_PixType> &p, _Pred _fnc)
{
    r.SetSize(p.w(), p.h());

    for(int i=0; i<p.size(); i++) 
        for(int j=0; j<_PixType::chan(); j++) r[i][j] = _fnc(p[i][j]);
}

// Apply an arbitrary function to each channel of each pixel.
// Modifies the image in place.
template<class _PixType, class _Pred>
void func(tImage<_PixType> &p, _Pred _fnc)
{
    for(int i=0; i<p.size(); i++) 
        for(int j=0; j<_PixType::chan(); j++) p[i][j] = _fnc(p[i][j]);
}
#endif

// Unary minus.
// WARNING: This is probably bad for unsigned pixel types.
template<class _PixType>
tImage<_PixType> operator-(const tImage<_PixType> &p)
{
    tImage<_PixType> r(p.w(), p.h());
    for(int i=0; i<p.size(); i++) r[i] = -p[i];
    return r; 
}

// Equal. Doesn't use epsilon.
template<class _PixType>
bool operator==(const tImage<_PixType> &p1, const tImage<_PixType> &p2)
{
    if(p1.w() != p2.w() || p1.h() != p2.h())
        return false;
    for(int i=0; i<p1.size(); i++)
        if(p1[i] != p2[i])
            return false;
        return true;
}

// Not equal. Doesn't use epsilon.
template<class _PixType>
bool operator!=(const tImage<_PixType> &p1, const tImage<_PixType> &p2)
{
    return !(p1 == p2);
}

// With two images

// Didn't do the suggested local copy and += for optimization.
// WARNING: These have a memory error on MSVC. Don't use them.
template<class _PixType>
tImage<_PixType> operator+(const tImage<_PixType> &p1, const tImage<_PixType> &p2)
{
    ASSERT_R(p1.size() == p2.size());
    tImage<_PixType> r(p1.w(), p1.h());
    for(int i=0; i<p1.size(); i++) r[i] = p1[i] + p2[i];
    return r; 
}
template<class _PixType>
tImage<_PixType> operator-(const tImage<_PixType> &p1, const tImage<_PixType> &p2)
{
    ASSERT_R(p1.size() == p2.size());
    tImage<_PixType> r(p1.w(), p1.h());
    for(int i=0; i<p1.size(); i++) r[i] = p1[i] - p2[i];
    return r; 
}
template<class _PixType>
tImage<_PixType> operator*(const tImage<_PixType> &p1, const tImage<_PixType> &p2)
{
    ASSERT_R(p1.size() == p2.size());
    tImage<_PixType> r(p1.w(), p1.h());
    for(int i=0; i<p1.size(); i++) r[i] = p1[i] * p2[i];
    return r; 
}
// WARNING: Be careful for divide by zero.
template<class _PixType>
tImage<_PixType> operator/(const tImage<_PixType> &p1, const tImage<_PixType> &p2)
{
    ASSERT_R(p1.size() == p2.size());
    tImage<_PixType> r(p1.w(), p1.h());
    for(int i=0; i<p1.size(); i++) r[i] = p1[i] / p2[i];
    return r; 
}

// With a left scalar and a right image

template<class _PixType>
tImage<_PixType> operator+(const _PixType &v, const tImage<_PixType> &p)
{
    tImage<_PixType> r(p.w(), p.h());
    for(int i=0; i<p.size(); i++) r[i] = v + p[i];
    return r; 
}
template<class _PixType>
tImage<_PixType> operator-(const _PixType &v, const tImage<_PixType> &p)
{
    tImage<_PixType> r(p.w(), p.h());
    for(int i=0; i<p.size(); i++) r[i] = v - p[i];
    return r; 
}
template<class _PixType>
tImage<_PixType> operator*(const _PixType &v, const tImage<_PixType> &p)
{
    tImage<_PixType> r(p.w(), p.h());
    for(int i=0; i<p.size(); i++) r[i] = v * p[i];
    return r; 
}
template<class _PixType>
tImage<_PixType> operator/(const _PixType &v, const tImage<_PixType> &p)
{
    tImage<_PixType> r(p.w(), p.h());
    for(int i=0; i<p.size(); i++) r[i] = v / p[i];
    return r; 
}

// With a right scalar and a left image

template<class _PixType>
tImage<_PixType> operator+(const tImage<_PixType> &p, const _PixType &v)
{
    tImage<_PixType> r(p.w(), p.h());
    for(int i=0; i<p.size(); i++) r[i] = p[i] + v;
    return r; 
}
template<class _PixType>
tImage<_PixType> operator-(const tImage<_PixType> &p, const _PixType &v)
{
    tImage<_PixType> r(p.w(), p.h());
    for(int i=0; i<p.size(); i++) r[i] = p[i] - v;
    return r; 
}
template<class _PixType>
tImage<_PixType> operator*(const tImage<_PixType> &p, const _PixType &v)
{
    tImage<_PixType> r(p.w(), p.h());
    for(int i=0; i<p.size(); i++) r[i] = p[i] * v;
    return r; 
}
#if 0
template<class _PixType>
tImage<_PixType> operator/(const tImage<_PixType> &p, const _PixType &v)
{
    tImage<_PixType> r(p.w(), p.h());
    for(int i=0; i<p.size(); i++) r[i] = p[i] / v;
    return r; 
}
#endif
// Linearly interpolate between pixels p1 and p2.
// If weight==0, returns p1. If weight==1, returns p2.
// WARNING: This should get overloaded for unsigned _ElTypes.
template<class _PixType>
tImage<_PixType> Interpolate(const tImage<_PixType> &p1, const tImage<_PixType> &p2,
                             _PixType weight)
{
    ASSERT_R(p1.size() == p2.size());
    tImage<_PixType> r(p1.w(), p1.h());
    for(int i=0; i<p1.size(); i++) r[i] = p1[i] + (p2[i] - p1[i]) * weight;
    return r; 
}

// Pixel-wise max.
template<class _PixType>
tImage<_PixType> Max(const tImage<_PixType> &p1, const tImage<_PixType> &p2)
{
    ASSERT_R(p1.size() == p2.size());
    tImage<_PixType> r(p1.w(), p1.h());
    for(int i=0; i<p1.size(); i++) r[i] = Max(p1[i], p2[i]);
    return r;
}

// Pixel-wise min.
template<class _PixType>
tImage<_PixType> Min(const tImage<_PixType> &p1, const tImage<_PixType> &p2)
{
    ASSERT_R(p1.size() == p2.size());
    tImage<_PixType> r(p1.w(), p1.h());
    for(int i=0; i<p1.size(); i++) r[i] = Min(p1[i], p2[i]);
    return r;
}

// Pixel-wise absolute value.
template<class _PixType>
tImage<_PixType> Abs(const tImage<_PixType> &p)
{
    tImage<_PixType> r(p.w(), p.h());
    for(int i=0; i<p.size(); i++) r[i] = Abs(p[i]);
    return r;
}

//////////////////////////////////////////////////////////////////////////////
// Specializations

typedef tImage<uc1Pixel> uc1Image;
typedef tImage<uc2Pixel> uc2Image;
typedef tImage<uc3Pixel> uc3Image;
typedef tImage<uc4Pixel> uc4Image;

typedef tImage<us1Pixel> us1Image;
typedef tImage<us2Pixel> us2Image;
typedef tImage<us3Pixel> us3Image;
typedef tImage<us4Pixel> us4Image;

typedef tImage<s1Pixel> s1Image;
typedef tImage<s2Pixel> s2Image;
typedef tImage<s3Pixel> s3Image;
typedef tImage<s4Pixel> s4Image;

typedef tImage<ui1Pixel> ui1Image;
typedef tImage<ui2Pixel> ui2Image;
typedef tImage<ui3Pixel> ui3Image;
typedef tImage<ui4Pixel> ui4Image;

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

typedef ui1Image zImage;
typedef tImage<rgbePixel> rgbeImage; // Overload this to get a constructor.

template <class _ImgType> extern void tLoad(const char *fname, _ImgType *out);
template <class _ImgType> extern void tSave(const char *fname, const _ImgType *img);

// List here the image types for which tLoad is instantiated.
// This avoids tLoad being wanted for types for which it doesn't exist.
template<> DMC_INL void uc1Image::Load(const char *fname) {tLoad(fname, this);}
template<> DMC_INL void uc2Image::Load(const char *fname) {tLoad(fname, this);}
template<> DMC_INL void uc3Image::Load(const char *fname) {tLoad(fname, this);}
template<> DMC_INL void uc4Image::Load(const char *fname) {tLoad(fname, this);}
template<> DMC_INL void us1Image::Load(const char *fname) {tLoad(fname, this);}
template<> DMC_INL void us2Image::Load(const char *fname) {tLoad(fname, this);}
template<> DMC_INL void us3Image::Load(const char *fname) {tLoad(fname, this);}
template<> DMC_INL void us4Image::Load(const char *fname) {tLoad(fname, this);}
template<> DMC_INL void ui1Image::Load(const char *fname) {tLoad(fname, this);}
//template<> DMC_INL void ui2Image::Load(const char *fname) {tLoad(fname, this);}
//template<> DMC_INL void ui3Image::Load(const char *fname) {tLoad(fname, this);}
//template<> DMC_INL void ui4Image::Load(const char *fname) {tLoad(fname, this);}
template<> DMC_INL void f1Image::Load(const char *fname) {tLoad(fname, this);}
//template<> DMC_INL void f2Image::Load(const char *fname) {tLoad(fname, this);}
template<> DMC_INL void f3Image::Load(const char *fname) {tLoad(fname, this);}
//template<> DMC_INL void f4Image::Load(const char *fname) {tLoad(fname, this);}

// List here the image types for which tSave is instantiated.
// This avoids tSave being wanted for types for which it doesn't exist.
template<> DMC_INL void uc1Image::Save(const char *fname) const {tSave(fname, this);}
template<> DMC_INL void uc2Image::Save(const char *fname) const {tSave(fname, this);}
template<> DMC_INL void uc3Image::Save(const char *fname) const {tSave(fname, this);}
template<> DMC_INL void uc4Image::Save(const char *fname) const {tSave(fname, this);}
template<> DMC_INL void us1Image::Save(const char *fname) const {tSave(fname, this);}
template<> DMC_INL void us2Image::Save(const char *fname) const {tSave(fname, this);}
template<> DMC_INL void us3Image::Save(const char *fname) const {tSave(fname, this);}
template<> DMC_INL void us4Image::Save(const char *fname) const {tSave(fname, this);}
template<> DMC_INL void ui1Image::Save(const char *fname) const {tSave(fname, this);}
//template<> DMC_INL void ui2Image::Save(const char *fname) const {tSave(fname, this);}
//template<> DMC_INL void ui3Image::Save(const char *fname) const {tSave(fname, this);}
//template<> DMC_INL void ui4Image::Save(const char *fname) const {tSave(fname, this);}
template<> DMC_INL void f1Image::Save(const char *fname) const {tSave(fname, this);}
//template<> DMC_INL void f2Image::Save(const char *fname) const {tSave(fname, this);}
template<> DMC_INL void f3Image::Save(const char *fname) const {tSave(fname, this);}
//template<> DMC_INL void f4Image::Save(const char *fname) const {tSave(fname, this);}

#endif
