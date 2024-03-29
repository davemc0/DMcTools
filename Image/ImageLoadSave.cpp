//////////////////////////////////////////////////////////////////////
// ImageLoadSave.cpp - Load and save images of many file formats
//
// Copyright David K. McAllister, Aug. 1997 - 2007.

#include "Image/ImageLoadSave.h"

#include "Image/RGBEio.h"
#include "Image/tImage.h"
#include "Util/Utils.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#ifdef DMC_USE_TIFF
#include "tiffio.h"
#endif

#ifdef DMC_USE_MAT
extern "C" {
#include "mat.h"
}
#endif

#include <fstream>
#include <string>

namespace {
const int RAS_MAGIC = 0x59a66a95;
const int GIF_MAGIC = 0x47494638;
const int JPEG_MAGIC = 0xffd8ffe0;
const int JPEG1_MAGIC = 0xffd8ffe1;
const int RGB_MAGIC = 0x01da0101;

const int BMP_ = 0x00706d62; // "bmp\0", etc.
const int GIF_ = 0x00666967;
const int HDR_ = 0x00726468;
const int JPE_ = 0x0065706a;
const int JPG_ = 0x0067706a;
const int MAT_ = 0x0074616d;
const int PNG_ = 0x00676e70;
const int PAM_ = 0x006d6170;
const int PFM_ = 0x006d6670;
const int PGM_ = 0x006d6770;
const int PPM_ = 0x006d7070;
const int PSM_ = 0x006d7370;
const int PZM_ = 0x006d7a70;
const int RAS_ = 0x00736172;
const int RGB_ = 0x00626772;
const int TGA_ = 0x00616774;
const int TIF_ = 0x00666974;

// Return an int whose bytes equal the characters of the extension string
int GetExtensionVal(const std::string& fname)
{
    char* ext = GetFileExtension(fname);
    ToLower(ext);
    return (ext[0] << 0) | (ext[1] << 8) | (ext[2] << 16);
}
}; // namespace

// Destroy an ImageLoadSave.
ImageLoadSave ::~ImageLoadSave()
{
    // If LoadtImage() was called, it steals the baseImg and sets it to NULL.
    // If tImage.Load() was called it lets this destructor destroy the baseImg, though it may steal baseImg's raster first.
    // If an error gets thrown while loading, this destructor gets called and baseImg is not NULL.
    // If an error gets thrown while saving, this destructor gets called and baseImg is NULL, so destroys nothing.
    // The saver doesn't own any of the data it points to.
    if (baseImg) delete baseImg;
}

// Fills wid, hgt, chan, Pix, and baseImg.
// Pix points to the same data as does baseImg.Pix.
// The data must be deleted by baseImg.
// baseImg must also be deleted by the caller (tLoad or LoadtImage).
// Pix and baseImg will be NULL on error.
void ImageLoadSave::Load(const std::string& fname)
{
    ASSERT_R(!fname.empty());
    ASSERT_R(Pix == NULL && wid == 0 && hgt == 0 && chan == 0);

    int exts = GetExtensionVal(fname.c_str());

    std::ifstream InFile(fname, std::ios::in | std::ios::binary);
    if (!InFile.is_open()) throw DMcError("Failed to open file '" + std::string(fname) + "'");

    unsigned int Magic;
    char* Mag = (char*)&Magic;
    InFile.read(Mag, 4);
    InFile.close();
    unsigned int eMagic = Magic;
    ConvertLong(&eMagic, 1);

    if (Magic == RAS_MAGIC || eMagic == RAS_MAGIC || exts == RAS_) {
        LoadRas(fname);
    } else if (Magic == GIF_MAGIC || eMagic == GIF_MAGIC || exts == GIF_) {
        LoadGIF(fname);
    } else if (Magic == JPEG_MAGIC || eMagic == JPEG_MAGIC || exts == JPG_ || exts == JPE_) {
        LoadJPEG(fname);
    } else if (Magic == RGB_MAGIC || eMagic == RGB_MAGIC || exts == RGB_) {
        LoadRGB(fname);
    } else if ((Mag[0] == 'P' && (Mag[1] == '5' || Mag[1] == '6' || Mag[1] == '7' || Mag[1] == '8' || Mag[1] == 'Z')) ||
               (Mag[3] == 'P' && (Mag[2] == '5' || Mag[2] == '6' || Mag[2] == '7' || Mag[2] == '8' || Mag[2] == 'Z')) || exts == PPM_ || exts == PGM_ ||
               exts == PAM_ || exts == PFM_ || exts == PSM_ || exts == PZM_) {
        LoadPPM(fname);
    } else if ((Mag[1] == 'P' && Mag[2] == 'N' && Mag[3] == 'G') || (Mag[2] == 'P' && Mag[1] == 'N' && Mag[0] == 'G') || exts == PNG_) {
        LoadPNG(fname);
    } else if ((Mag[0] == 'B' && Mag[1] == 'M') || (Mag[3] == 'B' && Mag[2] == 'M') || exts == BMP_) {
        LoadBMP(fname);
    } else if ((Mag[0] == 0 || Mag[3] == 0) && (exts == TGA_)) {
        LoadTGA(fname);
    } else if (exts == TIF_) {
        LoadTIFF(fname);
    } else if (exts == HDR_) {
        LoadRGBE(fname);
    } else {
        std::string er = std::string("Could not determine file type of `") + fname + "'.\n";
        er += "Magic was " + std::to_string(Magic) + " or " + std::to_string(eMagic) + " or `" + Mag[0] + Mag[1] + Mag[2] + Mag[3] + "'.\n";
        er += "Extension was " + std::to_string(exts) + '\n';
        throw DMcError(er);
    }
    Pix = NULL; // When loading, Pix is only used by Load*(). baseImg carries the data when we return from here.
}

