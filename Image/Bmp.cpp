//////////////////////////////////////////////////////////////////////
// Bmp.cpp - Read and Write Windows BMP images.
//
// Changes Copyright David K. McAllister, Dec. 1999.
// Taken from XV v. 3.10, which probably stole the code without credit.
// Also, don't worry about the unused variable warnings generated
// during compile.

#include "Image/ImageLoadSave.h"

typedef unsigned char byte;

#define BI_RGB 0
#define BI_RLE8 1
#define BI_RLE4 2

#define WIN_OS2_OLD 12
#define WIN_NEW 40
#define OS2_NEW 64

#define FERROR(fp) (ferror(fp) || feof(fp))

/*******************************************/
static DMC_DECL void bmpError(const char* fname, const char* st) { throw DMcError("BMP error: '" + std::string(fname) + "': " + st); }

/*******************************************/
static DMC_DECL unsigned int getshort(FILE* fp)
{
    int c, c1;
    c = getc(fp);
    c1 = getc(fp);
    return ((unsigned int)c) + (((unsigned int)c1) << 8);
}

/*******************************************/
static DMC_DECL unsigned int getint(FILE* fp)
{
    int c, c1, c2, c3;
    c = getc(fp);
    c1 = getc(fp);
    c2 = getc(fp);
    c3 = getc(fp);
    return ((unsigned int)c) + (((unsigned int)c1) << 8) + (((unsigned int)c2) << 16) + (((unsigned int)c3) << 24);
}

/*******************************************/
static DMC_DECL void putshort(FILE* fp, int i)
{
    int c, c1;

    c = ((unsigned int)i) & 0xff;
    c1 = (((unsigned int)i) >> 8) & 0xff;
    putc(c, fp);
    putc(c1, fp);
}

/*******************************************/
static DMC_DECL void putint(FILE* fp, int i)
{
    int c, c1, c2, c3;
    c = ((unsigned int)i) & 0xff;
    c1 = (((unsigned int)i) >> 8) & 0xff;
    c2 = (((unsigned int)i) >> 16) & 0xff;
    c3 = (((unsigned int)i) >> 24) & 0xff;

    putc(c, fp);
    putc(c1, fp);
    putc(c2, fp);
    putc(c3, fp);
}

/*******************************************/
static int loadBMP1(FILE* fp, byte* Pix, unsigned int w, unsigned int h)
{
    int i, j, c, bitnum, padw;
    byte* pp;

    c = 0;
    padw = ((w + 31) / 32) * 32; /* 'w', padded to be a multiple of 32 */

    for (i = h - 1; i >= 0; i--) {
        pp = Pix + (i * w);

        for (j = bitnum = 0; j < padw; j++, bitnum++) {
            if ((bitnum & 7) == 0) { /* read the next byte */
                c = getc(fp);
                bitnum = 0;
            }

            if (j < (int)w) {
                *pp++ = (c & 0x80) ? 1 : 0;
                c <<= 1;
            }
        }
        if (FERROR(fp)) break;
    }

    return (FERROR(fp));
}

