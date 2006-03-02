//////////////////////////////////////////////////////////////////////
// ImageLoadSave.h - Load and save unsigned char images.
//
// Copyright David K. McAllister, June 2001.

#ifndef _ImageLoadSave_h
#define _ImageLoadSave_h

#include <Util/Assert.h>

static const bool DMC_ERROR = true;
static const bool DMC_OK = false;

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
        ASSERT0(Pix == NULL);
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
    // This is a fast way to assign an image that avoids the huge
    // memory allocate and copy.
    void TransferFrom(ImageLoadSave &Img)
    {
        SetImage(Img.Pix, Img.wid, Img.hgt, Img.chan);
        Img.Pix = NULL;
        Img.wid = Img.hgt = Img.chan = 0;
        Img.is_uint = Img.is_float = false;
    }
    
    void Load(const char *fname);
    // Return DMC_OK or DMC_ERROR.
    bool Save(const char *fname) const;
    
    bool LoadGIF(const char *fname, bool WantPaletteInds = false); // If true, returns one channel image. No palette.
    bool LoadPPM(const char *fname);
    bool LoadRas(const char *fname);
    bool LoadBMP(const char *fname);
    bool LoadRGB(const char *fname);
    bool LoadTGA(const char *fname, bool R5G6B5 = false); // If true, interprets incoming 16 bits as R5G6B5 instead of X1R5G5B5.
    bool SaveGIF(const char *fname, int MaxColorsWanted = 256) const;
    bool SavePPM(const char *fname) const;
    bool SaveBMP(const char *fname) const;
    bool SaveTGA(const char *fname) const;
    bool LoadTIFF(const char *fname);
    bool SaveTIFF(const char *fname) const;
    bool LoadJPEG(const char *fname);
    bool SaveJPEG(const char *fname) const;
    bool LoadPNG(const char *fname);
    bool SavePNG(const char *fname) const;
    bool LoadMAT(const char *fname);
    bool SaveMAT(const char *fname) const;
#ifdef DMC_USE_HDR
    bool LoadHDR(const char* fname);
    bool SaveHDR(const char *fname) const;
#endif
    
    // This is actually a pointer to the tImage struct that really holds the pixel data.
    void *secret;
    unsigned char *ImageAlloc();
    void ucLoad(const char *fname, int ch);
};

#endif