// Allocate a tImage of the appropriate type for the file being loaded.
// Called by Load*(). It creates a tImage that matches the is_* and chan args. Stores the pointer to the tImage in baseImg.
unsigned char* ImageLoadSave::ImageAlloc()
{
    switch (chan) {
    case 1:
        if (is_uint) {
            baseImg = new ui1Image(wid, hgt);
        } else if (is_float) {
            baseImg = new f1Image(wid, hgt);
        } else if (is_ushort) {
            baseImg = new us1Image(wid, hgt);
        } else {
            baseImg = new uc1Image(wid, hgt);
        }
        break;
    case 2:
        if (is_ushort) {
            baseImg = new us2Image(wid, hgt);
        } else {
            baseImg = new uc2Image(wid, hgt);
        }
        break;
    case 3:
        if (is_float) {
            baseImg = new f3Image(wid, hgt);
        } else if (is_ushort) {
            baseImg = new us3Image(wid, hgt);
        } else {
            baseImg = new uc3Image(wid, hgt);
        }
        break;
    case 4:
        if (is_ushort) {
            baseImg = new us4Image(wid, hgt);
        } else {
            baseImg = new uc4Image(wid, hgt);
        }
        break;
    }
    Pix = (unsigned char*)baseImg->pv_virtual();
    return Pix;
}

// Choose a saver based strictly on extension.
// The individual savers may look at chan, is_uint, etc. to decide a format.
void ImageLoadSave::Save(const std::string& fname_) const
{
    ASSERT_R(!fname_.empty());
    char* fname = _strdup(fname_.c_str());
    int exts = GetExtensionVal(fname);

    char* outfname3 = strchr(fname, '\n');
    if (outfname3) *outfname3 = '\0';
    outfname3 = strchr(fname, '\r');
    if (outfname3) *outfname3 = '\0';

    switch (exts) {
    case BMP_:
        SaveBMP(fname); // 1 3
        break;
    case GIF_:
        SaveGIF(fname); // 1 3
        break;
    case HDR_:
        SaveRGBE(fname); // 3f
        break;
    case JPG_:
        SaveJPEG(fname); // 1 3
        break;
    case TGA_:
        SaveTGA(fname); // 1 3 4
        break;
    case TIF_:
        SaveTIFF(fname); // 1 2 3 4 uc, us, ui, f
        break;
    case MAT_:
        SaveMAT(fname); // 1 2 3 4
        break;
    case PNG_:
        SavePNG(fname); // 1 2 3 4
        break;
    case PPM_:
    case PGM_:
    case PAM_:
    case PSM_:
    case PFM_:
    case PZM_:
        SavePPM(fname); // 1s 2s 3s 4s 1f 3f 1uc 3uc 4uc
        break;
    default: throw DMcError("Saving file with unknown filename extension in filename '" + std::string(fname_) + "'"); break;
    }

    if (fname) delete[] fname;
}

//////////////////////////////////////////////////////
// Sun Raster File Format

// RAS Header
struct rasterfile {
    int ras_magic;     /* magic number */
    int ras_width;     /* width (pixels) of image */
    int ras_height;    /* height (pixels) of image */
    int ras_depth;     /* depth (1, 8, or 24 bits) of pixel */
    int ras_length;    /* length (bytes) of image */
    int ras_type;      /* type of file; see RT_* below */
    int ras_maptype;   /* type of colormap; see RMT_* below */
    int ras_maplength; /* length (bytes) of following map */
    /* color map follows for ras_maplength bytes, followed by image */
};

/* Sun supported ras_type's */
#define RT_OLD 0               /* Raw pixrect image in 68000 byte order */
#define RT_STANDARD 1          /* Raw pixrect image in 68000 byte order */
#define RT_BYTE_ENCODED 2      /* Run-length compression of bytes */
#define RT_FORMAT_RGB 3        /* XRGB or RGB instead of XBGR or BGR */
#define RT_FORMAT_TIFF 4       /* tiff <-> standard rasterfile */
#define RT_FORMAT_IFF 5        /* iff (TAAC format) <-> standard rasterfile */
#define RT_EXPERIMENTAL 0xffff /* Reserved for testing */

/* Sun registered ras_maptype's */
#define RMT_RAW 2
/* Sun supported ras_maptype's */
#define RMT_NONE 0      /* ras_maplength is expected to be 0 */
#define RMT_EQUAL_RGB 1 /* red[ras_maplength/3],green[],blue[] */

