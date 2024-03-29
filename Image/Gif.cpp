//////////////////////////////////////////////////////////////////////
// Gif.cpp - Load and save GIF images
//
// Changes Copyright David K. McAllister, Aug. 1997.
// Taken from XV v. 3.10, which was taken from xgif and others 1987-1999.
// As I'm sure you know, the LZW compression contained herein is patented.
// You need to make sure that your use complies with the licensing agreement.
// By the way, I heard the patent expires in 2003.
// Also, don't worry about the unused variable warnings generated
// during compile.

#include "Image/ImageLoadSave.h"
#include "Image/Quant.h"

#include <cstdio>
#include <memory>

typedef unsigned char byte;

#define NEXTBYTE (*dataptr++)
#define EATBYTE (dataptr++)
#define EXTENSION 0x21
#define IMAGESEP 0x2c
#define TRAILER 0x3b
#define INTERLACEMASK 0x40
#define COLORMAPMASK 0x80

static const char* id87 = "GIF87a";
static const char* id89 = "GIF89a";

static int EGApalette[16][3] = {{0, 0, 0},       {0, 0, 128},     {0, 128, 0},     {0, 128, 128},   {128, 0, 0},     {128, 0, 128},
                                {128, 128, 0},   {200, 200, 200}, {100, 100, 100}, {100, 100, 255}, {100, 255, 100}, {100, 255, 255},
                                {255, 100, 100}, {255, 100, 255}, {255, 255, 100}, {255, 255, 255}};

/* Fetch the next code from the raster data stream. The codes can be
 * any length from 3 to 12 bits, packed into 8-bit bytes, so we have to
 * maintain our location in the Raster array as a BIT Offset. We compute
 * the byte Offset into the raster array by dividing this by 8, pick up
 * three bytes, compute the bit Offset into our 24-bit chunk, shift to
 * bring the desired code to the bottom, then mask it off and return it.
 */

/*****************************/

// Info structure filled in by ReadGIF()
class GIFReader {
public:
    GIFReader(const std::string& fname_) : fname(fname_) {}

    unsigned char* pic; // Image data
    int chan;
    unsigned char r[256], g[256], b[256]; // Colormap

    int BitOffset,                // Bit Offset of next code
        XC, YC,                   // Output X and Y coords of current pixel
        Pass,                     // Used by output routine if interlaced pic
        OutCount,                 // Decompressor output 'stack count'
        LeftOfs, TopOfs,          // Image offset
        Width, Height,            // Image dimensions
        BitsPerPixel,             // Bits per pixel, read from GIF header
        ColorMapSize,             // Number of colors
        Background,               // Background color
        CodeSize,                 // Code size, read from GIF header
        InitCodeSize,             // Starting code size, used during Clear
        Code,                     // Value returned by ReadCode
        MaxCode,                  // Limiting value for current code size
        ClearCode,                // GIF clear code
        EOFCode,                  // GIF end-of-information code
        CurCode, OldCode, InCode, // Decompressor variables
        FirstFree,                // First free code, generated per GIF spec
        FreeCode,                 // Decompressor, next free slot in hash table
        FinChar,                  // Decompressor variable
        BitMask,                  // AND mask for data size
        ReadMask,                 // Code AND mask for current code size
        Misc,                     // Miscellaneous bits (interlace, local cmap)
        filesize;                 // Length of the input file.

    const std::string& fname;

    bool Interlace, HasColormap, GrayColormap, WantPaletteInds;

    byte* RawGIF; // The heap array to hold it, raw
    byte* Raster; // The raster data stream, unblocked
    byte* pic8;
    byte* dataptr;

    // TODO: Allocate these on heap to save stack size
    // The hash table used by the decompressor
    int Prefix[4096];
    int Suffix[4096];

    // An output array used by the decompressor
    int OutCode[4097];

    //////////////////////////////
    DMC_DECL void gifWarning(const char* st)
    {
#ifdef DMC_DEBUG
        std::cerr << fname << ": " << st << std::endl;
#endif
    }

    //////////////////////////////
    DMC_DECL int gifError(const char* st)
    {
        if (RawGIF != NULL) delete[] RawGIF;
        if (Raster != NULL) delete[] Raster;

        if (pic) delete[] pic;

        if (pic8 && pic8 != pic) delete[] pic8;

        pic = (byte*)NULL;

        throw DMcError(st);
    }