static int loadBMP4(FILE* fp, byte* Pix, unsigned int w, unsigned int h, unsigned int comp)
{
    int i, j, c, c1, x, y, nybnum, padw, rv;
    byte* pp;

    rv = 0;
    c = c1 = 0;

    if (comp == BI_RGB) {         /* read uncompressed data */
        padw = ((w + 7) / 8) * 8; /* 'w' padded to a multiple of 8pix (32 bits) */

        for (i = h - 1; i >= 0; i--) {
            pp = Pix + (i * w);

            for (j = nybnum = 0; j < padw; j++, nybnum++) {
                if ((nybnum & 1) == 0) { /* read next byte */
                    c = getc(fp);
                    nybnum = 0;
                }

                if (j < (int)w) {
                    *pp++ = (c & 0xf0) >> 4;
                    c <<= 4;
                }
            }
            if (FERROR(fp)) break;
        }
    }

    else if (comp == BI_RLE4) { /* read RLE4 compressed data */
        x = y = 0;
        pp = Pix + x + (h - y - 1) * w;

        while (y < (int)h) {
            c = getc(fp);
            if (c == EOF) {
                rv = 1;
                break;
            }

            if (c) { /* encoded mode */
                c1 = getc(fp);
                for (i = 0; i < c; i++, x++, pp++) *pp = (i & 1) ? (c1 & 0x0f) : ((c1 >> 4) & 0x0f);
            }

            else { /* c==0x00 : escape codes */
                c = getc(fp);
                if (c == EOF) {
                    rv = 1;
                    break;
                }

                if (c == 0x00) { /* end of line */
                    x = 0;
                    y++;
                    pp = Pix + x + (h - y - 1) * w;
                }

                else if (c == 0x01)
                    break; /* end of Pix */

                else if (c == 0x02) { /* delta */
                    c = getc(fp);
                    x += c;
                    c = getc(fp);
                    y += c;
                    pp = Pix + x + (h - y - 1) * w;
                }

                else { /* absolute mode */
                    for (i = 0; i < c; i++, x++, pp++) {
                        if ((i & 1) == 0) c1 = getc(fp);
                        *pp = (i & 1) ? (c1 & 0x0f) : ((c1 >> 4) & 0x0f);
                    }

                    if (((c & 3) == 1) || ((c & 3) == 2)) getc(fp); /* read pad byte */
                }
            } /* escape processing */
            if (FERROR(fp)) break;
        } /* while */
    }

    else {
        fprintf(stderr, "unknown BMP compression type 0x%0x\n", comp);
    }

    if (FERROR(fp)) rv = 1;
    return rv;
}

/*******************************************/
static int loadBMP8(FILE* fp, byte* Pix, unsigned int w, unsigned int h, unsigned int comp)
{
    int i, c, c1, padw, x, y, rv;
    byte* pp;

    rv = 0;

    if (comp == BI_RGB) {         /* read uncompressed data */
        padw = ((w + 3) / 4) * 4; /* 'w' padded to a multiple of 4pix (32 bits) */

        for (i = h - 1; i >= 0; i--) {
            pp = Pix + (i * w);

            fread(pp, 1, w, fp);
            for (int j = w; j < padw; j++) getc(fp);

            if (FERROR(fp)) {
                rv = 1;
                break;
            }
        }
    } else if (comp == BI_RLE8) { /* read RLE8 compressed data */
        x = y = 0;
        pp = Pix + x + (h - y - 1) * w;

        while (y < (int)h) {
            c = getc(fp);
            if (c == EOF) {
                rv = 1;
                break;
            }

            if (c) { /* encoded mode */
                c1 = getc(fp);
                for (i = 0; i < c; i++, x++, pp++) *pp = c1;
            }

            else { /* c==0x00 : escape codes */
                c = getc(fp);
                if (c == EOF) {
                    rv = 1;
                    break;
                }

                if (c == 0x00) { /* end of line */
                    x = 0;
                    y++;
                    pp = Pix + x + (h - y - 1) * w;
                }

                else if (c == 0x01)
                    break; /* end of Pix */

                else if (c == 0x02) { /* delta */
                    c = getc(fp);
                    x += c;
                    c = getc(fp);
                    y += c;
                    pp = Pix + x + (h - y - 1) * w;
                }

                else { /* absolute mode */
                    for (i = 0; i < c; i++, x++, pp++) {
                        c1 = getc(fp);
                        *pp = c1;
                    }

                    if (c & 1) getc(fp); /* odd length run: read an extra pad byte */
                }
            } /* escape processing */
            if (FERROR(fp)) break;
        } /* while */
    }

    else {
        fprintf(stderr, "unknown BMP compression type 0x%0x\n", comp);
    }

    if (FERROR(fp)) rv = 1;
    return rv;
}

/*******************************************/
static int loadBMP24(FILE* fp, byte* Pix, unsigned int w, unsigned int h)
{
    int rv = 0;

    int padb = (4 - ((w * 3) % 4)) & 0x03; /* # of pad bytes to read at EOscanline */
    int linebytecnt = padb + 3 * w;
    unsigned char* lbuf = new unsigned char[linebytecnt];
    ASSERT_RM(lbuf, "memory alloc failed");

    for (int i = h - 1; i >= 0; i--) {
        byte* pp = Pix + (i * w * 3);

        fread(lbuf, 1, linebytecnt, fp);
        int w3 = w * 3;
        for (int j = 0; j < w3;) {
            pp[2] = lbuf[j++]; /* blue */
            pp[1] = lbuf[j++]; /* green */
            pp[0] = lbuf[j++]; /* red */
            pp += 3;
        }

        rv = (FERROR(fp));
        if (rv) break;
    }

    delete[] lbuf;

    return rv;
}