// NOTES:
// Each line of the image is rounded out to a multiple of 16 bits!!!!!!!!!!!!!!
// This corresponds to the rounding convention used by the memory pixrect
// package (/usr/include/pixrect/memvar.h) of the SunWindows system.
// The ras_encoding field (always set to 0 by Sun's supported software)
// was renamed to ras_length in release 2.0. As a result, rasterfiles
// of type 0 generated by the old software claim to have 0 length; for
// compatibility, code reading rasterfiles must be prepared to compute the
// true length from the width, height, and depth fields.

void ImageLoadSave::LoadRas(const std::string& fname)
{
    // Read a Sun Raster File image.
    std::ifstream InFile(fname, std::ios::in | std::ios::binary);
    if (!InFile.is_open()) throw DMcError("Could not open file for LoadRas: " + std::string(fname));

    rasterfile Hedr;
    InFile.read((char*)&Hedr, sizeof(rasterfile));

#ifdef DMC_LITTLE_ENDIAN
    ConvertLong((unsigned int*)&Hedr, sizeof(rasterfile) / sizeof(int));
#endif

    wid = Hedr.ras_width;
    hgt = Hedr.ras_height;
    chan = 3;

    if (Hedr.ras_depth != 24) throw DMcError("Take your " + std::to_string(Hedr.ras_depth) + " bit image and go away!");
    if (size_bytes() != Hedr.ras_length)
        throw DMcError("Size was " + std::to_string(size_bytes()) + ", but ras_length was " + ::std::to_string(Hedr.ras_length) + ".\n");
    if (wid > 4096) throw DMcError("Too big! " + std::to_string(wid));

    is_uint = false;
    is_float = false;
    Pix = ImageAlloc();

    if (Hedr.ras_type == RT_FORMAT_RGB) {
        if (Hedr.ras_maptype == RMT_NONE) {
            // Now read the color values.
            for (int y = 0; y < hgt; y++) { InFile.read((char*)&Pix[y * wid * 3], wid * 3); }
        } else if (Hedr.ras_maptype == RMT_EQUAL_RGB) {
            if (SP.verbose) std::cerr << "Reading color mapped image. Maplength = " << Hedr.ras_maplength << std::endl;

            unsigned char ColorMap[256][3];
            unsigned char Colors[4096];
            InFile.read((char*)ColorMap, Hedr.ras_maplength * 3);

            for (int y = 0; y < hgt; y++) {
                InFile.read((char*)Colors, wid);

                for (int x = 0; x < wid;) {
                    Pix[y * wid + x] = ColorMap[Colors[x]][0];
                    x++;
                    Pix[y * wid + x] = ColorMap[Colors[x]][1];
                    x++;
                    Pix[y * wid + x] = ColorMap[Colors[x]][2];
                    x++;
                }
            }
        } else {
            throw DMcError("Strange color map scheme.");
        }
    } else if (Hedr.ras_type == RT_STANDARD) {
        if (SP.verbose) std::cerr << "BGR ImageLoadSave (RT_STANDARD)\n";
        if (Hedr.ras_maptype == RMT_NONE) {
            // Now read the color values.
            unsigned char Colors[4096][3];

            int ii = 0;
            for (int y = 0; y < hgt; y++) {
                InFile.read((char*)Colors, wid * 3);

                for (int x = 0; x < wid; x++) {
                    Pix[ii++] = Colors[x][2];
                    Pix[ii++] = Colors[x][1];
                    Pix[ii++] = Colors[x][0];
                }
            }
        } else {
            throw DMcError("Strange color map scheme.");
        }
    } else {
        throw DMcError("Strange format.");
    }

    InFile.close();
}

//////////////////////////////////////////////////////
// PPM File Format

