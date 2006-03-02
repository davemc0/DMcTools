//////////////////////////////////////////////////////////////////////
// ImageLoadSave.cpp - Load and save images of many file formats.
//
// Copyright David K. McAllister, Aug. 1997.

#include <Image/ImageLoadSave.h>
#include <Util/Utils.h>
#include <Image/tImage.h>

#ifdef DMC_USE_TIFF
#include <tiffio.h>
#endif

#ifdef DMC_USE_JPEG
extern "C" {
#include <jpeglib.h>
}
#endif

#ifdef DMC_USE_PNG
extern "C" {
#include <png.h>
}
#endif

#ifdef DMC_USE_MAT
extern "C" {
#include <mat.h>
}
#endif

#ifdef DMC_USE_HDR
#include "RGBEio.h"
#endif

#ifdef DMC_MACHINE_sgi
#include <fstream>
#include <string>
using namespace std;
#endif

#ifdef DMC_MACHINE_win
#include <fstream>
#include <string>
using namespace std;
#endif

#ifdef DMC_MACHINE_hp
#include <fstream.h>
#include <string.h>
#endif

#define RAS_MAGIC 0x59a66a95
#define GIF_MAGIC 0x47494638
#define JPEG_MAGIC 0xffd8ffe0
#define RGB_MAGIC 0x01da0101

#ifdef DMC_DEBUG
static bool Verbose = true;
#else
static bool Verbose = false;
#endif

// A back-door way to set this.
bool dmcTGA_R5G6B5 = false;

static bool GetExtension(string &exts, const char *fname)
{
    char *ext = GetFileExtension(fname);
    ToLower(ext);
    exts = ext;
    
    return DMC_OK;
}

// Fills wid, hgt, chan, Pix, and secret.
// Pix points to the same data as does secret.Pix.
// The data must be deleted by secret.
// secret must also be deleted by the caller.
// Pix and secret will be NULL on error.
void ImageLoadSave::Load(const char *fname)
{
    if(Pix)
        delete [] Pix;
    
    Pix = NULL;
    wid = hgt = chan = 0;
    
    string exts;
    if(GetExtension(exts, fname))
        return; // Error.
    
    ifstream InFile(fname, ios::in | ios::binary);
    if(!InFile.is_open()) {
        // cerr << "Failed to open file `" << fname << "'.\n";
        return;
    }
    
    unsigned int Magic;
    char *Mag = (char *)&Magic;
    InFile.read(Mag, 4);
    InFile.close();
    unsigned int eMagic = Magic;
    ConvertLong(&eMagic, 1);
    
    if(Magic==RAS_MAGIC || eMagic==RAS_MAGIC || exts=="ras") {
        LoadRas(fname);
    } else if(Magic==GIF_MAGIC || eMagic==GIF_MAGIC || exts=="gif") {
        LoadGIF(fname);
    } else if(Magic==JPEG_MAGIC || eMagic==JPEG_MAGIC || exts=="jpg") {
        LoadJPEG(fname);
    } else if(Magic==RGB_MAGIC || eMagic==RGB_MAGIC || exts=="rgb") {
        LoadRGB(fname);
    } else if((Mag[0]=='P' && (Mag[1]=='5' || Mag[1]=='6' || Mag[1]=='7' || Mag[1]=='8' || Mag[1]=='Z')) ||
        (Mag[3]=='P' && (Mag[2]=='5' || Mag[2]=='6' || Mag[2]=='7' || Mag[2]=='8' || Mag[2]=='Z')) ||
        exts=="ppm" || exts=="pgm" || exts=="pam" || exts=="pfm" || exts=="psm" || exts=="pzm") {
        LoadPPM(fname);
    } else if((Mag[1]=='P' && Mag[2]=='N' && Mag[3]=='G') ||
        (Mag[2]=='P' && Mag[1]=='N' && Mag[0]=='G') || exts=="png") {
        LoadPNG(fname);
    } else if((Mag[0]=='B' && Mag[1]=='M') ||
        (Mag[3]=='B' && Mag[2]=='M') || exts=="bmp") {
        LoadBMP(fname);
    } else if((Mag[0]==0 || Mag[3]==0) && (exts=="tga")) {
        LoadTGA(fname, dmcTGA_R5G6B5);
    } else if(exts=="tif") {
        LoadTIFF(fname);
    } else if(exts=="hdr") {
        LoadHDR(fname);
    } else {
        cerr << "Could not determine file type of `" << fname << "'.\n";
        cerr << "Magic was " << Magic << " or "<<eMagic<<
            " or `" << Mag[0] << Mag[1] << Mag[2] << Mag[3] << "'.\n";
        cerr << "Extension was " << exts << endl;
    }
}

// Choose a saver based strictly on extension.
// The individual savers may look at chan, is_uint, etc. to decide a format.
// Returns DMC_OK on success; DMC_ERROR on error.
bool ImageLoadSave::Save(const char *_fname) const
{
    char *fname = strdup(_fname);
    string exts;
    bool rslt = GetExtension(exts, fname);
    if(rslt == DMC_ERROR) return rslt;
    
        
    char *outfname3 = strchr(fname, '\n');
    if(outfname3) {
        *outfname3 = '\0';
    }
    
    outfname3 = strchr(fname, '\r');
    if(outfname3) {
        *outfname3 = '\0';
    }

    bool bad = DMC_ERROR;
    if(exts=="gif")
        bad =  SaveGIF(fname);
    else if(exts=="tif")
        bad =  SaveTIFF(fname);
    else if(exts=="tga")
        bad =  SaveTGA(fname);
    else if(exts=="jpg")
        bad =  SaveJPEG(fname);
    else if(exts=="png")
        bad =  SavePNG(fname);
    else if(exts=="bmp")
        bad =  SaveBMP(fname);
    else if(exts=="hdr")
        bad =  SaveHDR(fname);
    else if(exts=="mat")
        bad =  SaveMAT(fname);
    else if(exts=="ppm" || exts=="pgm" || exts=="pam" ||
        exts=="psm" || exts=="pfm" || exts=="pzm")
        bad =  SavePPM(fname);
    else
        bad = DMC_ERROR;
    
    if(fname)
        delete [] fname;

    return bad;
}

// For loading an image into whatever kind of tImage is most appropriate.
// XXX Broken: Points to image data, not image struct.
// Also, how am I supposed to tell what kind it loaded, anyway?
baseImage *LoadtImage(const char *fname)
{
    ImageLoadSave loader;
    loader.Load(fname);
    
    if(loader.Pix == NULL) {
        // I can't deal with deleting this data here.
        ASSERT0(loader.secret == NULL); // This may be too strong.
    }

    return (baseImage *)loader.secret;
}