    //////////////////////////////
    DMC_DECL int readCode()
    {
        int RawCode, ByteOffset;

        ByteOffset = BitOffset / 8;
        RawCode = Raster[ByteOffset] + (Raster[ByteOffset + 1] << 8);
        if (CodeSize >= 8) RawCode += (((int)Raster[ByteOffset + 2]) << 16);
        RawCode >>= (BitOffset % 8);
        BitOffset += CodeSize;

        return (RawCode & ReadMask);
    }

    //////////////////////////////
    DMC_DECL void doInterlace(int Index)
    {
        static byte* ptr = NULL;
        static int oldYC = -1;

        if (oldYC != YC) {
            ptr = pic8 + YC * Width * chan;
            oldYC = YC;
        }

        if (YC < Height) {
            *ptr++ = r[Index];
            if (!GrayColormap) {
                *ptr++ = g[Index];
                *ptr++ = b[Index];
            }
        }

        // Update the X-coordinate, and if it overflows, update the Y-coordinate

        if (++XC == Width) {
            /* deal with the interlace as described in the GIF
             * spec. Put the decoded scan line out to the screen if we haven't gone
             * past the bottom of it
             */

            XC = 0;

            switch (Pass) {
            case 0:
                YC += 8;
                if (YC >= Height) {
                    Pass++;
                    YC = 4;
                }
                break;

            case 1:
                YC += 8;
                if (YC >= Height) {
                    Pass++;
                    YC = 2;
                }
                break;

            case 2:
                YC += 4;
                if (YC >= Height) {
                    Pass++;
                    YC = 1;
                }
                break;

            case 3: YC += 2; break;

            default: break;
            }
        }
    }