// PAM is four-channel PPM. PFM is one- or three-channel float. PGM is one-channel uchar.
void ImageLoadSave::LoadPPM(const std::string& fname)
{
    std::ifstream InFile(fname, std::ios::in | std::ios::binary);
    if (!InFile.is_open()) throw DMcError("Could not open file for LoadPPM: " + std::string(fname));

    char Magic1, Magic2;
    InFile >> Magic1 >> Magic2;

    if (Magic1 != 'P' ||
        (Magic2 != '5' && Magic2 != '6' && Magic2 != '7' && Magic2 != '8' && Magic2 != 'Z' && Magic2 != 'S' && Magic2 != 'T' && Magic2 != 'U' && Magic2 != 'V')) {
        InFile.close();
        throw DMcError("Not a known PPM file: " + std::string(fname));
    }

    InFile.get();
    char c = InFile.peek();
    while (c == '#') {
        char line[999];
        InFile.getline(line, 1000);
        if (SP.verbose) std::cerr << line << std::endl;
        c = InFile.peek();
    }

    int dyn_range;
    InFile >> wid >> hgt >> dyn_range;
    InFile.get();

    if (dyn_range != 255) throw DMcError("PPM Must be 255.");

    // XXX Need to distinguish one-channel float images.
    is_uint = false;
    is_float = false;
    switch (Magic2) {
    case '5': chan = 1; break;
    case '6': chan = 3; break;
    case '8': chan = 4; break;
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
    InFile.read((char*)Pix, size_bytes());

    InFile.close();

#ifdef DMC_LITTLE_ENDIAN
    // Intel is little-endian.
    // Always assume they are stored as big-endian.
    if (is_uint || is_float) ConvertLong((unsigned int*)Pix, size() * chan);
    if (is_ushort) ConvertShort((unsigned short*)Pix, size() * chan);
#endif

    if (SP.verbose) std::cerr << "Loaded a PPM image.\n";
}

void ImageLoadSave::SavePPM(const std::string& fname) const
{
    ASSERT_RM(fname.c_str(), "NULL fname");
    if (Pix == NULL || chan < 1 || wid < 1 || hgt < 1) { throw DMcError("PPM image is not defined. Not saving."); }

    ASSERT_RM(!(chan < 1 || chan > 4), "Can't save a X channel image as a PPM.");

    std::ofstream OutFile(fname, std::ios::out | std::ios::binary);
    if (!OutFile.is_open()) throw DMcError("Could not open file for SavePPM: " + std::string(fname));

    OutFile << 'P';
    if (is_ushort)
        OutFile << char('S' + chan - 1) << std::endl;
    else if (is_float)
        OutFile << (chan == 1 ? 'Z' : '7') << std::endl; // '7' is f3
    else
        OutFile << (chan == 1 ? '5' : (chan == 3 ? '6' : '8')) << std::endl;
    OutFile << wid << " " << hgt << std::endl << 255 << std::endl;

#ifdef DMC_LITTLE_ENDIAN
    if (is_uint || is_float || is_ushort) {
        // Need to convert multibyte words to big-endian before saving.
        unsigned char* Tmp = new unsigned char[size_bytes()];
        ASSERT_RM(Tmp, "memory alloc failed");
        memcpy(Tmp, Pix, size_bytes());
        if (is_ushort)
            ConvertShort((unsigned short*)Tmp, size() * chan);
        else
            ConvertLong((unsigned int*)Tmp, size() * chan);
        OutFile.write((char*)Tmp, size_bytes());
        delete[] Tmp;
    } else
        OutFile.write((char*)Pix, size_bytes());
#else
    OutFile.write((char*)Pix, size_bytes());
#endif

    OutFile.close();

    if (SP.verbose) std::cerr << "Wrote PPM file " << fname << std::endl;
}

//////////////////////////////////////////////////////
// SGI Iris RGB Images

/* private typedefs */
struct rawImageRec {
    unsigned short imagic;
    unsigned short type;
    unsigned short dim;
    unsigned short sizeX, sizeY, sizeZ;
    unsigned long minv, maxv;
    unsigned long wasteBytes;
    char name[80];
    unsigned long colorMap;

    // Not part of image header.
    FILE* file;
    unsigned char* tmp;
    unsigned long rleEnd;
    unsigned int* rowStart;
    int* rowSize;
};

namespace {
void RawImageGetRow(rawImageRec* raw, unsigned char* buf, int y, int z)
{
    unsigned char *iPtr, *oPtr, pixel;
    int count;

    if ((raw->type & 0xFF00) == 0x0100) {
        fseek(raw->file, raw->rowStart[y + z * raw->sizeY], SEEK_SET);
        fread(raw->tmp, 1, (unsigned int)raw->rowSize[y + z * raw->sizeY], raw->file);

        iPtr = raw->tmp;
        oPtr = buf;
        while (1) {
            pixel = *iPtr++;
            count = (int)(pixel & 0x7F);
            if (!count) { return; }
            if (pixel & 0x80) {
                while (count--) { *oPtr++ = *iPtr++; }
            } else {
                pixel = *iPtr++;
                while (count--) { *oPtr++ = pixel; }
            }
        }
    } else {
        fseek(raw->file, 512 + (y * raw->sizeX) + (z * raw->sizeX * raw->sizeY), SEEK_SET);
        fread(buf, 1, raw->sizeX, raw->file);
    }
}
}; // namespace

void ImageLoadSave::LoadRGB(const std::string& fname)
{
    rawImageRec raw;
    unsigned char *tmpR, *tmpG, *tmpB;

    bool swapFlag = AmLittleEndian();

    // Open the file
    if ((raw.file = fopen(fname.c_str(), "rb")) == NULL) throw DMcError("LoadRGB() failed: can't open image file " + std::string(fname));

    fread(&raw, 1, 104, raw.file);

    if (SP.verbose) std::cerr << "ImageLoadSave name is: `" << raw.name << "'\n";

    if (swapFlag) { ConvertShort(&raw.imagic, 6); }

    raw.tmp = new unsigned char[raw.sizeX * 256];
    tmpR = new unsigned char[raw.sizeX * 256];
    tmpG = new unsigned char[raw.sizeX * 256];
    tmpB = new unsigned char[raw.sizeX * 256];
    ASSERT_RM(raw.tmp && tmpR && tmpG && tmpB, "memory alloc failed");

    if ((raw.type & 0xFF00) == 0x0100) {
        int x = raw.sizeY * raw.sizeZ;
        int y = x * sizeof(unsigned int);
        raw.rowStart = new unsigned int[x];
        raw.rowSize = new int[x];
        ASSERT_RM(raw.rowStart && raw.rowSize, "memory alloc failed");

        raw.rleEnd = 512 + (2 * y);
        fseek(raw.file, 512, SEEK_SET);
        fread(raw.rowStart, 1, y, raw.file);
        fread(raw.rowSize, 1, y, raw.file);
        if (swapFlag) {
            ConvertLong(raw.rowStart, x);
            ConvertLong((unsigned int*)raw.rowSize, x);
        }
    }

    wid = raw.sizeX;
    hgt = raw.sizeY;
    chan = raw.sizeZ;
    is_uint = false;
    is_float = false;

    Pix = ImageAlloc();
    if ((raw.type & 0xFF00) == 0x0100) {
        if (SP.verbose) std::cerr << "Loading an rle compressed RGB image.\n";
    } else {
        if (SP.verbose) std::cerr << "Loading a raw RGB image.\n";
    }

    unsigned char* ptr = Pix;
    for (int i = raw.sizeY - 1; i >= 0; i--) {
        if (chan == 1) {
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

    delete[] raw.tmp;
    delete[] tmpR;
    delete[] tmpG;
    delete[] tmpB;
    delete[] raw.rowStart;
    delete[] raw.rowSize;
}

//////////////////////////////////////////////////////
// JPEG File Format

void ImageLoadSave::LoadJPEG(const std::string& fname)
{
    unsigned char* image = stbi_load(fname.c_str(), &wid, &hgt, &chan, STBI_default);
    if (!image) throw DMcError("Can't open JPEG file " + std::string(stbi_failure_reason()) + std::string(fname));

    is_uint = false;
    is_float = false;
    Pix = ImageAlloc();
    memcpy(Pix, image, wid * hgt * chan);
    stbi_image_free(image);
}

void ImageLoadSave::SaveJPEG(const std::string& fname) const
{
    if (Pix == NULL || chan < 1 || wid < 1 || hgt < 1) throw DMcError("Image is not defined. Not saving.");
    if (fname.empty()) throw DMcError("SaveJPEG: Filename not specified. Not saving.");
    // Note: A 2 or 4 channel image will have the alpha channel stripped out by the JPEG saver.

    int success = stbi_write_jpg(fname.c_str(), wid, hgt, chan, Pix, 93);
    if (!success) throw DMcError("SaveJPEG() failed: can't write to " + std::string(fname));
}

//////////////////////////////////////////////////////
// TIFF File Format

#ifdef DMC_USE_TIFF

// Compression mode constants
#define NONE 1
#define LEMPELZIV 5

void TiffErrHand(const char* module, const char* fmt, va_list ap)
{
    char Err[1024];
    sprintf(Err, fmt, ap);
    throw DMcError(string(module) + string(Err));
}

void ImageLoadSave::LoadTIFF(const std::string& fname)
{
    TIFF* tif; // Tif file handler

    TIFFSetErrorHandler(TiffErrHand);

    if (SP.verbose) std::cerr << "Attempting to open " << fname << " as TIFF.\n";
    if (SP.verbose) std::cerr << "TIFF version is " << TIFFGetVersion() << std::endl;

    tif = TIFFOpen(fname, "r");
    if (!tif) throw DMcError("Could not open TIFF file '" + string(fname) + "'.");

    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &wid);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &hgt);
    TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &chan);
#ifdef DMC_MACHINE_sgi
    chan = chan >> 16;
#endif
    int bitspersample = -1;
    TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bitspersample);
    bitspersample = bitspersample & 0xffff;

    if (SP.verbose) {
        std::cerr << "size=" << wid << "x" << hgt << std::endl;
        std::cerr << "TIFFTAG_SAMPLESPERPIXEL=" << chan << std::endl;
        int tmp = 0, tmp2 = 0;
        TIFFGetField(tif, TIFFTAG_EXTRASAMPLES, &tmp, &tmp2);
        std::cerr << "TIFFTAG_EXTRASAMPLES " << tmp << ": " << tmp2 << std::endl;
        std::cerr << "TIFFTAG_BITSPERSAMPLE " << bitspersample << std::endl;
        TIFFGetField(tif, TIFFTAG_COMPRESSION, &tmp);
        std::cerr << "TIFFTAG_COMPRESSION " << tmp << std::endl;
        TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &tmp);
        std::cerr << "TIFFTAG_PHOTOMETRIC " << tmp << std::endl;

        TIFFPrintDirectory(tif, stderr, 0);
    }

    is_uint = false;
    is_ushort = false;
    is_float = false;
    if (bitspersample == 32) is_float = true;
    if (bitspersample == 16) is_ushort = true;

    // XXX How do we distinguish a uint from a float image?
    if (bitspersample > 16) throw DMcError("The TIFF library can save float TIFF, but not load them.");

    // Loads the data into a 32-bit word for each pixel, regardless of chan.
    chan = 4;
    Pix = ImageAlloc();
    if (!TIFFReadRGBAImage(tif, wid, hgt, (uint32*)Pix, 0)) { throw DMcError("TIFFReadRGBAImage failed."); }

    if (SP.verbose) {
        int dircount = 0;
        do {
            dircount++;
        } while (TIFFReadDirectory(tif));
        if (dircount > 1) std::cerr << fname << "contains " << dircount << " directories!!!\n";
    }

    TIFFClose(tif);
}