// Load an image and store it in the tImage out.
// Assumes that out has already been cleared.
template <class _ImgType> void tLoad(const char *fname, _ImgType *out)
{
    // Delete the old contents of the image.
    out->SetSize();

    ImageLoadSave loader;
    loader.Load(fname);
    
    if(loader.Pix == NULL) {
        // I can't deal with deleting this data here.
        ASSERT0(loader.secret == NULL); // This may be too strong.
        return;
    }

    baseImage *unk = (baseImage *)loader.secret;
    
    // Now find out the type of the loaded image and convert to type of out.
    // Need a case for every kind of image file format.
    // If in and out are the same type, just grab the data.
    // The cast to _ImgType is a cheat to make it compile.
    // That code will only get called when the types are really the same.
    // *** Comment out the image kinds that we currently can't load.
    switch(loader.chan) {
    case 1:
        if(loader.is_uint) {
            if(typeid(_ImgType) == typeid(ui1Image)) {out->swap(*(_ImgType *)unk);}
            else {*out = *(ui1Image *)unk; delete (ui1Image *)unk;}
        } else if(loader.is_float) {
            if(typeid(_ImgType) == typeid(f1Image)) {out->swap(*(_ImgType *)unk);}
            else {*out = *(f1Image *)unk; delete (f1Image *)unk;}
        } else if(loader.is_ushort) {
            if(typeid(_ImgType) == typeid(us1Image)) {out->swap(*(_ImgType *)unk);}
            else {*out = *(us1Image *)unk; delete (us1Image *)unk;}
        } else {
            if(typeid(_ImgType) == typeid(uc1Image)) {out->swap(*(_ImgType *)unk);}
            else {*out = *(uc1Image *)unk; delete (uc1Image *)unk;}
        }
        break;
    case 2:
        /* if(loader.is_uint) {
            if(typeid(_ImgType) == typeid(ui2Image)) {out->swap(*(_ImgType *)unk);}
            else {*out = *(ui2Image *)unk; delete (ui2Image *)unk;}
        } else if(loader.is_float) {
            if(typeid(_ImgType) == typeid(f2Image)) {out->swap(*(_ImgType *)unk);}
            else {*out = *(f2Image *)unk; delete (f2Image *)unk;}
        } else */ if(loader.is_ushort) {
            if(typeid(_ImgType) == typeid(us2Image)) {out->swap(*(_ImgType *)unk);}
            else {*out = *(us2Image *)unk; delete (us2Image *)unk;}
        } else {
            if(typeid(_ImgType) == typeid(uc2Image)) {out->swap(*(_ImgType *)unk);}
            else {*out = *(uc2Image *)unk; delete (uc2Image *)unk;}
        }
        break;
    case 3:
        /* if(loader.is_uint) {
            if(typeid(_ImgType) == typeid(ui3Image)) {out->swap(*(_ImgType *)unk);}
            else {*out = *(ui3Image *)unk; delete (ui3Image *)unk;}
        } else */ if(loader.is_float) {
            if(typeid(_ImgType) == typeid(f3Image)) {out->swap(*(_ImgType *)unk);}
            else {*out = *(f3Image *)unk; delete (f3Image *)unk;}
        } else if(loader.is_ushort) {
            if(typeid(_ImgType) == typeid(us3Image)) {out->swap(*(_ImgType *)unk);}
            else {*out = *(us3Image *)unk; delete (us3Image *)unk;}
        } else {
            if(typeid(_ImgType) == typeid(uc3Image)) {out->swap(*(_ImgType *)unk);}
            else {*out = *(uc3Image *)unk; delete (uc3Image *)unk;}
        }
        break;
    case 4:
        /* if(loader.is_uint) {
            if(typeid(_ImgType) == typeid(ui4Image)) {out->swap(*(_ImgType *)unk);}
            else {*out = *(ui4Image *)unk; delete (ui4Image *)unk;}
        } else if(loader.is_float) {
            if(typeid(_ImgType) == typeid(f4Image)) {out->swap(*(_ImgType *)unk);}
            else {*out = *(f4Image *)unk; delete (f4Image *)unk;}
        } else */ if(loader.is_ushort) {
            if(typeid(_ImgType) == typeid(us4Image)) {out->swap(*(_ImgType *)unk);}
            else {*out = *(us4Image *)unk; delete (us4Image *)unk;}
        } else {
            if(typeid(_ImgType) == typeid(uc4Image)) {out->swap(*(_ImgType *)unk);}
            else {*out = *(uc4Image *)unk; delete (uc4Image *)unk;}
        }
        break;
    default:
        ASSERT0(0);
        break;
    }
    loader.Pix = NULL;
}

// Instantiations. Need a case for every kind of image that will
// have the capability to load an image (all of them).
template void tLoad(const char *fname, uc1Image *out);
template void tLoad(const char *fname, uc2Image *out);
template void tLoad(const char *fname, uc3Image *out);
template void tLoad(const char *fname, uc4Image *out);
template void tLoad(const char *fname, us1Image *out);
template void tLoad(const char *fname, us2Image *out);
template void tLoad(const char *fname, us3Image *out);
template void tLoad(const char *fname, us4Image *out);
template void tLoad(const char *fname, ui1Image *out);
//template void tLoad(const char *fname, ui2Image *out);
//template void tLoad(const char *fname, ui3Image *out);
//template void tLoad(const char *fname, ui4Image *out);
template void tLoad(const char *fname, f1Image *out);
//template void tLoad(const char *fname, f2Image *out);
template void tLoad(const char *fname, f3Image *out);
//template void tLoad(const char *fname, f4Image *out);
#if 0
template void tLoad(const char *fname, h1Image *out);
//template void tLoad(const char *fname, h2Image *out);
template void tLoad(const char *fname, h3Image *out);
template void tLoad(const char *fname, h4Image *out);
#endif

// XXX Using the virtual members of baseImage, this could be done without templating.
// Save a tImage. Need to know all the details about it.
template <class _ImgType> void tSave(const char *fname, const _ImgType *img)
{
    ImageLoadSave saver;
    saver.SetImage((unsigned char *)img->pp(), img->w(), img->h(),
        img->chan(), (typeid(_ImgType::PixType::ElType)==typeid(unsigned int)),
        (typeid(_ImgType::PixType::ElType)==typeid(unsigned short)),
        (typeid(_ImgType::PixType::ElType)==typeid(float)));
    bool rslt = saver.Save(fname);
    saver.Pix = NULL; saver.wid = saver.hgt = saver.chan = 0;
}

// Instantiations. Need a case for every kind of image that will
// have the capability to save an image (all of them).
template void tSave(const char *fname, const uc1Image *out);
template void tSave(const char *fname, const uc2Image *out);
template void tSave(const char *fname, const uc3Image *out);
template void tSave(const char *fname, const uc4Image *out);
template void tSave(const char *fname, const us1Image *out);
template void tSave(const char *fname, const us2Image *out);
template void tSave(const char *fname, const us3Image *out);
template void tSave(const char *fname, const us4Image *out);
template void tSave(const char *fname, const ui1Image *out);
//template void tSave(const char *fname, const ui2Image *out);
//template void tSave(const char *fname, const ui3Image *out);
//template void tSave(const char *fname, const ui4Image *out);
template void tSave(const char *fname, const f1Image *out);
//template void tSave(const char *fname, const f2Image *out);
template void tSave(const char *fname, const f3Image *out);
//template void tSave(const char *fname, const f4Image *out);
#if 0
template void tSave(const char *fname, const h1Image *out);
//template void tSave(const char *fname, const h2Image *out);
template void tSave(const char *fname, const h3Image *out);
template void tSave(const char *fname, const h4Image *out);
#endif