    //////////////////////////////
    int readImage()
    {
        byte ch, ch1, *ptr1, *picptr;
        int npixels, maxpixels;

        npixels = maxpixels = 0;

        // Read in values from the image descriptor

        ch = NEXTBYTE;
        LeftOfs = ch + 0x100 * NEXTBYTE;
        ch = NEXTBYTE;
        TopOfs = ch + 0x100 * NEXTBYTE;
        ch = NEXTBYTE;
        Width = ch + 0x100 * NEXTBYTE;
        ch = NEXTBYTE;
        Height = ch + 0x100 * NEXTBYTE;

        Misc = NEXTBYTE;
        Interlace = (Misc & INTERLACEMASK) ? true : false;

        if (Misc & 0x80) {
            GrayColormap = true;
            for (int i = 0; i < 1 << ((Misc & 7) + 1); i++) {
                r[i] = NEXTBYTE;
                g[i] = NEXTBYTE;
                b[i] = NEXTBYTE;
                GrayColormap = GrayColormap && (r[i] == g[i] && r[i] == b[i]);
                std::cerr << i << std::endl;
            }

            if (WantPaletteInds) {
                GrayColormap = true;
                for (int i = 0; i < 256; i++) {
                    // for(i=0; i< 1 << ((Misc&7)+1); i++) {
                    r[i] = i;
                }
            }
        }

        chan = GrayColormap ? 1 : 3;

        if (!HasColormap && !(Misc & 0x80)) {
            // No global or local colormap */
            gifWarning(": No colormap in this GIF file. Assuming EGA colors.\n");
        }

        // Start reading the raster data. First we get the intial code size
        // and compute decompressor constant values, based on this code size.

        CodeSize = NEXTBYTE;

        ClearCode = (1 << CodeSize);
        EOFCode = ClearCode + 1;
        FreeCode = FirstFree = ClearCode + 2;

        // The GIF spec has it that the code size is the code size used to
        // compute the above values is the code size given in the file, but
        // the code size used in compression/decompression is the code size
        // given in the file plus one. (thus the ++).

        CodeSize++;
        InitCodeSize = CodeSize;
        MaxCode = (1 << CodeSize);
        ReadMask = MaxCode - 1;

        // UNBLOCK: Read the raster data. Here we just transpose it from the
        // GIF array to the Raster array, turning it from a series of blocks
        // into one long data stream, which makes life much easier for
        // readCode().

        ptr1 = Raster;
        do {
            ch = ch1 = NEXTBYTE;
            while (ch--) {
                *ptr1 = NEXTBYTE;
                ptr1++;
            }
            if ((dataptr - RawGIF) > filesize) {
                gifWarning(": This GIF file seems to be truncated. Winging it.\n");
                break;
            }
        } while (ch1);

#ifdef DMC_DEBUG
        std::cerr << "ReadGIF() - picture is " << Width << "x" << Height << ", " << BitsPerPixel << " bits, " << (Interlace ? "" : "non-") << "interlaced\n";
#endif

        // Allocate the 'pic' */
        maxpixels = Width * Height;

        picptr = pic8 = new unsigned char[maxpixels * chan];

        // Decompress the file, continuing until you see the GIF EOF
        // code. One obvious enhancement is to add checking for corrupt
        // files here.

        Code = readCode();
        while (Code != EOFCode) {
            // Clear code sets everything back to its initial value, then
            // reads the immediately subsequent code as uncompressed data.

            if (Code == ClearCode) {
                CodeSize = InitCodeSize;
                MaxCode = (1 << CodeSize);
                ReadMask = MaxCode - 1;
                FreeCode = FirstFree;
                Code = readCode();
                CurCode = OldCode = Code;
                FinChar = CurCode & BitMask;
                if (!Interlace) {
                    *picptr++ = r[FinChar];
                    // std::cerr << int(FinChar) << '-';
                    if (!GrayColormap) {
                        *picptr++ = g[FinChar];
                        *picptr++ = b[FinChar];
                    }
                } else
                    doInterlace(FinChar);
                npixels++;
            } else {
                // If not a clear code, must be data: save same as CurCode and InCode

                // If we're at maxcode and didn't get a clear, stop loading
                if (FreeCode >= 4096) {
                    gifWarning("freecode blew up");
                    break;
                }

                CurCode = InCode = Code;

                // If greater or equal to FreeCode, not in the hash table yet; repeat the last character decoded.

                if (CurCode >= FreeCode) {
                    CurCode = OldCode;
                    if (OutCount > 4096) {
                        gifWarning("outcount1 blew up");
                        break;
                    }
                    OutCode[OutCount++] = FinChar;
                }

                // Unless this code is raw data, pursue the chain pointed to by
                // CurCode through the hash table to its end; each code in the
                // chain puts its associated output code on the output queue.

                while (CurCode > BitMask) {
                    if (OutCount > 4096) break; // Corrupt file
                    OutCode[OutCount++] = Suffix[CurCode];
                    CurCode = Prefix[CurCode];
                }

                if (OutCount > 4096) {
                    gifWarning("outcount blew up");
                    break;
                }

                // The last code in the chain is treated as raw data.

                FinChar = CurCode & BitMask;
                OutCode[OutCount++] = FinChar;

                // Now we put the data out to the Output routine. It's been stacked LIFO, so deal with it that way...

                // safety thing: prevent exceeding range of 'pic8'
                if (npixels + OutCount > maxpixels) OutCount = maxpixels - npixels;

                npixels += OutCount;
                if (!Interlace)
                    for (int i = OutCount - 1; i >= 0; i--) {
                        *picptr++ = r[OutCode[i]];
                        // std::cerr << OutCode[i] << '_';
                        if (!GrayColormap) {
                            *picptr++ = g[OutCode[i]];
                            *picptr++ = b[OutCode[i]];
                        }
                    }
                else
                    for (int i = OutCount - 1; i >= 0; i--) doInterlace(OutCode[i]);
                OutCount = 0;

                // Build the hash table on-the-fly. No table is stored in the file.

                Prefix[FreeCode] = OldCode;
                Suffix[FreeCode] = FinChar;
                OldCode = InCode;

                // Point to the next slot in the table. If we exceed the current
                // MaxCode value, increment the code size unless it's already
                // 12. If it is, do nothing: the next code decompressed better
                // be CLEAR

                FreeCode++;
                if (FreeCode >= MaxCode) {
                    if (CodeSize < 12) {
                        CodeSize++;
                        MaxCode *= 2;
                        ReadMask = (1 << CodeSize) - 1;
                    }
                }
            }
            Code = readCode();
            if (npixels >= maxpixels) break;
        }

        if (npixels != maxpixels) {
            gifWarning(": This GIF file seems to be truncated. Winging it.\n");
            if (!Interlace) // clear->EOBuffer */
                memset((char*)pic8 + npixels, 0, (size_t)(maxpixels - npixels));
        }

        pic = pic8;

        return 1;
    }