// Handles 1 through 4 channels, uchar, ushort, uint, or float.
void ImageLoadSave::SaveTIFF(const std::string& fname) const
{
    if (Pix == NULL || chan < 1 || wid < 1 || hgt < 1) throw DMcError("Image is not defined. Not saving.");

    TIFFSetErrorHandler(TiffErrHand);

    TIFF* tif = TIFFOpen(fname, "w");
    if (tif == NULL) throw DMcError("TIFFOpen failed: " + string(fname));

    int bitsperchan = 8;
    if (is_float || is_uint)
        bitsperchan = 32;
    else if (is_ushort)
        bitsperchan = 16;
    int bytesperchan = bitsperchan / 8;

    // WARNING: It seems to have a problem with two-channel images. Am I setting things wrong, or what?
    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, wid);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, hgt);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bitsperchan);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, LEMPELZIV);
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
    if (chan == 2 || chan == 4) TIFFSetField(tif, TIFFTAG_EXTRASAMPLES, 1, &extyp);

    // Write one row of the image at a time
    unsigned char* c = Pix;
    for (int l = 0; l < hgt; l++) {
        if (TIFFWriteScanline(tif, c, l, 0) < 0) {
            TIFFClose(tif);
            throw DMcError("TIFFWriteScanline failed: " + string(fname));
        }
        c += wid * chan * bytesperchan;
    }

    // Close the file and return OK
    TIFFClose(tif);
}