void ImageLoadSave::ucLoad(const char *fname, int ch)
{
    if(ch<0) ch = 0;
    
    switch(ch) {
    case 0:
        {
            // Don't care what kind of image you get.
            Load(fname);
            unsigned char *old = Pix;
            if(Pix == NULL)
                return;

            baseImage *unk = (baseImage *)secret;
            
            // Find out the type of unk and convert from it to unsigned char.
            // Need a case for every kind of image file format.
            switch(chan) {
            case 1:
                if(is_uint) {
                    ui1Image *Img = (ui1Image *)unk; uc1Image tmp = *Img; delete Img;
                    Pix = new unsigned char[tmp.size_bytes()]; memcpy(Pix, tmp.pp(), tmp.size_bytes());
                } else if(is_float) {
                    f1Image *Img = (f1Image *)unk; uc1Image tmp = *Img; delete Img;
                    Pix = new unsigned char[tmp.size_bytes()]; memcpy(Pix, tmp.pp(), tmp.size_bytes());
                } else if(is_ushort) {
                    us1Image *Img = (us1Image *)unk; uc1Image tmp = *Img; delete Img;
                    Pix = new unsigned char[tmp.size_bytes()]; memcpy(Pix, tmp.pp(), tmp.size_bytes());
                } else {
                    uc1Image *Img = (uc1Image *)unk; Pix = new unsigned char[size_bytes()];
                    memcpy(Pix, old, size_bytes()); delete Img;
                }
                break;
            case 2:
                if(is_uint) {
                    ui2Image *Img = (ui2Image *)unk; uc2Image tmp = *Img; delete Img;
                    Pix = new unsigned char[tmp.size_bytes()]; memcpy(Pix, tmp.pp(), tmp.size_bytes());
                } else if(is_float) {
                    f2Image *Img = (f2Image *)unk; uc2Image tmp = *Img; delete Img;
                    Pix = new unsigned char[tmp.size_bytes()]; memcpy(Pix, tmp.pp(), tmp.size_bytes());
                } else if(is_ushort) {
                    us2Image *Img = (us2Image *)unk; uc2Image tmp = *Img; delete Img;
                    Pix = new unsigned char[tmp.size_bytes()]; memcpy(Pix, tmp.pp(), tmp.size_bytes());
                } else {
                    uc2Image *Img = (uc2Image *)unk; Pix = new unsigned char[size_bytes()];
                    memcpy(Pix, old, size_bytes()); delete Img;
                }
                break;
            case 3:
                if(is_uint) {
                    ui3Image *Img = (ui3Image *)unk; uc3Image tmp = *Img; delete Img;
                    Pix = new unsigned char[tmp.size_bytes()]; memcpy(Pix, tmp.pp(), tmp.size_bytes());
                } else if(is_float) {
                    f3Image *Img = (f3Image *)unk; uc3Image tmp = *Img; delete Img;
                    Pix = new unsigned char[tmp.size_bytes()]; memcpy(Pix, tmp.pp(), tmp.size_bytes());
                } else if(is_ushort) {
                    us3Image *Img = (us3Image *)unk; uc3Image tmp = *Img; delete Img;
                    Pix = new unsigned char[tmp.size_bytes()]; memcpy(Pix, tmp.pp(), tmp.size_bytes());
                } else {
                    uc3Image *Img = (uc3Image *)unk; Pix = new unsigned char[size_bytes()];
                    memcpy(Pix, old, size_bytes()); delete Img;
                }
                break;
            case 4:
                if(is_uint) {
                    ui4Image *Img = (ui4Image *)unk; uc4Image tmp = *Img; delete Img;
                    Pix = new unsigned char[tmp.size_bytes()]; memcpy(Pix, tmp.pp(), tmp.size_bytes());
                } else if(is_float) {
                    f4Image *Img = (f4Image *)unk; uc4Image tmp = *Img; delete Img;
                    Pix = new unsigned char[tmp.size_bytes()]; memcpy(Pix, tmp.pp(), tmp.size_bytes());
                } else if(is_ushort) {
                    us3Image *Img = (us3Image *)unk; uc3Image tmp = *Img; delete Img;
                    Pix = new unsigned char[tmp.size_bytes()]; memcpy(Pix, tmp.pp(), tmp.size_bytes());
                } else {
                    uc4Image *Img = (uc4Image *)unk; Pix = new unsigned char[size_bytes()];
                    memcpy(Pix, old, size_bytes()); delete Img;
                }
                break;
            default:
                ASSERT0(0);
                break;
            }
            break;
        }
        // If you know how many channels you want, make an image of that size.
        // Then copy the data to unsigned char. The tImage will be destroyed.
    case 1:
        {
            uc1Image tmp(fname);
            wid = tmp.w(); hgt = tmp.h(); chan = ch;
            Pix = new unsigned char[tmp.size_bytes()];
            memcpy(Pix, tmp.pp(), tmp.size_bytes());
            break;
        }
    case 2:
        {
            uc2Image tmp(fname);
            wid = tmp.w(); hgt = tmp.h(); chan = ch;
            Pix = new unsigned char[tmp.size_bytes()];
            memcpy(Pix, tmp.pp(), tmp.size_bytes());
            break;
        }
    case 3:
        {
            uc3Image tmp(fname);
            wid = tmp.w(); hgt = tmp.h(); chan = ch;
            Pix = new unsigned char[tmp.size_bytes()];
            memcpy(Pix, tmp.pp(), tmp.size_bytes());
            break;
        }
    case 4:
        {
            uc4Image tmp(fname);
            wid = tmp.w(); hgt = tmp.h(); chan = ch;
            Pix = new unsigned char[tmp.size_bytes()];
            memcpy(Pix, tmp.pp(), tmp.size_bytes());
            break;
        }
    }
}

// Allocate a tImage of the appropriate type for the info stored in this ImageLoadSave.
unsigned char *ImageLoadSave::ImageAlloc()
{
    switch(chan) {
    case 1:
        if(is_uint) {
            ui1Image *tmp = new ui1Image(wid,hgt);
            Pix = (unsigned char *)tmp->pp();
            secret = tmp;
        } else if(is_float) {
            f1Image *tmp = new f1Image(wid,hgt);
            Pix = (unsigned char *)tmp->pp();
            secret = tmp;
        } else if(is_ushort) {
            us1Image *tmp = new us1Image(wid,hgt);
            Pix = (unsigned char *)tmp->pp();
            secret = tmp;
        } else {
            uc1Image *tmp = new uc1Image(wid,hgt);
            Pix = (unsigned char *)tmp->pp();
            secret = tmp;
        }
        break;
    case 2:
        if(is_ushort) {
            us2Image *tmp = new us2Image(wid,hgt);
            Pix = (unsigned char *)tmp->pp();
            secret = tmp;
        } else {
            uc2Image *tmp = new uc2Image(wid,hgt);
            Pix = (unsigned char *)tmp->pp();
            secret = tmp;
            break;
        }
    case 3:
        if(is_float) {
            f3Image *tmp = new f3Image(wid,hgt);
            Pix = (unsigned char *)tmp->pp();
            secret = tmp;
        } else if(is_ushort) {
            us3Image *tmp = new us3Image(wid,hgt);
            Pix = (unsigned char *)tmp->pp();
            secret = tmp;
        } else {
            uc3Image *tmp = new uc3Image(wid,hgt);
            Pix = (unsigned char *)tmp->pp();
            secret = tmp;
        }
        break;
    case 4:
        if(is_ushort) {
            us4Image *tmp = new us4Image(wid,hgt);
            Pix = (unsigned char *)tmp->pp();
            secret = tmp;
        } else {
            uc4Image *tmp = new uc4Image(wid,hgt);
            Pix = (unsigned char *)tmp->pp();
            secret = tmp;
            break;
        }
    }
    
    return Pix;
}

//////////////////////////////////////////////////////
// Sun Raster File Format

// RAS Header
struct rasterfile
{
    int ras_magic; /* magic number */
    int ras_width; /* width (pixels) of image */
    int ras_height; /* height (pixels) of image */
    int ras_depth; /* depth (1, 8, or 24 bits) of pixel */
    int ras_length; /* length (bytes) of image */
    int ras_type; /* type of file; see RT_* below */
    int ras_maptype; /* type of colormap; see RMT_* below */
    int ras_maplength; /* length (bytes) of following map */
    /* color map follows for ras_maplength bytes, followed by image */
};

/* Sun supported ras_type's */
#define RT_OLD 0 /* Raw pixrect image in 68000 byte order */
#define RT_STANDARD 1 /* Raw pixrect image in 68000 byte order */
#define RT_BYTE_ENCODED 2 /* Run-length compression of bytes */
#define RT_FORMAT_RGB 3 /* XRGB or RGB instead of XBGR or BGR */
#define RT_FORMAT_TIFF 4 /* tiff <-> standard rasterfile */
#define RT_FORMAT_IFF 5 /* iff (TAAC format) <-> standard rasterfile */
#define RT_EXPERIMENTAL 0xffff /* Reserved for testing */

/* Sun registered ras_maptype's */
#define RMT_RAW 2
/* Sun supported ras_maptype's */
#define RMT_NONE 0 /* ras_maplength is expected to be 0 */
#define RMT_EQUAL_RGB 1 /* red[ras_maplength/3],green[],blue[] */

/*
* NOTES:
* Each line of the image is rounded out to a multiple of 16 bits.
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^!!!!!!!!!!!!!!
* This corresponds to the rounding convention used by the memory pixrect
* package (/usr/include/pixrect/memvar.h) of the SunWindows system.
* The ras_encoding field (always set to 0 by Sun's supported software)
* was renamed to ras_length in release 2.0. As a result, rasterfiles
* of type 0 generated by the old software claim to have 0 length; for
* compatibility, code reading rasterfiles must be prepared to compute the
* true length from the width, height, and depth fields.
*/