    //////////////////////////////////////////////////////////////////////
    int ReadGIF()
    {
        // Returns 1 if successful

        byte ch, *origptr;
        int block;
        bool gotimage;

        // Initialize variables
        BitOffset = XC = YC = Pass = OutCount = 0;
        gotimage = false;
        RawGIF = Raster = pic8 = NULL;
        bool gif89 = false;

        pic = NULL;

        FILE* fp = fopen(fname.c_str(), "rb");
        if (!fp) return (gifError("can't open file"));

        // Find the size of the file
        fseek(fp, 0L, 2);
        filesize = (int)ftell(fp);
        fseek(fp, 0L, 0);

        // The +256's are so we can read truncated GIF files without fear of seg violation
        dataptr = RawGIF = new byte[filesize + 256];
        ASSERT_RM(dataptr, "memory alloc failed");
        memset(dataptr, 0, filesize + 256);

        Raster = new byte[filesize + 256];
        ASSERT_RM(Raster, "memory alloc failed");
        memset(Raster, 0, filesize + 256);

        if (fread(dataptr, (size_t)filesize, (size_t)1, fp) != 1) return (gifError("GIF data read failed"));

        fclose(fp);

        origptr = dataptr;

        if (strncmp((char*)dataptr, id87, (size_t)6) == 0)
            gif89 = false;
        else if (strncmp((char*)dataptr, id89, (size_t)6) == 0)
            gif89 = true;
        else
            return (gifError("not a GIF file"));

        dataptr += 6;

        // Get variables from the GIF screen descriptor
        EATBYTE;
        EATBYTE;
        EATBYTE;
        EATBYTE;

        ch = NEXTBYTE;
        HasColormap = (ch & COLORMAPMASK) ? true : false;

        BitsPerPixel = (ch & 7) + 1;
        ColorMapSize = 1 << BitsPerPixel;
        BitMask = ColorMapSize - 1;

        Background = NEXTBYTE; // Background color... not used.

        EATBYTE;

        // Read in global colormap.

        if (HasColormap) {
#ifdef DMC_DEBUG
            std::cerr << "Reading " << ColorMapSize << " element colormap.\n";
#endif

            GrayColormap = true;

            for (int i = 0; i < ColorMapSize; i++) {
                r[i] = NEXTBYTE;
                g[i] = NEXTBYTE;
                b[i] = NEXTBYTE;
                // std::cerr << i << " " << int(r[i]) << " " << int(g[i]) << " " << int(b[i]) << std::endl;
                GrayColormap = GrayColormap && (r[i] == g[i] && r[i] == b[i]);
            }
        } else { // No colormap in GIF file
            // Put std EGA palette (repeated 16 times) into colormap, for lack of anything better to do

            GrayColormap = false;
            for (int i = 0; i < 256; i++) {
                r[i] = EGApalette[i & 15][0];
                g[i] = EGApalette[i & 15][1];
                b[i] = EGApalette[i & 15][2];
            }
        }

        if (WantPaletteInds) {
            GrayColormap = true;
            for (int i = 0; i < ColorMapSize; i++) { r[i] = i; }
        }

        /* possible things at this point are:
         * an application extension block
         * a comment extension block
         * an (optional) graphic control extension block
         * followed by either an image
         * or a plaintext extension
         */

        while (1) {
            block = NEXTBYTE;
            if (block == EXTENSION) { // Parse extension blocks
                int fn, blocksize;
                fn = NEXTBYTE; // Read extension block
#ifdef DMC_DEBUG
                std::cerr << "GIF extension type 0x%02x\n", fn;
#endif
                if (fn == 'R') { // GIF87 aspect extension
                    blocksize = NEXTBYTE;
                    if (blocksize == 2) {
                        EATBYTE;
                        EATBYTE;
                    } else {
                        for (int i = 0; i < blocksize; i++) EATBYTE;
                    }
                    int sbsize;
                    while ((sbsize = NEXTBYTE) > 0) { // Eat any following data subblocks
                        for (int i = 0; i < sbsize; i++) EATBYTE;
                    }
                } else if (fn == 0xFE) { // Comment Extension
                    int j, sbsize, cmtlen;
                    byte* ptr1;
                    char* sp;

                    cmtlen = 0;
                    ptr1 = dataptr; // Remember start of comments

                    do { // Figure out length of comment
                        sbsize = NEXTBYTE;
                        cmtlen += sbsize;
                        for (j = 0; j < sbsize; j++) EATBYTE;
                    } while (sbsize);

                    if (cmtlen > 0) { // Build into one un-blocked comment
                        char* cmt = new char[cmtlen + 1];
                        ASSERT_RM(cmt, "memory alloc failed");

                        sp = cmt;
                        do {
                            sbsize = (*ptr1++);
                            for (j = 0; j < sbsize; j++, sp++, ptr1++) *sp = *ptr1;
                        } while (sbsize);
                        *sp = '\0';
                        std::cerr << "GIF Comment: " << cmt << std::endl;
                    }
                } else if (fn == 0x01) { // PlainText Extension
                    int j, sbsize;
                    int tgLeft, tgTop, tgWidth, tgHeight, cWidth, cHeight, fg, bg;

                    gifWarning("PlainText extension found in GIF file. Ignored.\n");
                    sbsize = NEXTBYTE;
                    tgLeft = NEXTBYTE;
                    tgLeft += (NEXTBYTE) << 8;
                    tgTop = NEXTBYTE;
                    tgTop += (NEXTBYTE) << 8;
                    tgWidth = NEXTBYTE;
                    tgWidth += (NEXTBYTE) << 8;
                    tgHeight = NEXTBYTE;
                    tgHeight += (NEXTBYTE) << 8;
                    cWidth = NEXTBYTE;
                    cHeight = NEXTBYTE;
                    fg = NEXTBYTE;
                    bg = NEXTBYTE;

                    for (int i = 12; i < sbsize; i++) EATBYTE; // Read rest of first subblock
#ifdef DMC_DEBUG
                    std::cerr << "PlainText: tgrid=" << tgLeft << "," << tgTop << " " << tgWidth << "x" << tgHeight << " cell=" << cWidth << "x" << cHeight
                              << " col=" << fg << "," << bg << std::endl;
#endif
                    // Read (and ignore) data sub-blocks
                    do {
                        j = 0;
                        sbsize = NEXTBYTE;
                        while (j < sbsize) {
                            EATBYTE;
                            j++;
                        }
                    } while (sbsize);
#ifdef DMC_DEBUG
                    std::cerr << std::endl << std::endl;
#endif
                } else if (fn == 0xF9) { // Graphic Control Extension
                    int j, sbsize;

                    gifWarning("Graphic Control Extension in GIF file. Ignored.\n");
                    // Read (and ignore) data sub-blocks
                    do {
                        j = 0;
                        sbsize = NEXTBYTE;
                        while (j < sbsize) {
                            EATBYTE;
                            j++;
                        }
                    } while (sbsize);
                } else if (fn == 0xFF) { // Application Extension
                    int j, sbsize;
                    gifWarning("Application extension");
                    // Read (and ignore) data sub-blocks
                    do {
                        j = 0;
                        sbsize = NEXTBYTE;
                        while (j < sbsize) {
                            EATBYTE;
                            j++;
                        }
                    } while (sbsize);
                } else { // Unknown extension
                    int j, sbsize;

                    gifWarning("unknown GIF extension 0x%02x");

                    // Read (and ignore) data sub-blocks
                    do {
                        j = 0;
                        sbsize = NEXTBYTE;
                        while (j < sbsize) {
                            EATBYTE;
                            j++;
                        }
                    } while (sbsize);
                }
            }

            else if (block == IMAGESEP) {
#ifdef DMC_DEBUG
                std::cerr << "imagesep (got=" << gotimage << ") at start: offset=" << long(dataptr - RawGIF) << std::endl;
#endif

                if (gotimage) { // Just skip over remaining images
                    // Skip image header
                    EATBYTE;
                    EATBYTE; // Left position
                    EATBYTE;
                    EATBYTE; // Top position
                    EATBYTE;
                    EATBYTE; // Width
                    EATBYTE;
                    EATBYTE;             // Height
                    int misc = NEXTBYTE; // Misc. bits

                    if (misc & 0x80) { // Image has local colormap. skip it
                        for (int i = 0; i < 1 << ((misc & 7) + 1); i++) {
                            EATBYTE;
                            EATBYTE;
                            EATBYTE;
                        }
                    }

                    EATBYTE; // Minimum code size

                    // Skip image data sub-blocks
                    int ch1;
                    do {
                        int eatch;
                        eatch = ch1 = NEXTBYTE;
                        while (eatch--) EATBYTE;
                        if ((dataptr - RawGIF) > filesize) break; // EOF
                    } while (ch1);
                } else if (readImage())
                    gotimage = true;

#ifdef DMC_DEBUG
                std::cerr << " at end: dataptr=0x" << long(dataptr - RawGIF) << std::endl;
#endif
            }

            else if (block == TRAILER) { // Stop reading blocks
#ifdef DMC_DEBUG
                std::cerr << "trailer ";
#endif
                break;
            }

            else { // Unknown block type
                char str[128];

#ifdef DMC_DEBUG
                std::cerr << "block type " << block;
#endif

                // Don't mention bad block if file was trunc'd, as it's all bogus
                if ((dataptr - origptr) < filesize) {
                    sprintf(str, "Unknown block type (0x%02x) at offset 0x%lx", block, (long unsigned int)((dataptr - origptr) - 1));

                    if (!gotimage)
                        return gifError(str);
                    else
                        gifWarning(str);
                }

                break;
            }

#ifdef DMC_DEBUG
            std::cerr << std::endl;
#endif
        }

        delete[] RawGIF;
        RawGIF = NULL;
        delete[] Raster;
        Raster = NULL;

        if (!gotimage) return (gifError("no image data found in GIF file"));

#ifdef DMC_DEBUG
        std::cerr << std::endl << std::endl;
#endif

        return 1;
    }
};

