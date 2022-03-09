//////////////////////////////////////////////////////////////////////
// ImageLoadSave.h - Load and save images
//
// Copyright David K. McAllister, June 2001.

#pragma once

#include "Image/tImage.h"
#include "Util/Assert.h"

const int BMP_ = 0x00706d62; // "bmp\0", etc.
const int GIF_ = 0x00666967;
const int HDR_ = 0x00726468;
const int JPE_ = 0x0065706a;
const int JPG_ = 0x0067706a;
const int MAT_ = 0x0074616d;
const int PAM_ = 0x006d6170;
const int PFM_ = 0x006d6670;
const int PGM_ = 0x006d6770;
const int PNG_ = 0x00676e70;
const int PPM_ = 0x006d7070;
const int PSM_ = 0x006d7370;
const int PZM_ = 0x006d7a70;
const int RAS_ = 0x00736172;
const int RGB_ = 0x00626772;
const int TGA_ = 0x00616774;
const int TIF_ = 0x00666974;

// Return an int whose bytes equal the characters of the extension string
extern int GetExtensionVal(const char* fname);

class ImageLoadSave {
public:
    baseImage* baseImg; // This is actually a pointer to the tImage struct of a type matching the image file.
    saveParams SP;
    unsigned char* Pix;
    int wid, hgt, chan;
    bool is_uint, is_float, is_ushort;

    // Constructors
    ImageLoadSave()
    {
        Pix = NULL;
        baseImg = NULL;
        wid = hgt = chan = 0;
        is_uint = is_float = is_ushort = false;
    }

    // Destroy an ImageLoadSave.
    ~ImageLoadSave()
    {
        // If LoadtImage() was called, it steals the baseImg and sets it to NULL.
        // If tLoad() was called it lets this destructor destroy the baseImg, though it may steal baseImg's raster first.
        // If an error gets thrown while loading, this destructor gets called and baseImg is not NULL.
        // If an error gets thrown while saving, this destructor gets called and baseImg is NULL, so destroys nothing.
        // The saver doesn't own any of the data it points to.
        if (baseImg) delete baseImg;
    }

    int size() const { return wid * hgt; }
    int size_bytes() const { return size() * chan * ((is_uint || is_float) ? 4 : is_ushort ? 2 : 1); }

    // Hooks the given raster image into this image object.
    void SetImage(unsigned char* p, const int w, const int h, const int ch, const bool is_uint_ = false, const bool is_ushort_ = false,
                  const bool is_float_ = false)
    {
        Pix = p;
        wid = w;
        hgt = h;
        chan = ch;
        is_uint = is_uint_;
        is_ushort = is_ushort_;
        is_float = is_float_;
    }

    // All of these throw a DMcError on error.
    void Load(const char* fname);
    void Save(const char* fname) const;

private:
    void LoadBMP(const char* fname);
    void LoadGIF(const char* fname, bool WantPaletteInds = false); // If true, returns one channel image. No palette.
    void LoadJPEG(const char* fname);
    void LoadMAT(const char* fname);
    void LoadPNG(const char* fname);
    void LoadPPM(const char* fname);
    void LoadRGB(const char* fname);
    void LoadRGBE(const char* fname);
    void LoadRas(const char* fname);
    void LoadTGA(const char* fname, bool R5G6B5 = false); // If true, interprets incoming 16 bits as R5G6B5 instead of X1R5G5B5.
    void LoadTIFF(const char* fname);

    void SaveBMP(const char* fname) const;
    void SaveGIF(const char* fname, int MaxColorsWanted = 256) const;
    void SaveJPEG(const char* fname) const;
    void SaveMAT(const char* fname) const;
    void SavePNG(const char* fname) const;
    void SavePPM(const char* fname) const;
    void SaveRGBE(const char* fname) const;
    void SaveTGA(const char* fname) const;
    void SaveTIFF(const char* fname) const;

    // Called by Load*(). It creates a tImage that matches the is_* and chan args. Stores the pointer to the tImage in baseImg.
    unsigned char* ImageAlloc();
};