bool ImageLoadSave::LoadRas(const char *fname)
{
    // Read a Sun Raster File image.
    ifstream InFile(fname, ios::in | ios::binary);
    if(!InFile.is_open()) {
        return DMC_ERROR;
    }
    
    rasterfile Hedr;
    InFile.read((char *) &Hedr, sizeof(rasterfile));
    
#ifdef DMC_LITTLE_ENDIAN
    ConvertLong((unsigned int *)&Hedr, sizeof(rasterfile) / sizeof(int));
#endif
    
    wid = Hedr.ras_width;
    hgt = Hedr.ras_height;
    chan = 3;
    
    if(Hedr.ras_depth != 24) {
        cerr << "Take your " << Hedr.ras_depth << " bit image and go away!\n";
        wid = hgt = chan = 0;
        return DMC_ERROR;
    }
    
    if(size_bytes() != Hedr.ras_length) {
        cerr << "Size was " << size_bytes() << ", but ras_length was " << Hedr.ras_length << ".\n";
        return DMC_ERROR;
    }
    
    if(wid > 4096) {
        cerr << "Too big! " << wid << endl;
        return DMC_ERROR;
    }
    
    is_uint = false;
    is_float = false;
    Pix = ImageAlloc();
    
    if(Hedr.ras_type==RT_FORMAT_RGB) {
        if(Hedr.ras_maptype==RMT_NONE) {
            // Now read the color values.
            for (int y = 0; y < hgt; y++) {
                InFile.read((char *)&Pix[y*wid*3], wid*3);
            }
        }
        else if(Hedr.ras_maptype==RMT_EQUAL_RGB) {
            if(Verbose) cerr << "Reading color mapped image. Maplength = " << Hedr.ras_maplength << endl;
            
            unsigned char ColorMap[256][3];
            unsigned char Colors[4096];
            InFile.read((char *)ColorMap, Hedr.ras_maplength*3);
            
            for (int y = 0; y < hgt; y++) {
                InFile.read((char *)Colors, wid);
                
                for(int x=0; x<wid; ) {
                    Pix[y*wid+x++] = ColorMap[Colors[x]][0];
                    Pix[y*wid+x++] = ColorMap[Colors[x]][1];
                    Pix[y*wid+x++] = ColorMap[Colors[x]][2];
                }
            }
        } else {
            cerr << "Strange color map scheme.\n";
            return DMC_ERROR;
        }
    } else if(Hedr.ras_type==RT_STANDARD) {
        if(Verbose) cerr << "BGR ImageLoadSave (RT_STANDARD)\n";
        if(Hedr.ras_maptype==RMT_NONE) {
            // Now read the color values.
            unsigned char Colors[4096][3];
            
            int ii=0;
            for (int y = 0; y < hgt; y++) {
                InFile.read((char *)Colors, wid*3);
                
                for(int x=0; x<wid; x++) {
                    Pix[ii++] = Colors[x][2];
                    Pix[ii++] = Colors[x][1];
                    Pix[ii++] = Colors[x][0];
                }
            }
        } else {
            cerr << "Strange color map scheme.\n";
            return DMC_ERROR;
        }
    } else {
        cerr << "Strange format.\n";
        return DMC_ERROR;
    }
    
    InFile.close();
    
    return DMC_OK;
}


//////////////////////////////////////////////////////
// PPM File Format

// PAM is four-channel PPM. PFM is one- or three-channel float. PGM is one-channel uchar.
bool ImageLoadSave::LoadPPM(const char *fname)
{
    ifstream InFile(fname, ios::in | ios::binary);
    if(!InFile.is_open()) {
        return DMC_ERROR;
    }
    
    char Magic1, Magic2;
    InFile >> Magic1 >> Magic2;
    
    if(Magic1!='P' || (Magic2!='5' && Magic2!='6' && Magic2!='7' &&
        Magic2!='8' && Magic2!='Z' && Magic2!='S' && Magic2!='T' && Magic2!='U' && Magic2!='V')) {
        cerr << fname << " is not a known PPM file.\n";
        InFile.close();
        return DMC_ERROR;
    }
    
    InFile.get();
    char c = InFile.peek();
    while(c=='#') {
        char line[999];
        InFile.getline(line, 1000);
        if(Verbose) cerr << line << endl;
        c = InFile.peek();
    }
    
    int dyn_range;
    InFile >> wid >> hgt >> dyn_range;
    InFile.get();
    
    if(dyn_range != 255) {
        cerr << "Must be 255. Was " << dyn_range << endl;
        return DMC_ERROR;
    }
    
    // XXX Need to distinguish one-channel float images.
    is_uint = false;
    is_float = false;
    switch(Magic2) {
    case '5':
        chan = 1;
        break;
    case '6':
        chan = 3;
        break;
    case '8':
        chan = 4;
        break;
    case '7':
        chan = 3;
        is_float = true;
        break;
    case 'Z':
        chan = 1;
        is_float = true;
        break;
    case 'S':
    case 'T':
    case 'U':
    case 'V':
        // Unsigned short 1,2,3,4 channels
        chan = 1 + Magic2 - 'S';
        is_ushort = true;
        break;
    }
    
    Pix = ImageAlloc();
    InFile.read((char *)Pix, size_bytes());
    
    InFile.close();
    
#ifdef DMC_LITTLE_ENDIAN
    // Intel is little-endian.
    // Always assume they are stored as big-endian.
    if(is_uint || is_float)
        ConvertLong((unsigned int *)Pix, size()*chan);
    if(is_ushort)
        ConvertShort((unsigned short *)Pix, size()*chan);
#endif
    
    if(Verbose) cerr << "Loaded a PPM image.\n";
    
    return DMC_OK;
}

bool ImageLoadSave::SavePPM(const char *fname) const
{
    ASSERTERR(fname, "NULL fname");
    if(Pix==NULL || chan < 1 || wid < 1 || hgt < 1) {
        cerr << "Image is not defined. Not saving.\n";
        return DMC_ERROR;
    }
    
    ASSERTERR(!(chan < 1 || chan > 4), "Can't save a X channel image as a PPM.");
    
    ofstream OutFile(fname, ios::out | ios::binary);
    if(!OutFile.is_open()) {
        return DMC_ERROR;
    }
    
    OutFile << 'P';
    if(is_ushort)
        OutFile << char('S' + chan - 1) << endl;
    else
        OutFile << ((chan==1 ? (is_float?'Z':'5') : (chan==3 ? (is_float?'7':'6') : '8'))) << endl;
    OutFile << wid << " " << hgt << endl << 255 << endl;
    
#ifdef DMC_LITTLE_ENDIAN
    if(is_uint || is_float || is_ushort) {
        // Need to convert multibyte words to big-endian before saving.
        unsigned char *Tmp = new unsigned char[size_bytes()];
        ASSERTERR(Tmp, "memory alloc failed");
        memcpy(Tmp, Pix, size_bytes());
        if(is_ushort)
            ConvertShort((unsigned short *)Tmp, size()*chan);
        else
            ConvertLong((unsigned int *)Tmp, size()*chan);
        OutFile.write((char *)Tmp, size_bytes());
        delete [] Tmp;
    } else
        OutFile.write((char *)Pix, size_bytes());
#else
    OutFile.write((char *)Pix, size_bytes());
#endif
    
    OutFile.close();
    
    if(Verbose) cerr << "Wrote PPM file " << fname << endl;
    
    return DMC_OK;
}

//////////////////////////////////////////////////////
// SGI Iris RGB Images

/* private typedefs */
struct rawImageRec
{
    unsigned short imagic;
    unsigned short type;
    unsigned short dim;
    unsigned short sizeX, sizeY, sizeZ;
    unsigned long min, max;
    unsigned long wasteBytes;
    char name[80];
    unsigned long colorMap;
    
    // Not part of image header.
    FILE *file;
    unsigned char *tmp;
    unsigned long rleEnd;
    unsigned int *rowStart;
    int *rowSize;
};