#else /* DMC_USE_TIFF */

void ImageLoadSave::LoadTIFF(const std::string& fname) { throw DMcError("TIFF Support not compiled in."); }

void ImageLoadSave::SaveTIFF(const std::string& fname) const { throw DMcError("TIFF Support not compiled in."); }

#endif /* DMC_USE_TIFF */

//////////////////////////////////////////////////////
// PNG File Format

// Read a PNG file.
void ImageLoadSave::LoadPNG(const std::string& fname)
{
    unsigned char* image = stbi_load(fname.c_str(), &wid, &hgt, &chan, STBI_default);
    if (!image) throw DMcError("Can't open PNG file " + std::string(stbi_failure_reason()) + std::string(fname));

    is_uint = false;
    is_float = false;
    Pix = ImageAlloc();
    memcpy(Pix, image, wid * hgt * chan);
    stbi_image_free(image);
}

// Write a PNG file
void ImageLoadSave::SavePNG(const std::string& fname) const
{
    if (Pix == NULL || chan < 1 || wid < 1 || hgt < 1) throw DMcError("Image is not defined. Not saving.");
    if (fname.empty()) throw DMcError("SavePNG: Filename not specified. Not saving.");
    int success = stbi_write_png(fname.c_str(), wid, hgt, chan, Pix, wid * chan);
    if (!success) throw DMcError("SavePNG() failed: " + std::string(fname));
}

//////////////////////////////////////////////////////
// Matlab MAT File Format

#ifdef DMC_USE_MAT

