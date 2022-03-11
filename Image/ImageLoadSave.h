//////////////////////////////////////////////////////////////////////
// ImageLoadSave.h - Load and save images
//
// Copyright David K. McAllister, June 2001.

#pragma once

#include "Image/LoadSaveParams.h"

#include <string>

class baseImage;

class ImageLoadSave {
public:
    baseImage* baseImg; // This is actually a pointer to the tImage struct of a type matching the image file.
    LoadSaveParams SP;
    unsigned char* Pix;
    int wid, hgt, chan;
    bool is_uint, is_float, is_ushort;

    ImageLoadSave()
    {
        Pix = NULL;
        baseImg = NULL;
        wid = hgt = chan = 0;
        is_uint = is_float = is_ushort = false;
    }

    ~ImageLoadSave();

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

    void Load(const char* fname);
    void Save(const char* fname) const;

    template <class Image_T> const baseImage* ConvertToSaveFormat(const char* fname, const Image_T* srcImg);

private:
    void LoadBMP(const char* fname);
    void LoadGIF(const char* fname);
    void LoadJPEG(const char* fname);
    void LoadMAT(const char* fname);
    void LoadPNG(const char* fname);
    void LoadPPM(const char* fname);
    void LoadRGB(const char* fname);
    void LoadRGBE(const char* fname);
    void LoadRas(const char* fname);
    void LoadTGA(const char* fname);
    void LoadTIFF(const char* fname);

    void SaveBMP(const char* fname) const;
    void SaveGIF(const char* fname) const;
    void SaveJPEG(const char* fname) const;
    void SaveMAT(const char* fname) const;
    void SavePNG(const char* fname) const;
    void SavePPM(const char* fname) const;
    void SaveRGBE(const char* fname) const;
    void SaveTGA(const char* fname) const;
    void SaveTIFF(const char* fname) const;

    // Called by Load*(). It creates a tImage that matches the is_* and chan args. Stores the pointer to the tImage in baseImg.
    unsigned char* ImageAlloc();

    baseImage* ConvertToSaveFormat(const baseImage* srcImg);
};