static void RawImageGetRow(rawImageRec *raw, unsigned char *buf, int y, int z)
{
    unsigned char *iPtr, *oPtr, pixel;
    int count;
    
    if((raw->type & 0xFF00)==0x0100) {
        fseek(raw->file, raw->rowStart[y+z*raw->sizeY], SEEK_SET);
        fread(raw->tmp, 1, (unsigned int)raw->rowSize[y+z*raw->sizeY],
            raw->file);
        
        iPtr = raw->tmp;
        oPtr = buf;
        while (1) {
            pixel = *iPtr++;
            count = (int)(pixel & 0x7F);
            if(!count) {
                return;
            }
            if(pixel & 0x80) {
                while (count--) {
                    *oPtr++ = *iPtr++;
                }
            } else {
                pixel = *iPtr++;
                while (count--) {
                    *oPtr++ = pixel;
                }
            }
        }
    } else {
        fseek(raw->file, 512+(y*raw->sizeX)+(z*raw->sizeX*raw->sizeY),
            SEEK_SET);
        fread(buf, 1, raw->sizeX, raw->file);
    }
}

bool ImageLoadSave::LoadRGB(const char *fname)
{
    rawImageRec raw;
    unsigned char *tmpR, *tmpG, *tmpB;
    
    bool swapFlag = AmLittleEndian();
    
    // Open the file
    if((raw.file = fopen(fname, "rb"))==NULL) {
        cerr << "LoadRGB() failed: can't open image file " << fname << endl;
        return DMC_ERROR;
    }
    
    fread(&raw, 1, 104, raw.file);
    
    if(Verbose) cerr << "ImageLoadSave name is: `" << raw.name << "'\n";
    
    if(swapFlag) {
        ConvertShort(&raw.imagic, 6);
    }
    
    raw.tmp = new unsigned char[raw.sizeX*256];
    tmpR = new unsigned char[raw.sizeX*256];
    tmpG = new unsigned char[raw.sizeX*256];
    tmpB = new unsigned char[raw.sizeX*256];
    ASSERTERR(raw.tmp && tmpR && tmpG && tmpB, "memory alloc failed");
    
    if((raw.type & 0xFF00)==0x0100) {
        int x = raw.sizeY * raw.sizeZ;
        int y = x * sizeof(unsigned int);
        raw.rowStart = new unsigned int[x];
        raw.rowSize = new int[x];
        ASSERTERR(raw.rowStart && raw.rowSize, "memory alloc failed");
        
        raw.rleEnd = 512 + (2 * y);
        fseek(raw.file, 512, SEEK_SET);
        fread(raw.rowStart, 1, y, raw.file);
        fread(raw.rowSize, 1, y, raw.file);
        if(swapFlag) {
            ConvertLong(raw.rowStart, x);
            ConvertLong((unsigned int *)raw.rowSize, x);
        }
    }
    
    wid = raw.sizeX;
    hgt = raw.sizeY;
    chan = raw.sizeZ;
    is_uint = false;
    is_float = false;
    
    Pix = ImageAlloc();
    if((raw.type & 0xFF00)==0x0100) {
        if(Verbose) cerr << "Loading an rle compressed RGB image.\n";
    } else {
        if(Verbose) cerr << "Loading a raw RGB image.\n";
    }
    
    unsigned char *ptr = Pix;
    for (int i = raw.sizeY - 1; i >= 0; i--) {
        if(chan==1) {
            RawImageGetRow(&raw, ptr, i, 0);
            ptr += wid;
        } else {
            RawImageGetRow(&raw, tmpR, i, 0);
            RawImageGetRow(&raw, tmpG, i, 1);
            RawImageGetRow(&raw, tmpB, i, 2);
            
            // Copy into standard RGB image.
            for (int j = 0; j < raw.sizeX; j++) {
                *ptr++ = tmpR[j];
                *ptr++ = tmpG[j];
                *ptr++ = tmpB[j];
            }
        }
    }
    
    fclose(raw.file);
    
    delete [] raw.tmp;
    delete [] tmpR;
    delete [] tmpG;
    delete [] tmpB;
    delete [] raw.rowStart;
    delete [] raw.rowSize;
    
    return DMC_OK;
}

//////////////////////////////////////////////////////
// JPEG File Format

#ifdef DMC_USE_JPEG

bool ImageLoadSave::LoadJPEG(const char *fname)
{
#define NUM_ROWS 16
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE *infile;
    unsigned int y;
    JSAMPROW row_ptr[NUM_ROWS];
    
    if((infile = fopen(fname, "rb"))==NULL)
    {
        cerr << "can't open JPEG file " << fname << endl;
        return DMC_ERROR;
    }
    
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    
    jpeg_start_decompress(&cinfo);
    
    wid = cinfo.output_width;
    hgt = cinfo.output_height;
    chan = cinfo.output_components;
    is_uint = false;
    is_float = false;
    Pix = ImageAlloc();
    
    while(cinfo.output_scanline < cinfo.output_height) {
        for(y=0; y<NUM_ROWS; y++)
            row_ptr[y] = &Pix[(cinfo.output_scanline + y) * wid * chan];
        jpeg_read_scanlines(&cinfo, row_ptr, NUM_ROWS);
    }
    
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    
    fclose(infile);
    
    return DMC_OK;
}

bool ImageLoadSave::SaveJPEG(const char *fname) const
{
    if(Pix==NULL || chan < 1 || wid < 1 || hgt < 1) {
        cerr << "Image is not defined. Not saving.\n";
        return DMC_ERROR;
    }
    
    if(chan != 1 && chan != 3) {
        cerr << "Can't save a " << chan << " channel image as a JPEG.\n";
        return DMC_ERROR;
    }
    
    
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE *outfile;
    int y;
    JSAMPROW row_ptr[1];
    
    if((outfile = fopen(fname, "wb"))==NULL) {
        cerr << "SaveJPEG() failed: can't write to " << fname << endl;
        return DMC_ERROR;
    }
    
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    
    jpeg_stdio_dest(&cinfo, outfile);
    
    cinfo.image_width = wid;
    cinfo.image_height = hgt;
    cinfo.input_components = chan;
    cinfo.in_color_space = (chan==1) ? JCS_GRAYSCALE : JCS_RGB;
    
    jpeg_set_defaults(&cinfo);
    
    jpeg_start_compress(&cinfo, TRUE);
    
    for(y=0; y<hgt; y++) {
        row_ptr[0] = &Pix[y*wid*chan];
        jpeg_write_scanlines(&cinfo, row_ptr, 1);
    }
    
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    
    fclose(outfile);
    
    return DMC_OK;
}

#else /* DMC_USE_JPEG */

bool ImageLoadSave::LoadJPEG(const char *fname)
{
    cerr << "JPEG Support not compiled in." << endl;
    return DMC_ERROR;
}

bool ImageLoadSave::SaveJPEG(const char *fname) const
{
    cerr << "JPEG Support not compiled in." << endl;
    return DMC_ERROR;
}

#endif /* DMC_USE_JPEG */

//////////////////////////////////////////////////////
// TIFF File Format

#ifdef DMC_USE_TIFF

// Compression mode constants
#define NONE 1
#define LEMPELZIV 5

// Returns DMC_OK on success and DMC_ERROR on failure.
bool ImageLoadSave::LoadTIFF(const char *fname)
{
    TIFF* tif; // tif file handler
    
#ifdef DMC_DEBUG
    cerr << "Attempting to open " << fname << " as TIFF.\n";
    cerr << "TIFF version is " << TIFFGetVersion() << endl;
#endif
    
    tif = TIFFOpen(fname, "r");
    if(!tif) {
        cerr << "Could not open TIFF file '" << fname << "'.\n";
        return DMC_ERROR;
    }
    
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &wid);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &hgt);
    TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &chan);
#ifdef DMC_MACHINE_sgi
    chan = chan >> 16;
#endif
    int bitspersample = -1;
    TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bitspersample);
    bitspersample = bitspersample & 0xffff;
    
#ifdef DMC_DEBUG
    if(Verbose) cerr << "size=" << wid <<"x"<< hgt << " TIFFTAG_SAMPLESPERPIXEL=" << chan << endl;
    int tmp = 0;
    int bitspersample = 0;
    cerr << "TIFFTAG_BITSPERSAMPLE " << bitspersample << endl;
    TIFFGetField(tif, TIFFTAG_COMPRESSION, &tmp);
    cerr << "TIFFTAG_COMPRESSION " << tmp << endl;
    TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &tmp);
    cerr << "TIFFTAG_PHOTOMETRIC " << tmp << endl;
    TIFFGetField(tif, TIFFTAG_EXTRASAMPLES, &tmp);
    cerr << "TIFFTAG_EXTRASAMPLES " << tmp << endl;
    
    TIFFPrintDirectory(tif, stderr, 0);