//////////////////////////////////////////////////////////////////////
void ImageLoadSave::LoadGIF(const std::string& fname)
{
    GIFReader gi(fname);

    gi.WantPaletteInds = SP.wantPaletteInds;
    int ret = gi.ReadGIF();

    if (ret == 0) {
        wid = hgt = chan = 0;
        Pix = NULL;
        throw DMcError("LoadGIF() failed: can't read GIF image file " + std::string(fname));
    }

    wid = gi.Width;
    hgt = gi.Height;
    chan = gi.chan;
    is_uint = false;
    is_float = false;
    Pix = ImageAlloc();
    memcpy(Pix, gi.pic, size_bytes());
    delete[] gi.pic;
}

//////////////////////////////////////////////////////////////////////
// GIF Saving Routines

// Used in output
static unsigned long masks[] = {0x0000, 0x0001, 0x0003, 0x0007, 0x000F, 0x001F, 0x003F, 0x007F, 0x00FF,
                                0x01FF, 0x03FF, 0x07FF, 0x0FFF, 0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF};

class GIFWriter {
    int init_bits;
    int EOFCode;

    // Number of characters so far in this 'packet'
    int a_count;

    // Define the storage for the packet accumulator
    char accum[256];

    // What file we will write to.
    FILE* fp;
    const char* bname;