// Read a MAT file.
void ImageLoadSave::LoadMAT(const std::string& fname)
{
    MATFile* pmat;
    mxArray* pa;

    // Open file and verify its contents with matGetArray
    pmat = matOpen(fname, "r");
    if (pmat == NULL) throw DMcError("Error opening fname " + string(fname));

    // Read in array
    pa = matGetNextArray(pmat);
    if (pa == NULL) throw DMcError("Error reading matrix.)";
    if(mxGetNumberOfDimensions(pa) != 2)  throw DMcError("MAT Error: matrix does not have two dimensions.");

    // Copy the data out.
    // Not implemented.
    ASSERT_R(0);

    mxDestroyArray(pa);

    if(matClose(pmat)) throw DMcError("Error closing MAT file.");
}

// Write a MAT file
void ImageLoadSave::SaveMAT(const std::string& fname) const
{
    if (Pix == NULL || chan < 1 || wid < 1 || hgt < 1)  throw DMcError("MAT Image is not defined. Not saving.)";
    if(!fname || !fname[0])  throw DMcError("MAT Filename not specified. Not saving.");

    MATFile *pmat;
    mxArray *pa;
    int status;

    if(SP.verbose) std::cerr << "Saving " << fname << std::endl;
    pmat = matOpen(fname, "w");
    if(pmat == NULL)  throw DMcError("Error creating fname " +string(fname));

    mxClassID TheClass = mxUNKNOWN_CLASS;

    if(is_ushort) {
            TheClass = mxUINT16_CLASS;
    } else {
            ASSERT_R(0);
    }

    int dims[3];
    dims[0] = hgt;
    dims[1] = wid;
    dims[2] = chan;
    pa = mxCreateNumericArray(chan > 1 ? 3:2, dims, TheClass, mxREAL);
    ASSERT_RM(pa, "memory alloc failed");
    mxSetName(pa, "Img");

    if(is_ushort) {
            // Transpose the data. I hate this.
            unsigned short* ImTr = (unsigned short*)mxGetPr(pa);
            unsigned short* usPix = (unsigned short*)Pix;
            int k = 0;
            for (int y = 0; y < hgt; y++) {
                for (int x = 0; x < wid; x++) {
                    for (int c = 0; c < chan; c++) { ImTr[(x * hgt + y) * chan + c] = usPix[k++]; }
                }
            }
    } else {
            ASSERT_R(0);
    }

    status = matPutArray(pmat, pa);
    if(status) throw DMcError("Error using matPutArray.");

    mxDestroyArray(pa);

    if(matClose(pmat) != 0) throw DMcError("Error closing MAT file.");
}

#else /* DMC_USE_MAT */

void ImageLoadSave::LoadMAT(const std::string& fname) { throw DMcError("MAT Support not compiled in."); }

void ImageLoadSave::SaveMAT(const std::string& fname) const { throw DMcError("MAT Support not compiled in."); }

#endif /* DMC_USE_MAT */

//////////////////////////////////////////////////////
// HDR (RGBE) File Format

// Currently this loads and saves f3Images, not rgbeImages.
// I will add this later.
void ImageLoadSave::LoadRGBE(const std::string& fname)
{
    FILE* filep = fopen(fname.c_str(), "rb");
    if (filep == NULL) throw DMcError("LoadRGBE: Unable to load HDR: " + std::string(fname));

    int i, j, row;
    float exposure;

    /*a very basic RADIANCE pic file header */
    char line[1000];
    fgets(line, 1000, filep);
    if (!strcmp(line, "#?RADIANCE")) throw DMcError("LoadRGBE: Not a HDR file: " + std::string(fname));

    fgets(line, 1000, filep);
    fgets(line, 1000, filep);
    fscanf(filep, "EXPOSURE=%f\n", &exposure);
    if (SP.verbose) std::cerr << "Reading HDR file with exposure " << exposure << std::endl;
    // fgets(line, 1000, filep);
    fscanf(filep, "-Y %d +X %d\n", &hgt, &wid);
    if (SP.verbose) std::cerr << wid << "x" << hgt << std::endl;
    is_uint = false;
    is_float = true;
    chan = 3;
    Pix = ImageAlloc();

    RGBE* helpit = new RGBE[wid];

    /*Allocate enough space for one row of COLOR at a time */
    COLOR* oneRow = (COLOR*)malloc(sizeof(COLOR) * wid);

    float invexp = 1.0f / exposure;

    /* Convert RGBE representation to float,float,float representation */
    int k = 0;
    float* P = (float*)Pix;
    for (row = 0; row < hgt; row++) {
        freadscan(oneRow, helpit, wid, filep);
        for (i = 0; i < wid; i++) {
            for (j = 0; j < 3; j++) {
                P[k] = oneRow[i][j] * invexp;
                k++;
            }
        }
    }
    fclose(filep);

    free(oneRow);
    delete[] helpit;
}

void ImageLoadSave::SaveRGBE(const std::string& fname) const
{
    ASSERT_R(is_float && chan == 3);
    ASSERT_R(Pix);

    FILE* filep = fopen(fname.c_str(), "wb");
    if (filep == NULL) throw DMcError("SaveRGBE: Unable to save HDR: " + std::string(fname));

    /*Allocate enough space for one row of COLOR at a time */
    COLOR* oneRow;

    oneRow = (COLOR*)malloc(sizeof(COLOR) * wid);
    float exposure;
    // exposure = 0.52/getApproxMedian();
    exposure = SP.exposure; // 1.0 gives a constant default exposure (and saves time).

    /*a very basic RADIANCE pic file header */
    fprintf(filep, "#?RADIANCE\n");
    fprintf(filep, "# %s\n", SP.comment.c_str());
    fprintf(filep, "FORMAT=32-bit_rle_rgbe\n");
    fprintf(filep, "EXPOSURE=%25.13f\n", exposure);
    fprintf(filep, "\n");
    fprintf(filep, "-Y %d +X %d\n", hgt, wid);

    RGBE* helpit = new RGBE[wid];

    /* Convert separated channel representation to per pixel representation */
    // int k=0;
    float* P = (float*)Pix;
    for (int row = 0; row < hgt; row++) {
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
        fwritescan((COLOR*)&P[row * wid * 3], helpit, wid, filep);
    }
    fclose(filep);

    free(oneRow);
    delete[] helpit;

    if (SP.verbose) std::cerr << "Wrote out HDR file with exposure " << exposure << std::endl;
}

template <class Image_T> const baseImage* ImageLoadSave::ConvertToSaveFormat(const std::string& fname, const Image_T* srcImg)
{
    const baseImage* outImg = static_cast<const baseImage*>(srcImg);

    // Put the image into a format that can be saved in the chosen file format
    // TODO: Put srcImg file format capability info in ImageLoadSave.
    int exts = GetExtensionVal(fname);

    const bool isUC = !srcImg->is_signed() && srcImg->is_integer_virtual() && srcImg->size_element_virtual() == 1;
    const int ch = srcImg->chan_virtual();

    if (exts == GIF_ || exts == JPG_ || exts == JPE_ || exts == BMP_) { // 1 3 uc
        if (ch == 2 || (ch == 1 && !isUC))
            outImg = new tImage<tPixel<unsigned char, 1>>(*srcImg);
        else if (ch == 4 || (ch == 3 && !isUC))
            outImg = new tImage<tPixel<unsigned char, 3>>(*srcImg);
    } else if (exts == TIF_) { // 1 2 3 4 uc, us, ui, f
        // These formats all work fine, except a bug with two-channel.
        // The following bug fix doesn't work because 2->4 channels replicates channel 0.
        if (ch == 2) outImg = new tImage<tPixel<typename Image_T::PixType::ElType, 4>>(*srcImg);
    } else if (exts == TGA_) { // 1 3 4 uc
        if (ch == 2 || (ch == 1 && !isUC))
            outImg = new tImage<tPixel<unsigned char, 1>>(*srcImg);
        else if (ch == 3 && !isUC)
            outImg = new tImage<tPixel<unsigned char, 3>>(*srcImg);
        else if (ch == 4 && !isUC)
            outImg = new tImage<tPixel<unsigned char, 4>>(*srcImg);
    } else if (exts == PNG_) { // 1 2 3 4 uc
        if (!isUC) outImg = new tImage<tPixel<unsigned char, Image_T::PixType::Chan>>(*srcImg);
    } else if (exts == HDR_) { // 3f
        if (ch != 3 || srcImg->is_integer_virtual()) outImg = new tImage<tPixel<float, 3>>(*srcImg);
    } else if (exts == MAT_) { // 1 2 3 4 us
        if (!(srcImg->is_integer_virtual() && srcImg->size_element_virtual() == 2))
            outImg = new tImage<tPixel<unsigned short, Image_T::PixType::Chan>>(*srcImg);
    } else if (exts == PPM_ || exts == PGM_ || exts == PAM_ || exts == PSM_ || exts == PFM_ || exts == PZM_) { // 1s 2s 3s 4s 1f 3f 1uc 3uc 4uc
        if (!srcImg->is_integer_virtual()) {
            if (ch != 1 && ch != 3) outImg = new tImage<tPixel<float, 3>>(*srcImg); // 2f 4f
        } else {
            if ((srcImg->size_element_virtual() != 2 || ch > 4) && (isUC && ch != 1 && ch != 3 && ch != 4))
                outImg = new tImage<tPixel<unsigned char, 3>>(*srcImg); // 2uc ui1 ui2 ui3 ui4
        }
    }

    return outImg;
}
template const baseImage* ImageLoadSave::ConvertToSaveFormat(const std::string& fname, const h1Image* srcImg);
template const baseImage* ImageLoadSave::ConvertToSaveFormat(const std::string& fname, const h2Image* srcImg);
template const baseImage* ImageLoadSave::ConvertToSaveFormat(const std::string& fname, const h3Image* srcImg);
template const baseImage* ImageLoadSave::ConvertToSaveFormat(const std::string& fname, const h4Image* srcImg);
template const baseImage* ImageLoadSave::ConvertToSaveFormat(const std::string& fname, const f1Image* srcImg);
template const baseImage* ImageLoadSave::ConvertToSaveFormat(const std::string& fname, const f2Image* srcImg);
template const baseImage* ImageLoadSave::ConvertToSaveFormat(const std::string& fname, const f3Image* srcImg);
template const baseImage* ImageLoadSave::ConvertToSaveFormat(const std::string& fname, const f4Image* srcImg);
template const baseImage* ImageLoadSave::ConvertToSaveFormat(const std::string& fname, const uc1Image* srcImg);
template const baseImage* ImageLoadSave::ConvertToSaveFormat(const std::string& fname, const uc2Image* srcImg);
template const baseImage* ImageLoadSave::ConvertToSaveFormat(const std::string& fname, const uc3Image* srcImg);
template const baseImage* ImageLoadSave::ConvertToSaveFormat(const std::string& fname, const uc4Image* srcImg);
template const baseImage* ImageLoadSave::ConvertToSaveFormat(const std::string& fname, const us1Image* srcImg);
template const baseImage* ImageLoadSave::ConvertToSaveFormat(const std::string& fname, const us2Image* srcImg);
template const baseImage* ImageLoadSave::ConvertToSaveFormat(const std::string& fname, const us3Image* srcImg);
template const baseImage* ImageLoadSave::ConvertToSaveFormat(const std::string& fname, const us4Image* srcImg);
template const baseImage* ImageLoadSave::ConvertToSaveFormat(const std::string& fname, const ui1Image* srcImg);
template const baseImage* ImageLoadSave::ConvertToSaveFormat(const std::string& fname, const ui2Image* srcImg);
template const baseImage* ImageLoadSave::ConvertToSaveFormat(const std::string& fname, const ui3Image* srcImg);
template const baseImage* ImageLoadSave::ConvertToSaveFormat(const std::string& fname, const ui4Image* srcImg);