#endif

    is_uint = false;
    is_ushort = false;
    is_float = false;
    int bytespersample = bitspersample / 8;
    if(bitspersample == 32) is_float = true;
    if(bitspersample == 16) is_ushort = true;

    // The TIFF library can save float TIFF, but not load them.
    if(bitspersample > 16)
        return DMC_ERROR;

    // XXX How do we distinguish a uint from a float image?
 
    // Loads the data into a 32-bit word for each pixel.
    uint32 *ABGR = (uint32*) _TIFFmalloc(size() * bytespersample * 4); // buffer for image data
    if(ABGR==NULL)
        return DMC_ERROR;
    if(!TIFFReadRGBAImage(tif, wid, hgt, ABGR, 0))
        return DMC_ERROR;
    
    Pix = ImageAlloc();
    
    unsigned char *a = (unsigned char *) ABGR;
    unsigned char *row;
    int k, x;
    
    // If it's not four channels, we need to pack it more tightly.
    // Move forward through the returned data and backward through Pix.
#ifdef DMC_LITTLE_ENDIAN
    if(is_float) {
        memcpy(Pix, ABGR, wid * hgt * chan * bytespersample);
    } else 
    switch(chan) {
    case 1:
        for (k=hgt-1; k>=0; k--) {
            row = &Pix[k*wid];
            for(x=0; x<wid; x++) {
                row[x] = *a; a += 4;
            }
        }
        break;
    case 2:
        for (k=hgt-1; k>=0; k--) {
            row = &Pix[k*wid*2];
            for(x=0; x<2*wid; ) {
                row[x++] = *(a+0);
                row[x++] = *(a+3);
                a += 4;
            }
        }
        break;
    case 3:
        for (k=hgt-1; k>=0; k--) {
            row = &Pix[k*wid*3];
            for(x=0; x<3*wid; ) {
                row[x++] = *(a+0);
                row[x++] = *(a+1);
                row[x++] = *(a+2);
                a += 4;
            }
        }
        break;
    case 4:
        for (k=hgt-1; k>=0; k--) {
            row = &Pix[k*wid*4];
            memcpy(row, a, wid*4);
            a += wid*4;
        }
        break;
    }
#else
    switch(chan) {
    case 1:
        a += 3;
        for (k=hgt-1; k>=0; k--) {
            row = &Pix[k*wid];
            for(x=0; x<wid; x++) {
                row[x] = *a; a += 4;
            }
        }
        break;
    case 2:
        for (k=hgt-1; k>=0; k--) {
            row = &Pix[k*wid*2];
            for(x=0; x<2*wid; ) {
                row[x++] = *(a+3);
                row[x++] = *(a+0);
                a += 4;
            }
        }
        break;
    case 3:
        for (k=hgt-1; k>=0; k--) {
            row = &Pix[k*wid*3];
            for(x=0; x<3*wid; ) {
                row[x++] = *(a+3);
                row[x++] = *(a+2);
                row[x++] = *(a+1);
                a += 4;
            }
        }
        break;
    case 4:
        for (k=hgt-1; k>=0; k--) {
            row = &Pix[k*wid*4];
            for(x=0; x<4*wid; ) {
                row[x++] = *(a+3);
                row[x++] = *(a+2);
                row[x++] = *(a+1);
                row[x++] = *(a);
                a += 4;
            }
        }
        break;
    }
#endif
    
    _TIFFfree(ABGR);
    
#ifdef DMC_DEBUG
    int dircount = 0;
    do {
        dircount++;
    } while (TIFFReadDirectory(tif));
    if(dircount > 1)
        cerr << "**** Contains " << dircount << " directories.\n";
#endif
    
    TIFFClose(tif);
    return 0;
}

// Handles 1 through 4 channels, uchar, ushor, uint, or float.
bool ImageLoadSave::SaveTIFF(const char *fname) const
{
    if(Pix==NULL || chan < 1 || wid < 1 || hgt < 1) {
        cerr << "Image is not defined. Not saving.\n";
        return DMC_ERROR;
    }
    
    TIFF *tif = TIFFOpen(fname, "w");
    if(tif==NULL)
        return DMC_ERROR;
    
    int bitsperchan = 8;
    if(is_float || is_uint) bitsperchan = 32;
    else if(is_ushort) bitsperchan = 16;
    int bytesperchan = bitsperchan / 8;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, wid);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, hgt);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bitsperchan);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, LEMPELZIV);
    // TIFFSetField(tif, TIFFTAG_COMPRESSION, NONE);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, (chan > 2) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, chan);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_XRESOLUTION, 96.0);
    TIFFSetField(tif, TIFFTAG_YRESOLUTION, 96.0);
    TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT, 2);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, 1);
    // Actually isn't associated, but the library breaks.
    uint16 extyp = EXTRASAMPLE_ASSOCALPHA;
    if(chan==2 || chan==4)
        TIFFSetField(tif, TIFFTAG_EXTRASAMPLES, 1, &extyp);
    
    unsigned char *c = Pix;
    
    // Write one row of the image at a time
    for (int l = 0; l < hgt; l++) {
        if(TIFFWriteScanline(tif, c, l, 0) < 0) {
            TIFFClose(tif);
            return DMC_ERROR;
        }
        c += wid*chan*bytesperchan;
    }
    
    // Close the file and return OK
    TIFFClose(tif);
    
    return DMC_OK;
}

#else /* DMC_USE_TIFF */

bool ImageLoadSave::LoadTIFF(const char *fname)
{
    cerr << "TIFF Support not compiled in." << endl;
    return DMC_ERROR;
}

bool ImageLoadSave::SaveTIFF(const char *fname) const
{
    cerr << "TIFF Support not compiled in." << endl;
    return DMC_ERROR;
}

#endif /* DMC_USE_TIFF */

//////////////////////////////////////////////////////
// PNG File Format

#ifdef DMC_USE_PNG 

static double guess_display_gamma()
{
    /* Try to guess a good value for the display exponent */
    /* Taken from rpng program by Greg Roelofs.
    * Copyright (c) 1998-2000 Greg Roelofs. All rights reserved.
    * This product includes software developed by Greg Roelofs
    * and contributors for the book, "PNG: The Definitive Guide,"
    * published by O'Reilly and Associates.
    */
    double LUT_exponent; /* just the lookup table */
    double CRT_exponent = 2.2; /* just the monitor */
    double default_display_exponent; /* whole display system */
    double display_exponent; 
    
    /* First set the default value for our display-system exponent, i.e.,
    * the product of the CRT exponent and the exponent corresponding to
    * the frame-buffer's lookup table (LUT), if any. This is not an
    * exhaustive list of LUT values (e.g., OpenStep has a lot of weird
    * ones), but it should cover 99% of the current possibilities. And
    * yes, these ifdefs are completely wasted in a Windows program... */
    
#if defined(NeXT)
    LUT_exponent = 1.0 / 2.2;
    /*
    if(some_next_function_that_returns_gamma(&next_gamma))
    LUT_exponent = 1.0 / next_gamma;
    */
#elif defined(sgi)
    LUT_exponent = 1.0 / 1.7;
    /* there doesn't seem to be any documented function to get the
    * "gamma" value, so we do it the hard way */
    FILE *infile = fopen("/etc/config/system.glGammaVal", "r");
    if(infile) {
        double sgi_gamma;
        char tmpline[90];
        
        fgets(tmpline, 80, infile);
        fclose(infile);
        sgi_gamma = atof(tmpline);
        if(sgi_gamma > 0.0)
            LUT_exponent = 1.0 / sgi_gamma;
    }
#elif defined(Macintosh)
    LUT_exponent = 1.8 / 2.61;
    /*
    if(some_mac_function_that_returns_gamma(&mac_gamma))
    LUT_exponent = mac_gamma / 2.61;
    */
#else
    LUT_exponent = 1.0; /* assume no LUT: most PCs */
#endif
    
    /* the defaults above give 1.0, 1.3, 1.5 and 2.2, respectively: */
    default_display_exponent = LUT_exponent * CRT_exponent;
    
    /* If the user has set the SCREEN_GAMMA environment variable as suggested
    * (somewhat imprecisely) in the libpng documentation, use that; otherwise
    * use the default value we just calculated. Either way, the user may
    * override this via a command-line option. */
    
    char *gamma_str;
    if((gamma_str = getenv("SCREEN_GAMMA")) != NULL)
        display_exponent = atof(gamma_str);
    else
        display_exponent = default_display_exponent;
    
    return display_exponent;
}