    //////////////////////////////////////////////////////////////////////
    // Set up the 'byte output' routine
    DMC_DECL void char_init() { a_count = 0; }

    //////////////////////////////////////////////////////////////////////
    // Flush the packet to disk, and reset the accumulator
    DMC_DECL void flush_char()
    {
        if (a_count > 0) {
            fputc(a_count, fp);
            fwrite(accum, (size_t)1, (size_t)a_count, fp);
            a_count = 0;
        }
    }

    //////////////////////////////////////////////////////////////////////
    // Add a character to the end of the current packet, and if it is 254
    // characters, flush the packet to disk.
    DMC_DECL void char_out(int c)
    {
        accum[a_count++] = c;
        if (a_count >= 254) flush_char();
    }

    //////////////////////////////////////////////////////////////////////
    // Crap used by the compress routine

    unsigned long cur_accum;
    int cur_bits;

#define min(a, b) ((a > b) ? b : a)

#define NUM_BITS 12 /* BITS was already defined on some systems */

#define HSIZE 5003 /* 80% occupancy */

    typedef long int count_int;

    int n_bits;     /* number of bits/code */
    int maxbits;    /* user settable max # bits/code */
    int maxcode;    /* maximum code, given n_bits */
    int maxmaxcode; /* NEVER generate this */

#define MAXCODE(n_bits) ((1 << (n_bits)) - 1)

    count_int htab[HSIZE];
    unsigned short codetab[HSIZE];
#define HashTabOf(i) htab[i]
#define CodeTabOf(i) codetab[i]

    int hsize; /* for dynamic table sizing */

    /*
     * To save much memory, we overlay the table used by compress() with those
     * used by decompress(). The tab_prefix table is the same size and type
     * as the codetab. The tab_suffix table needs 2**BITS characters. We
     * get this from the beginning of htab. The output stack uses the rest
     * of htab, and contains characters. There is plenty of room for any
     * possible stack (stack used to be 8000 characters).
     */

#define tab_prefixof(i) CodeTabOf(i)
#define tab_suffixof(i) ((byte*)(htab))[i]
#define de_stack ((byte*)&tab_suffixof(1 << NUM_BITS))

    int free_ent; /* first unused entry */

    // Block compression parameters -- after all codes are used up,
    // and compression rate changes, start over.
    int clear_flg;

    long int in_count;  /* length of input */
    long int out_count; /* # of codes output (for debugging) */

    void cl_hash(count_int hsize_)
    {
        for (int i = 0; i < hsize_; i++) htab[i] = -1;
    }