/*******************************************/
static int loadBMP32(FILE* fp, byte* Pix, unsigned int w, unsigned int h)
{
    int i, j, padb, rv;
    byte* pp;

    rv = 0;

    padb = 0; /* # of pad bytes to read at EOscanline */

    for (i = h - 1; i >= 0; i--) {
        pp = Pix + (i * w * 4);

        for (j = 0; j < (int)w; j++) {
            pp[2] = getc(fp); /* blue */
            pp[1] = getc(fp); /* green */
            pp[0] = getc(fp); /* red */
            pp[3] = getc(fp); /* alpha */
            pp += 4;
        }

        for (j = 0; j < padb; j++) getc(fp);

        rv = (FERROR(fp));
        if (rv) break;
    }

    return rv;
}

/*******************************************/
// Returns false on success.
void ImageLoadSave::LoadBMP(const char* fname)
{
    int i, c, c1, rv = 0;
    unsigned int bfSize, bfOffBits, biSize, biWidth, biHeight, biPlanes;
    unsigned int biBitCount, biCompression, biSizeImage, biXPelsPerMeter;
    unsigned int biYPelsPerMeter, biClrUsed, biClrImportant;
    int bPad;
    int cmaplen = 0;
    byte red[256], grn[256], blu[256];
    bool Gray = true;

    Pix = NULL;

    FILE* fp = fopen(fname, "rb");
    if (!fp) bmpError(fname, "couldn't open file");

    // Figure out the file size
    // fseek(fp, 0L, 2);
    // long filesize = ftell(fp);
    // fseek(fp, 0L, 0);

    /* read the file type (first two bytes) */
    c = getc(fp);
    c1 = getc(fp);
    if (c != 'B' || c1 != 'M') bmpError(fname, "file type != 'BM'");

    bfSize = getint(fp);
    getshort(fp); /* reserved and ignored */
    getshort(fp);
    bfOffBits = getint(fp);

    biSize = getint(fp);

    if (biSize == WIN_NEW || biSize == OS2_NEW) {
        biWidth = getint(fp);
        biHeight = getint(fp);
        biPlanes = getshort(fp);
        biBitCount = getshort(fp);
        biCompression = getint(fp);
        biSizeImage = getint(fp);
        biXPelsPerMeter = getint(fp);
        biYPelsPerMeter = getint(fp);
        biClrUsed = getint(fp);
        biClrImportant = getint(fp);
    }

    else {                      /* old bitmap format */
        biWidth = getshort(fp); /* Types have changed ! */
        biHeight = getshort(fp);
        biPlanes = getshort(fp);
        biBitCount = getshort(fp);

        /* Not in old versions so have to compute them*/
        biSizeImage = (((biPlanes * biBitCount * biWidth) + 31) / 32) * 4 * biHeight;

        biCompression = BI_RGB;
        biXPelsPerMeter = biYPelsPerMeter = 0;
        biClrUsed = biClrImportant = 0;
    }

#ifdef DMC_DEBUG
    {
        fprintf(stderr, "\nLoadBMP:\tbfSize=%d, bfOffBits=%d\n", bfSize, bfOffBits);
        fprintf(stderr, "\t\tbiSize=%d, biWidth=%d, biHeight=%d, biPlanes=%d\n", biSize, biWidth, biHeight, biPlanes);
        fprintf(stderr, "\t\tbiBitCount=%d, biCompression=%d, biSizeImage=%d\n", biBitCount, biCompression, biSizeImage);
        fprintf(stderr, "\t\tbiX,YPelsPerMeter=%d,%d biClrUsed=%d, biClrImp=%d\n", biXPelsPerMeter, biYPelsPerMeter, biClrUsed, biClrImportant);
    }
#endif

    if (FERROR(fp)) bmpError(fname, "EOF reached in file header");

    /* error checking */
    if ((biBitCount != 1 && biBitCount != 4 && biBitCount != 8 && biBitCount != 24 && biBitCount != 32) || biPlanes != 1 || biCompression > BI_RLE4) {
        throw DMcError(std::string("Bogus BMP File '") + fname + "': bitCount=" + std::to_string(biBitCount) + " Planes=" + std::to_string(biPlanes) +
                       " Compression=" + std::to_string(biCompression));
    }

    if (((biBitCount == 1 || biBitCount == 24) && biCompression != BI_RGB) || (biBitCount == 4 && biCompression == BI_RLE8) ||
        (biBitCount == 8 && biCompression == BI_RLE4)) {
        throw DMcError(std::string("Bogus BMP File '") + fname + "': bitCount=" + std::to_string(biBitCount) + " Planes=" + std::to_string(biPlanes) +
                       " Compression=" + std::to_string(biCompression));
    }

    bPad = 0;
    if (biSize != WIN_OS2_OLD) {
        /* skip ahead to colormap, using biSize */
        c = biSize - 40; /* 40 bytes read from biSize to biClrImportant */
        for (i = 0; i < c; i++) getc(fp);

        bPad = bfOffBits - (biSize + 14);
    }

    /* load up colormap, if any */
    cmaplen = (biClrUsed) ? biClrUsed : 1 << biBitCount;
    if (biBitCount < 24) {
        int i;

        for (i = 0; i < cmaplen; i++) {
            blu[i] = getc(fp);
            grn[i] = getc(fp);
            red[i] = getc(fp);
            if (biSize != WIN_OS2_OLD) {
                getc(fp);
                bPad -= 4;
            }
            Gray = Gray && (red[i] == grn[i] && grn[i] == blu[i]);
        }

        if (FERROR(fp)) bmpError(fname, "EOF reached in BMP colormap");

#ifdef DMC_DEBUG
        {
            fprintf(stderr, "LoadBMP: BMP colormap: (RGB order)\n");
            for (i = 0; i < cmaplen; i++) { fprintf(stderr, "%02x%02x%02x ", red[i], grn[i], blu[i]); }
            fprintf(stderr, "\n\n");
        }
#endif
    }

    if (biSize != WIN_OS2_OLD) {
        /* Waste any unused bytes between the colour map (if present)
            and the start of the actual bitmap data. */

        while (bPad > 0) {
            (void)getc(fp);
            bPad--;
        }
    }

    wid = biWidth;
    hgt = biHeight;
    is_uint = false;
    is_float = false;

    if (biBitCount == 24)
        chan = 3;
    else if (biBitCount == 32)
        chan = 4;
    else
        chan = 1;

    if (biBitCount < 24 && !Gray) {
        // It's color mapped 24-bit.
        chan = 3;
    }

    // From ImageLoadSave.cpp
    Pix = ImageAlloc();

    // Load up the image
    if (biBitCount == 1)
        rv = loadBMP1(fp, Pix, biWidth, biHeight); // Bloats into 1 byte per channel.
    else if (biBitCount == 4)
        rv = loadBMP4(fp, Pix, biWidth, biHeight, biCompression); // Bloats into 1 byte per channel.
    else if (biBitCount == 8)
        rv = loadBMP8(fp, Pix, biWidth, biHeight, biCompression); // Copies into 1 byte per channel.
    else if (biBitCount == 24)
        rv = loadBMP24(fp, Pix, biWidth, biHeight);
    else if (biBitCount == 32)
        rv = loadBMP32(fp, Pix, biWidth, biHeight);
    else
        bmpError(fname, "Weird bit depth.\n");

    if (rv) bmpError(fname, "File appears truncated. Winging it.\n");

    fclose(fp);

    if (biBitCount < 24) {
        // It's color mapped.

        if (Gray) {
            if (cmaplen <= 2) bmpError(fname, "Weird gray BMP image."); // XXX How does a one-bit image work?
            chan = 1;
            // Convert color map image to monochrome.
            // Replace the palette indices with the palette entries.
            for (i = 0; i < size(); i++) { Pix[i] = red[Pix[i]]; }
        } else {
            // Convert color map image to 24 bit.
            // Replace the palette indices with the palette entries.
            // The data is in Pix, but is one-third the right size.
            // Work backward to not overwrite.
            unsigned char* tmp = Pix;
            for (i = size() - 1; i; i--) {
                Pix[i * 3 + 0] = red[tmp[i]];
                Pix[i * 3 + 1] = grn[tmp[i]];
                Pix[i * 3 + 2] = blu[tmp[i]];
            }
        }
    }
}