// Read a PNG file.
bool ImageLoadSave::LoadPNG(const char *fname)
{
    FILE *fp;
    if((fp = fopen(fname, "rb"))==NULL) {
        cerr << "can't open PNG file " << fname << endl;
        return DMC_ERROR;
    }
    // Check magic number
    unsigned char buf[4];
    fread(buf, 1, 4, fp);
    if(png_sig_cmp(buf, 0, 4)) {
        cerr << fname << " is not a PNG file" << endl;
        return DMC_ERROR;
    }
    
    /* Create and initialize the png_struct with the desired error handler
    * functions. If you want to use the default stderr and longjump method,
    * you can supply NULL for the last three parameters. We also supply the
    * the compiler header file version, so that we know if the application
    * was compiled with a compatible version of the library. REQUIRED
    */
    png_structp png_ptr = png_create_read_struct
        (PNG_LIBPNG_VER_STRING, (png_voidp)NULL, NULL, NULL);
    if(!png_ptr) {
        fclose(fp);
        return DMC_ERROR;
    }
    
    /* Allocate/initialize the memory for image information. REQUIRED. */
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if(!info_ptr) {
        fclose(fp);
        png_destroy_read_struct(&png_ptr,
            (png_infopp)NULL, (png_infopp)NULL);
        return DMC_ERROR;
    }
    
    /* Set error handling if you are using the setjmp/longjmp method (this is
    * the normal method of doing things with libpng). REQUIRED unless you
    * set up your own error handlers in the png_create_read_struct() earlier.
    */
    if(setjmp(png_jmpbuf(png_ptr))) {
        /* Free all of the memory associated with the png_ptr and info_ptr */
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        fclose(fp);
        /* If we get here, we had a problem reading the file */
        return (DMC_ERROR);
    }
    
    /* Set up the input control using standard C streams */
    png_init_io(png_ptr, fp);
    
    /* Let pnglib know that we already read some of the signature */
    png_set_sig_bytes(png_ptr, 4);
    
    /* Read file info */
    png_read_info(png_ptr, info_ptr);
    
    /* Parse info_ptr */
    wid = png_get_image_width(png_ptr, info_ptr);
    hgt = png_get_image_height(png_ptr, info_ptr);
    chan = png_get_channels(png_ptr, info_ptr);
    
    int color_type = png_get_color_type(png_ptr, info_ptr);
    
    /* Expand paletted colors into true RGB triplets */
    if(color_type==PNG_COLOR_TYPE_PALETTE) {
        png_set_expand(png_ptr);
        chan = 3;
    }
    
    /* Expand paletted or RGB images with transparency to full alpha channels
    * so the data will be available as RGBA quartets.
    */
    if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
        png_set_expand(png_ptr);
        chan = 4;
    }
    
    /* Allocate image buffer */
    is_uint = false;
    is_float = false;
    Pix = ImageAlloc();
    
    /* Tell lib we can't handle 16 bit channels */
    png_set_strip_16(png_ptr);
    /* Extract multiple pixels with bit depths of 1, 2, and 4 from a single
    * byte into separate bytes (useful for paletted and grayscale images).
    */
    png_set_packing(png_ptr);
    
    double gamma; // gamma of image
    // If this png doesn't set gamma, we shouldn't play with gamma either
    if(png_get_gAMA(png_ptr, info_ptr, &gamma)) {
        double display_gamma = guess_display_gamma(); // exponent of screen
        png_set_gamma(png_ptr, display_gamma, gamma);
    }
    
    /* Set up row pointers to hand to libpng */
    png_bytep* row_pointers = new png_bytep[hgt];
    unsigned int row_stride = wid*chan;
    unsigned char *rowptr = Pix;
    for (unsigned int row = 0; row < hgt; row++) {
        row_pointers[row] = rowptr;
        rowptr += row_stride;
    }
    /* Read the whole thing */
    png_read_image(png_ptr, row_pointers);
    
    /* clean up after the read, and free any memory allocated - REQUIRED */
    delete [] row_pointers;
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
    
    /* close the file */
    fclose(fp);
    
    /* that's it */
    return (DMC_OK);
}

// Write a PNG file
bool ImageLoadSave::SavePNG(const char *fname) const
{
    if(Pix==NULL || chan < 1 || wid < 1 || hgt < 1) {
        cerr << "Image is not defined. Not saving.\n";
        return DMC_ERROR;
    }
    if(!fname || !fname[0]) {
        cerr << "Filename not specified. Not saving.\n";
        return DMC_ERROR;
    }
    FILE *fp;
    if((fp = fopen(fname, "wb"))==NULL) {
        cerr << "SavePNG() failed: can't write to " << fname << endl;
        return DMC_ERROR;
    }
    
    /* Allocate write structures */
    png_structp png_ptr = png_create_write_struct
        (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!png_ptr) {
        fclose(fp);
        return (DMC_ERROR);
    }
    
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if(!info_ptr) {
        png_destroy_write_struct(&png_ptr,
            (png_infopp)NULL);
        fclose(fp);
        return (DMC_ERROR);
    }
    
    /* Setup error handling */
    if(setjmp(png_jmpbuf(png_ptr))) {
        /* If we get here, we had a problem reading the file */
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        return (DMC_ERROR);
    }
    
    /* Setup file IO */
    png_init_io(png_ptr, fp);
    
    /* Set file options */
    int chan2color_type[] = 
    { 0, PNG_COLOR_TYPE_GRAY, PNG_COLOR_TYPE_GRAY_ALPHA,
    PNG_COLOR_TYPE_RGB, PNG_COLOR_TYPE_RGB_ALPHA
    };
    int bit_depth = 8;
    png_set_IHDR(png_ptr, info_ptr, wid, hgt, bit_depth, 
        chan2color_type[chan],
        PNG_INTERLACE_NONE,
        // PNG_INTERLACE_ADAM7,
        PNG_COMPRESSION_TYPE_BASE, 
        PNG_FILTER_TYPE_BASE);
    
    // Optional gamma chunk is strongly suggested if you have any guess
    // as to the correct gamma of the image. (we don't have a guess)
    //
    // png_set_gAMA(png_ptr, info_ptr, image_gamma);
    
    // Write the file header information. REQUIRED
    png_write_info(png_ptr, info_ptr);
    
    /* The easiest way to write the image (you may have a different memory
    * layout, however, so choose what fits your needs best). You need to
    * use the first method if you aren't handling interlacing yourself.
    */
    png_bytep* row_pointers = new png_bytep[hgt];
    unsigned int row_stride = wid*chan;
    unsigned char *rowptr = Pix;
    for (unsigned int row = 0; row < hgt; row++) {
        row_pointers[row] = rowptr;
        rowptr += row_stride;
    }
    /* write out the entire image data in one call */
    png_write_image(png_ptr, row_pointers);
    
    /* It is REQUIRED to call this to finish writing the rest of the file */
    png_write_end(png_ptr, info_ptr);
    
    /* clean up after the write, and free any memory allocated */
    png_destroy_write_struct(&png_ptr, &info_ptr);
    
    /* close the file */
    fclose(fp);
    
    return DMC_OK;
}

#else /* DMC_USE_PNG */

bool ImageLoadSave::LoadPNG(const char *fname)
{
    cerr << "PNG Support not compiled in." << endl;
    return DMC_ERROR;
}

bool ImageLoadSave::SavePNG(const char *fname) const
{
    cerr << "PNG Support not compiled in." << endl;
    return DMC_ERROR;
}