    /*
     * Compress
     *
     * Algorithm: use open addressing double hashing (no chaining) on the
     * prefix code / next character combination. We do a variant of Knuth's
     * algorithm D (vol. 3, sec. 6.4) along with G. Knott's relatively-prime
     * secondary probe. Here, the modular division first probe gives way
     * to a faster exclusive-or manipulation. Also do block compression with
     * an adaptive reset, whereby the code table is cleared when the compression
     * ratio decreases, but after the table fills. The variable-length output
     * codes are re-sized at this point, and a special CLEAR code is generated
     * for the decompressor. Late addition: construct the table according to
     * file size for noticeable speed improvement on small files. Please direct
     * questions about this implementation to ames!jaw.
     */

    /*****************************************************************
     * Output the given code.
     * Inputs:
     * code: A n_bits-bit integer. If == -1, then EOF. This assumes
     * that n_bits <= (long)wordsize - 1.
     *
     * Outputs:
     * Outputs code to the file.
     *
     * Algorithm:
     * Maintain a BITS character long buffer (so that 8 codes will
     * fit in it exactly). Use the VAX insv instruction to insert each
     * code in turn. When the buffer fills up empty it and start over.
     */
    void output(int code)
    {
        cur_accum &= masks[cur_bits];

        if (cur_bits > 0)
            cur_accum |= ((long)code << cur_bits);
        else
            cur_accum = code;

        cur_bits += n_bits;

        while (cur_bits >= 8) {
            char_out((int)(cur_accum & 0xff));
            cur_accum >>= 8;
            cur_bits -= 8;
        }

        /*
         * If the next entry is going to be too big for the code size,
         * then increase it, if possible.
         */

        if (free_ent > maxcode || clear_flg) {
            if (clear_flg) {
                maxcode = MAXCODE(n_bits = init_bits);
                clear_flg = 0;
            } else {
                n_bits++;
                if (n_bits == maxbits)
                    maxcode = maxmaxcode;
                else
                    maxcode = MAXCODE(n_bits);
            }
        }

        if (code == EOFCode) {
            /* At EOF, write the rest of the buffer */
            while (cur_bits > 0) {
                char_out((int)(cur_accum & 0xff));
                cur_accum >>= 8;
                cur_bits -= 8;
            }

            flush_char();

            fflush(fp);

            ASSERT_RM(!ferror(fp), "unable to write GIF file");
        }
    }

    //////////////////////////////////////////////////////////////////////
    void compress(byte* data, int len)
    {
        long fcode;
        int i = 0;
        int c;
        int ent;
        int disp;
        int hsize_reg;
        int hshift;

        // Set up the necessary values
        maxbits = NUM_BITS;
        maxmaxcode = 1 << NUM_BITS;
        memset(htab, 0, sizeof(htab));
        memset(codetab, 0, sizeof(codetab));
        hsize = HSIZE;
        free_ent = 0;
        cur_accum = 0;
        cur_bits = 0;

        out_count = 0;
        clear_flg = 0;
        in_count = 1;
        maxcode = MAXCODE(n_bits = init_bits);

        int ClearCode = (1 << (init_bits - 1));
        EOFCode = ClearCode + 1;
        free_ent = ClearCode + 2;

        char_init();
        // Translate the byte using lookup table.
        ent = *data++;
        len--;

        hshift = 0;
        for (fcode = (long)hsize; fcode < 65536L; fcode *= 2L) hshift++;
        hshift = 8 - hshift; /* set hash code range bound */

        hsize_reg = hsize;
        cl_hash((count_int)hsize_reg); /* clear hash table */

        output(ClearCode);

        while (len) {
            c = *data++;
            len--;
            in_count++;

            fcode = (long)(((long)c << maxbits) + ent);
            i = (((int)c << hshift) ^ ent); /* xor hashing */

            if (HashTabOf(i) == fcode) {
                ent = CodeTabOf(i);
                continue;
            }

            else if ((long)HashTabOf(i) < 0) /* empty slot */
                goto nomatch;

            disp = hsize_reg - i; /* secondary hash (after G. Knott) */
            if (i == 0) disp = 1;

        probe:
            if ((i -= disp) < 0) i += hsize_reg;

            if (HashTabOf(i) == fcode) {
                ent = CodeTabOf(i);
                continue;
            }

            if ((long)HashTabOf(i) >= 0) goto probe;

        nomatch:
            output(ent);
            out_count++;
            ent = c;

            if (free_ent < maxmaxcode) {
                CodeTabOf(i) = free_ent++; /* code -> hashtable */
                HashTabOf(i) = fcode;
            } else {
                // Clear out the hash table for block compress.
                cl_hash((count_int)hsize);
                free_ent = ClearCode + 2;
                clear_flg = 1;

                output(ClearCode);
            }
        }

        /* Put out the final code */
        output(ent);
        out_count++;
        output(EOFCode);
    }

