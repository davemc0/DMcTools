//////////////////////////////////////////////////////////////////////
// ImageLoadSave.h - Load and save images.
//
// Copyright David K. McAllister, June 2001.

#ifndef _ImageLoadSave_h
#define _ImageLoadSave_h

#include <Util/Assert.h>

class ImageLoadSave
{
public:
    unsigned char *Pix;
    int wid, hgt, chan;
    bool is_uint, is_float, is_ushort;

    // Constructors
    ImageLoadSave()
    {
        Pix = NULL;
        secret = NULL;
        wid = hgt = chan = 0;
        is_uint = is_float = is_ushort = false;
    }

    // Destroy an image.
    ~ImageLoadSave()
    {
        // The data should always get taken away from me.
        ASSERT_R(Pix == NULL);
    }

    int size() const {return wid * hgt;}
    int size_bytes() const {return size() * chan * ((is_uint || is_float) ? 4 : is_ushort ? 2 : 1);}

    // Hooks the given raster image into this image object.
    void SetImage(unsigned char *p, const int w, const int h, const int ch,
        const bool _is_uint = false, const bool _is_ushort = false, const bool _is_float = false)
    {
        Pix = p;
        wid = w;
        hgt = h;
        chan = ch;
        is_uint = _is_uint;
        is_ushort = _is_ushort;
        is_float = _is_float;
    }

    // Give this image the contents of Img and remove them therefrom.
    // This is a fast way to assign an image that avoids the huge memory allocate and copy.
    void TransferFrom(ImageLoadSave &Img)
    {
        SetImage(Img.Pix, Img.wid, Img.hgt, Img.chan);
        Img.Pix = NULL;
        Img.wid = Img.hgt = Img.chan = 0;
        Img.is_uint = Img.is_float = false;
    }

    // All of these throw a DMcError on error.
    void Load(const char *fname);
    void Save(const char *fname) const;

    void LoadGIF(const char *fname, bool WantPaletteInds = false); // If true, returns one channel image. No palette.
    void LoadPPM(const char *fname);
    void LoadRas(const char *fname);
    void LoadBMP(const char *fname);
    void LoadRGB(const char *fname);
    void LoadTGA(const char *fname, bool R5G6B5 = false); // If true, interprets incoming 16 bits as R5G6B5 instead of X1R5G5B5.
    void SaveGIF(const char *fname, int MaxColorsWanted = 256) const;
    void SavePPM(const char *fname) const;
    void SaveBMP(const char *fname) const;
    void SaveTGA(const char *fname) const;
    void LoadTIFF(const char *fname);
    void SaveTIFF(const char *fname) const;
    void LoadJPEG(const char *fname);
    void SaveJPEG(const char *fname) const;
    void LoadPNG(const char *fname);
    void SavePNG(const char *fname) const;
    void LoadMAT(const char *fname);
    void SaveMAT(const char *fname) const;
#ifdef DMC_USE_RGBE
    void LoadHDR(const char* fname);
    void SaveHDR(const char *fname) const;
#endif

    // This is actually a pointer to the tImage struct that really holds the pixel data.
    void *secret;
    unsigned char *ImageAlloc();
    void ucLoad(const char *fname, int ch);
};

#endif