#endif /* DMC_USE_PNG */

//////////////////////////////////////////////////////
// MAT File Format

#ifdef DMC_USE_MAT 

// Read a MAT file.
bool ImageLoadSave::LoadMAT(const char *fname)
{
    MATFile *pmat;
    mxArray *pa;

    // open file and verify its contents with matGetArray
    pmat = matOpen(fname, "r");
    if(pmat == NULL) {
        cerr << "Error opening fname " << fname << endl;
        return(DMC_ERROR);
    }
    
    // Read in array
    pa = matGetNextArray(pmat);
    if(pa == NULL) {
        cerr << "Error reading matrix.\n";
        return(DMC_ERROR);
    }
    if(mxGetNumberOfDimensions(pa) != 2) {
        cerr << "Error: matrix does not have two dimensions.\n";
        return(DMC_ERROR);
    }
   
    // Copy the data out.
    // XXX
    ASSERT0(0);

    mxDestroyArray(pa);
    
    if(matClose(pmat)) {
        cerr << "Error closing file.\n";
        return(DMC_ERROR);
    }
    
    return DMC_OK;
}

// Write a MAT file
bool ImageLoadSave::SaveMAT(const char *fname) const
{
    if(Pix==NULL || chan < 1 || wid < 1 || hgt < 1) {
        cerr << "Image is not defined. Not saving.\n";
        return DMC_ERROR;
    }
    if(!fname || !fname[0]) {
        cerr << "Filename not specified. Not saving.\n";
        return DMC_ERROR;
    }
    
    MATFile *pmat;
    mxArray *pa;
    int status; 
    
    cerr << "Saving " << fname << endl;
    pmat = matOpen(fname, "w");
    if(pmat == NULL) {
        cerr << "Error creating fname " << fname << endl;
        return(DMC_ERROR);
    }
    
    mxClassID TheClass = mxUNKNOWN_CLASS;

    if(is_ushort) {
        TheClass = mxUINT16_CLASS;
    } else {
        ASSERT0(0);
    }

    int dims[3];
    dims[0] = hgt;
    dims[1] = wid;
    dims[2] = chan;
    pa = mxCreateNumericArray(chan > 1 ? 3:2, dims, TheClass, mxREAL);
    ASSERTERR(pa, "memory alloc failed");
    mxSetName(pa, "Img");
    
    if(is_ushort) {
        // Transpose the data. I hate this.
        unsigned short *ImTr = (unsigned short *)mxGetPr(pa);
        unsigned short *usPix = (unsigned short *)Pix;
        int k = 0;
        for(int y=0; y<hgt; y++) {
            for(int x=0; x<wid; x++) {
                for(int c=0; c<chan; c++) {
                    ImTr[(x * hgt + y) * chan + c] = usPix[k++];
                }
            }
        }
    } else {
        ASSERT0(0);
    }

    status = matPutArray(pmat, pa);
    if(status) {
        cerr << "Error using matPutArray\n";
        return(DMC_ERROR);
    } 
    
    mxDestroyArray(pa);
    
    if(matClose(pmat) != 0) {
        cerr << "Error closing file.\n";
        return(DMC_ERROR);
    }
    
    return DMC_OK;
}

#else /* DMC_USE_MAT */

bool ImageLoadSave::LoadMAT(const char *fname)
{
    cerr << "MAT Support not compiled in." << endl;
    return DMC_ERROR;
}

bool ImageLoadSave::SaveMAT(const char *fname) const
{
    cerr << "MAT Support not compiled in." << endl;
    return DMC_ERROR;
}

#endif /* DMC_USE_MAT */

//////////////////////////////////////////////////////
// HDR (RGBE) File Format

#ifdef DMC_USE_HDR

// Currently this loads and saves f3Images, not rgbeImages.
// I will add this later.
bool ImageLoadSave::LoadHDR(const char* fname)
{
    FILE *filep = fopen(fname, "rb");
    if(filep == NULL) {
        cerr << "LoadHDR : Unable to load HDR.\n";
        return DMC_ERROR;
    }
    
    int i, j, row;
    float exposure;
    
    /*a very basic RADIANCE pic file header */
    char line[1000];
    fgets(line, 1000, filep);
    if(!strcmp(line, "#?RADIANCE")) {
        cerr << "Not a HDR file.\n";
        return DMC_ERROR;
    }
    
    fgets(line, 1000, filep);
    fgets(line, 1000, filep);
    fscanf(filep, "EXPOSURE=%f\n", &exposure);
    // cerr << "Reading HDR file with exposure " << exposure << endl;
    //fgets(line, 1000, filep);
    fscanf(filep, "-Y %d +X %d\n", &hgt, &wid);
    // cerr << wid << "x" << hgt << endl;
    is_uint = false;
    is_float = true;
    chan = 3;
    Pix = ImageAlloc();
    
    RGBE *helpit = new RGBE[wid];
    
    /*Allocate enough space for one row of COLOR at a time */
    COLOR* oneRow = (COLOR*)malloc(sizeof(COLOR)*wid);
    
    float invexp = 1.0f / exposure;
    
    /* Convert separated channel representation to per pixel representation */
    int k=0;
    float *P = (float *)Pix;
    for (row=0;row<hgt;row++) { 
        freadscan(oneRow, helpit, wid, filep); 
        for(i=0;i<wid;i++) { 
            for(j=0;j<3;j++) { 
                P[k] = oneRow[i][j] * invexp;
                k++;
            } 
        } 
    } 
    fclose(filep);
    
    free(oneRow);
    delete [] helpit;
    
    return DMC_OK;
}

// int freadscan(register COLOR  *scanline, int len, FILE  *fp) /* read in a scanline */

// XXX Here's a global variable. It's totally evil. Use it to set the outgoing exposure.
float DMcExposureGlobal = 1.0f;

bool ImageLoadSave::SaveHDR(const char* fname) const
{
    ASSERT0(is_float && chan==3);
    ASSERT0(Pix);

    FILE *filep = fopen(fname, "wb");
    if(filep == NULL) {
        cerr << "SaveHDR : Unable to save.\n";
        return true;
    }
    
    char* comments = "no comment";
    
    /*Allocate enough space for one row of COLOR at a time */
    COLOR* oneRow;
    
    oneRow = (COLOR*)malloc(sizeof(COLOR)*wid);
    float exposure;
    // exposure = 0.52/getApproxMedian();
    // exposure = 1.0f; // Give a constant default exposure (and save time).
    exposure = DMcExposureGlobal; // Use the evil global exposure value.
    
    /*a very basic RADIANCE pic file header */
    fprintf(filep,"#?RADIANCE\n");
    fprintf(filep,"# %s\n",comments);
    fprintf(filep,"FORMAT=32-bit_rle_rgbe\n");
    fprintf(filep,"EXPOSURE=%25.13f\n",exposure);
    fprintf(filep,"\n");
    fprintf(filep,"-Y %d +X %d\n",hgt, wid);
    
    RGBE *helpit = new RGBE[wid];
    
    /* Convert separated channel representation to per pixel representation */
    // int k=0;
    float *P = (float *)Pix;
    for (int row=0;row<hgt;row++) { 
#if 0
        int i, j, row;
        for(i=0;i<wid;i++) { 
            for(j=0;j<3;j++) { 
                oneRow[i][j] = P[k]; // *exposure; 
                k++;
            } 
        } 
        // fwritescan(oneRow, helpit, wid, filep); 
#endif
        fwritescan((COLOR*)&P[row*wid*3], helpit, wid, filep); 
    } 
    fclose(filep);
    
    free(oneRow);
    delete [] helpit;
    
    cerr << "Wrote out HDR file with exposure " << exposure << endl;
    
    return false;
}

#else /* DMC_USE_HDR */

bool ImageLoadSave::LoadHDR(const char *fname)
{
    cerr << "HDR Support not compiled in." << endl;
    return DMC_ERROR;
}

bool ImageLoadSave::SaveHDR(const char *fname) const
{
    cerr << "HDR Support not compiled in." << endl;
    return DMC_ERROR;
}

#endif /* DMC_USE_HDR */