/*******************************************/
static void writeBMP8(FILE* fp, byte* Pix, int w, int h)
{
    int i, j, padw;

    padw = ((w + 3) / 4) * 4; /* 'w' padded to a multiple of 4pix (32 bits) */

    for (i = h - 1; i >= 0; i--) {
        byte* pp = Pix + (i * w);

        for (j = 0; j < w; j++) putc(*pp++, fp);
        for (; j < padw; j++) putc(0, fp);
    }
}

/*******************************************/
static void writeBMP24(FILE* fp, byte* Pix, int w, int h)
{
    int i, j, padb;

    padb = (4 - ((w * 3) % 4)) & 0x03; /* # of pad bytes to write at EOscanline */

    for (i = h - 1; i >= 0; i--) {
        byte* pp = Pix + (i * w * 3);

        for (j = 0; j < w; j++) {
            putc(pp[2], fp);
            putc(pp[1], fp);
            putc(pp[0], fp);
            pp += 3;
        }

        for (j = 0; j < padb; j++) putc(0, fp);
    }
}

/*******************************************/
void ImageLoadSave::SaveBMP(const char* fname) const
{
    FILE* fp = fopen(fname, "wb");
    if (!fp) return (bmpError(fname, "couldn't write file"));

    int i, nc = 0, nbits = 0, bytesperline;

    if (chan == 1) {
        // Grayscale
        nc = 256;
        nbits = 8;
    } else if (chan == 3) {
        // True color
        nbits = 24;
        nc = 0;
    } else {
        bmpError(fname, "Can only save a 1- or 3-channel BMP for now.");
    }

    bytesperline = ((wid * nbits + 31) / 32) * 4; /* # bytes written per line */

    putc('B', fp);
    putc('M', fp); /* BMP file magic number */

    /* compute filesize and write it */
    i = 14 +                /* size of bitmap file header */
        40 +                /* size of bitmap info header */
        (nc * 4) +          /* size of colormap */
        bytesperline * hgt; /* size of image data */

    putint(fp, i);
    putshort(fp, 0);                /* reserved1 */
    putshort(fp, 0);                /* reserved2 */
    putint(fp, 14 + 40 + (nc * 4)); /* offset from BOfile to BObitmap */

    putint(fp, 40);                 /* biSize: size of bitmap info header */
    putint(fp, wid);                /* biWidth */
    putint(fp, hgt);                /* biHeight */
    putshort(fp, 1);                /* biPlanes: must be '1' */
    putshort(fp, nbits);            /* biBitCount: 1,4,8, or 24 */
    putint(fp, BI_RGB);             /* biCompression: BI_RGB, BI_RLE8 or BI_RLE4 */
    putint(fp, bytesperline * hgt); /* biSizeImage: size of raw image data */
    putint(fp, 96 * 39);            /* biXPelsPerMeter: (96dpi * 39" per meter) */
    putint(fp, 96 * 39);            /* biYPelsPerMeter: (96dpi * 39" per meter) */
    putint(fp, nc);                 /* biClrUsed: # of colors used in cmap */
    putint(fp, nc);                 /* biClrImportant: same as above */

    /* write out the colormap */
    if (chan == 1) {
        for (i = 0; i < nc; i++) {
            putc(i, fp);
            putc(i, fp);
            putc(i, fp);
            putc(0, fp);
        }
    }

    /* write out the image */
    if (nbits == 8)
        writeBMP8(fp, Pix, wid, hgt);
    else if (nbits == 24)
        writeBMP24(fp, Pix, wid, hgt);
    else
        bmpError(fname, "Bad bit depth");

    if (FERROR(fp)) bmpError(fname, "Premature file close.");
}