    //////////////////////////////
    DMC_DECL void putword(int w, FILE* fp_)
    {
        /* writes a 16-bit integer in GIF order (LSB first) */
        fputc(w & 0xff, fp_);
        fputc((w >> 8) & 0xff, fp_);
    }

public:
    //////////////////////////////////////////////////////////////////////
    void WriteGIF(const std::string& fname, int wid, int hgt, byte* Pix, bool isOneChan, LoadSaveParams SP)
    {
        int size = wid * hgt;

        if ((fp = fopen(fname.c_str(), "wb")) == 0) throw DMcError("WriteGIF() failed: can't write GIF image file " + std::string(fname));

        int ColorMapSize, InitCodeSize, BitsPerPixel;

        // Fill in the 8-bit image and the color map somehow
        Quantizer<uc3Pixel, unsigned char> Qnt((uc3Pixel*)Pix, size, isOneChan, SP.QP);

        byte* pic8 = Qnt.GetIndexImage(); // It allocates pic8; I delete it.
        size_t NumColors = Qnt.GetColorMap().size();

        // Compute BitsPerPixel
        for (BitsPerPixel = 1; BitsPerPixel < 8; BitsPerPixel++) {
            if (size_t(1ull << BitsPerPixel) >= NumColors) break;
        }

        ColorMapSize = 1 << BitsPerPixel;

        if (BitsPerPixel <= 1)
            InitCodeSize = 2;
        else
            InitCodeSize = BitsPerPixel;

        fwrite(id89, (size_t)1, (size_t)6, fp); // The GIF magic number (can use id87 if no comment)

        putword(wid, fp); // Screen descriptor
        putword(hgt, fp);

        int code = 0x80;            // Yes, there is a color map
        code |= (8 - 1) << 4;       // OR in the color resolution (hardwired 8)
        code |= (BitsPerPixel - 1); // OR in the # of bits per pixel
        fputc(code, fp);

        int Background = 0;
        fputc(Background, fp); // Background color

        fputc(0, fp); // Future expansion byte

        // Write the colormap
        int i = 0;
        for (i = 0; i < NumColors && i < ColorMapSize; i++) {
            // std::cerr << Qnt.GetColorMap()[i] << '\n';
            fputc(Qnt.GetColorMap()[i].r(), fp);
            fputc(Qnt.GetColorMap()[i].g(), fp);
            fputc(Qnt.GetColorMap()[i].b(), fp);
        }

        for (; i < ColorMapSize; i++) {
            fputc(0, fp);
            fputc(0, fp);
            fputc(0, fp);
        }

        if (!SP.comment.empty()) { // Write comment blocks
            int blen;

            fputc(0x21, fp); // EXTENSION block
            fputc(0xFE, fp); // Comment extension

            const char* sp = SP.comment.c_str();
            while ((blen = int(strlen(sp))) > 0) {
                if (blen > 255) blen = 255;
                fputc(blen, fp);
                for (int j = 0; j < blen; j++, sp++) fputc(*sp, fp);
            }
            fputc(0, fp); // Zero-length data subblock to end extension
        }

        fputc(',', fp); // Image separator

        // Write the Image header
        putword(0, fp); // LeftOfs
        putword(0, fp); // TopOfs
        putword(wid, fp);
        putword(hgt, fp);

        fputc(0x00, fp); // Global colormap, no interlace.

        fputc(InitCodeSize, fp);
        init_bits = InitCodeSize + 1;
        compress(pic8, wid * hgt); // Write the compressed pixel data

        fputc(0, fp);   // Write out a Zero-length packet (EOF)
        fputc(';', fp); // Write GIF file terminator

        delete[] pic8;

        if (ferror(fp)) {
            fclose(fp);
            throw DMcError("File error in WriteGIF()");
        }

        fclose(fp);
    }
};

void ImageLoadSave::SaveGIF(const std::string& fname) const
{
    if (Pix == NULL || chan < 1 || wid < 1 || hgt < 1) { throw DMcError("Image is empty. Not saving."); }

    if (chan != 1 && chan != 3) throw DMcError(std::string("Can't save a ") + std::to_string(chan) + " channel image as a GIF.");

    GIFWriter gw; // TODO: gw uses a lot of stack
    gw.WriteGIF(fname, wid, hgt, (unsigned char*)Pix, chan == 1, SP);
}
