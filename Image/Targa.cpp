//////////////////////////////////////////////////////////////////////
// Targa.cpp - Load and save Targa images.
//
// Modified by David K. McAllister, Aug. 2000.

#include <Image/ImageLoadSave.h>

#include <fstream>
using namespace std;

// Header definition.
typedef struct TGA_Header_
{
	unsigned char ImageIDLength; // length of Identifier String.
	unsigned char CoMapType; // 0 = no map
	unsigned char ImgType; // image type (see below for values)
	unsigned char Index_lo, Index_hi; // index of first color map entry
	unsigned char Length_lo, Length_hi; // number of entries in color map
	unsigned char CoSize; // size of color map entry (15,16,24,32)
	unsigned char X_org_lo, X_org_hi; // x origin of image
	unsigned char Y_org_lo, Y_org_hi; // y origin of image
	unsigned char Width_lo, Width_hi; // width of image
	unsigned char Height_lo, Height_hi; // height of image
	unsigned char PixelSize; // pixel size (8,16,24,32)
	unsigned char Desc; // 4 bits are number of attribute bits per pixel
} TGA_Header;

// Definitions for image types.
#define TGA_NULL 0
#define TGA_MAP 1
#define TGA_RGB 2
#define TGA_MONO 3
#define TGA_RLEMAP 9
#define TGA_RLERGB 10
#define TGA_RLEMONO 11

#define TGA_DESC_ALPHA_MASK ((unsigned char)0xF) // number of alpha channel bits
#define TGA_DESC_ORG_MASK ((unsigned char)0x30) // origin mask
#define TGA_ORG_BOTTOM_LEFT 0x00 // origin mask
#define TGA_ORG_BOTTOM_RIGHT 0x10
#define TGA_ORG_TOP_LEFT 0x20
#define TGA_ORG_TOP_RIGHT 0x30

////////////////////////////////////////////////////////////////
// Load Targa

// Color mapped image with one-byte indices at each pixel.
// Unpacks it into a standard raster image with chan channels.
static int decode_map8(const unsigned char *src0, unsigned char *dest0, const int size,
					   const int chan, const unsigned char *color_map)
{
	const unsigned char *src = src0;
	unsigned char *dest = dest0;
	
	for(int i=0; i<size; i++)
	{
		for(int c=0; c<chan; c++)
			dest[chan - c - 1] = color_map[src[0] * chan + c];
		src++;
		dest += chan;
	}
	
	return(0);
}

// Make a 24-bit image out of the 16-bit RGB image
// Works for X1R5G5B5 images.
static int decode_rgb16(const unsigned char *src0, unsigned char *dest0,
						const int wid, const int hgt, const bool R5G6B5)
{
	const unsigned char *src = src0;
	unsigned char *dest = dest0;
    
    if(R5G6B5) {
        for(int i=0; i<wid*hgt; i++)
        {
            dest[0] = (src[1] << 0) & 0xf8;
            dest[1] = ((src[1] << 5) | (src[0] >> 3)) & 0xfc;
            dest[2] = (src[0] << 3) & 0xf8;
            src += 2;
            dest += 3;
        }
    } else{
        for(int i=0; i<wid*hgt; i++)
        {
            dest[0] = (src[1] << 1) & 0xf8;
            dest[1] = ((src[1] << 6) | (src[0] >> 2)) & 0xf8;
            dest[2] = (src[0] << 3) & 0xf8;
            src += 2;
            dest += 3;
        }
    }
    
    return(0);
}

static int decode_rgb24(const unsigned char *src0, unsigned char *dest0,
						const int wid, const int hgt)
{
	const unsigned char *src = src0;
	unsigned char *dest = dest0;
	
	for(int i=0; i<wid*hgt; i++)
	{
		dest[0] = src[2]; // Red
		dest[1] = src[1]; // Green 
		dest[2] = src[0]; // Blue
		src += 3;
		dest += 3;
	}
	
	return(0);
}

static int decode_rgb32(const unsigned char *src0, unsigned char *dest0,
						const int wid, const int hgt)
{
	const unsigned char *src = src0;
	unsigned char *dest = dest0;
	
	for(int i=0; i<wid*hgt; i++)
	{
		dest[0] = src[2]; // Red
		dest[1] = src[1]; // Green 
		dest[2] = src[0]; // Blue
		dest[3] = src[3]; // Alpha
		src += 4;
		dest += 4;
	}
	
	return(0);
}

// Decode run-length encoded Targa into 24-bit pixels
// Stores 16-bit RGB source data into a 24-bit dest.
static int decode_rgb_rle16(const unsigned char *src0, unsigned char *dest0,
							const int wid, const int hgt, const bool R5G6B5)
{
	int chan = 3;
	const unsigned char *src = src0;
	unsigned char *dest = dest0;

	while(dest < dest0 + wid*hgt*chan) {
		// Pixels encoded in "packets"
		// First byte is raw/rle flag(upper bit) and count(1-128 as 0-127 in lower 7 bits)
		// If raw, the next count chan-byte color values in the file are taken verbatim
		// If rle, the next single chan-byte color value speaks for the next count pixels
		
		int raw = (*src & 0x80) == 0; // Is this packet raw pixels or a repeating color
		int count = (*src & 0x7f) + 1; // How many raw pixels or color repeats
		src++; // Advance src beyond first byte to next color

		if(dest + count*chan > dest0 + wid*hgt*chan) // prevent from writing out of dest range
			count = (dest0 + wid*hgt*chan - dest) / chan;
        
        if(R5G6B5) {
            for(int j=0; j<count; j++)
            {
                dest[0] = (src[1] << 0) & 0xf8;
                dest[1] = ((src[1] << 5) | (src[0] >> 3)) & 0xfc;
                dest[2] = (src[0] << 3) & 0xf8;
                
                if(raw) // In raw mode, keep advancing "src" to subsequent values
                    src += 2; // In RLE mode, just repeat the packet[1] RGB color
                dest += chan;
            }
        } else {
            for(int j=0; j<count; j++)
            {
                dest[0] = (src[1] << 1) & 0xf8;
                dest[1] = ((src[1] << 6) | (src[0] >> 2)) & 0xf8;
                dest[2] = (src[0] << 3) & 0xf8;
                
                if(raw) // In raw mode, keep advancing "src" to subsequent values
                    src += 2; // In RLE mode, just repeat the packet[1] RGB color
                dest += chan;
            }
        }

		if(!raw) // After outputting count RGB values, advance "src" beyond color if rle
			src += 2;
	}

	ASSERT_R(dest <= dest0 + wid*hgt*chan);

	return(0);
}

// Decode run-length encoded Targa into 8-bit pixels
// Stores 8-bit source data into a 8-bit dest.
static int decode_rgb_rle8(const unsigned char *src0, unsigned char *dest0,
							const int wid, const int hgt)
{
	int chan = 1;
	const unsigned char *src = src0;
	unsigned char *dest = dest0;
	
	while(dest < dest0 + wid*hgt*chan) {
		// Pixels encoded in "packets"
		// First byte is raw/rle flag(upper bit) and count(1-128 as 0-127 in lower 7 bits)
		// If raw, the next count chan-byte color values in the file are taken verbatim
		// If rle, the next single chan-byte color value speaks for the next count pixels
		
		int raw = (*src & 0x80) == 0; // Is this packet raw pixels or a repeating color
		int count = (*src & 0x7f) + 1; // How many raw pixels or color repeats
		src++; // Advance src beyond first byte to 8-bit color
		
		if(dest + count*chan > dest0 + wid*hgt*chan) // prevent from writing out of dest range
			count = (dest0 + wid*hgt*chan - dest) / chan;
		
		for(int j=0; j<count; j++)
		{
			dest[0] = src[0]; // Red
			
			if(raw) // In raw mode, keep advancing "src" to subsequent values
				src += chan; // In RLE mode, just repeat the packet[1] RGB color
			dest += chan;
		}
		if(!raw) // After outputting count RGB values, advance "src" beyond color if rle
			src += chan;
	}
	
	ASSERT_R(dest <= dest0 + wid*hgt*chan);

	return(0);
}

// Decode run-length encoded Targa into 24-bit pixels
// Stores 24-bit RGB source data into a 24-bit dest.
static int decode_rgb_rle24(const unsigned char *src0, unsigned char *dest0,
							const int wid, const int hgt)
{
	int chan = 3;
	const unsigned char *src = src0;
	unsigned char *dest = dest0;
	
	while(dest < dest0 + wid*hgt*chan) {
		// Pixels encoded in "packets"
		// First byte is raw/rle flag(upper bit) and count(1-128 as 0-127 in lower 7 bits)
		// If raw, the next count chan-byte color values in the file are taken verbatim
		// If rle, the next single chan-byte color value speaks for the next count pixels
		
		int raw = (*src & 0x80) == 0; // Is this packet raw pixels or a repeating color
		int count = (*src & 0x7f) + 1; // How many raw pixels or color repeats
		src++; // Advance src beyond first byte to 24-bit color
		
		if(dest + count*chan > dest0 + wid*hgt*chan) // prevent from writing out of dest range
			count = (dest0 + wid*hgt*chan - dest) / chan;
		
		for(int j=0; j<count; j++)
		{
			dest[0] = src[2]; // Red
			dest[1] = src[1]; // Green 
			dest[2] = src[0]; // Blue
			
			if(raw) // In raw mode, keep advancing "src" to subsequent values
				src += chan; // In RLE mode, just repeat the packet[1] RGB color
			dest += chan;
		}
		if(!raw) // After outputting count RGB values, advance "src" beyond color if rle
			src += chan;
	}
	
	ASSERT_R(dest <= dest0 + wid*hgt*chan);

	return(0);
}

// Decode run-length encoded Targa into 32-bit pixels
// This used to assume that RLE runs don't cross scanline boundaries, but our
// targa encoder in commonGrLib violates this.
static int decode_rgb_rle32(const unsigned char *src0, unsigned char *dest0,
							const int wid, const int hgt)
{
	int chan = 4;
	const unsigned char *src = src0;
	unsigned char *dest = dest0;
	
	while(dest < dest0 + wid*hgt*chan) {
		// Pixels encoded in "packets"
		// First byte is raw/rle flag(upper bit) and count(1-128 as 0-127 in lower 7 bits)
		// If raw, the next count chan-byte color values in the file are taken verbatim
		// If rle, the next single chan-byte color value speaks for the next count pixels
		
		int raw = (*src & 0x80) == 0; // Is this packet raw pixels or a repeating color
		int count = (*src & 0x7f) + 1; // How many raw pixels or color repeats
		src++; // Advance src beyond first byte to 32-bit color
		
		if(dest + count*chan > dest0 + wid*hgt*chan) // prevent from writing out of dest range
			count = (dest0 + wid*hgt*chan - dest) / chan;
		
		for(int j=0; j<count; j++)
		{
			dest[0] = src[2]; // Red
			dest[1] = src[1]; // Green 
			dest[2] = src[0]; // Blue
			dest[3] = src[3]; // Alpha
			
			if(raw) // In raw mode, keep advancing "src" to subsequent values
				src += chan; // In RLE mode, just repeat the packet[1] RGB color
			dest += chan;
		}
		if(!raw) // After outputting count RGBA values, advance "src" beyond color if rle
			src += chan;
	}
	
	return(0);
}

// Returns false on success.
bool ImageLoadSave::LoadTGA(const char *fname, bool R5G6B5)
{
	// Read whole file "fname" into array.
	ifstream InFile(fname, ios::in | ios::binary);
	if(!InFile.is_open())
	{
		cerr << "Failed to open file `" << fname << "'.\n";
		return true;
	}
	
	InFile.seekg(0, ios::end);
	int fsize = InFile.tellg();
	InFile.seekg(0, ios::beg);
	
	unsigned char *fdata = new unsigned char[fsize];
	ASSERT_RM(fdata, "memory alloc failed");
	
	InFile.read((char *)fdata, fsize);
	
	if(InFile.gcount() != fsize)
	{
		cerr << "Didn't get the right amount of data.\n";
		InFile.close();
		return true;
	}
	
	InFile.close();
	
	// Decode the contents of fdata.
	// Put the resulting pixels in *Pix.
	
	TGA_Header *header = (TGA_Header *)fdata; // Header starts at first byte of file
	unsigned char *color_map = fdata + sizeof(TGA_Header) + header->ImageIDLength;
	int cmapsize = header->CoMapType ? (header->CoSize / 8) *
		((header->Length_hi << 8) | header->Length_lo) : 0;	
	unsigned char *encoded_pixels = color_map + cmapsize;

	char itype_names[16][16] = {"NULL", "MAP", "RGB", "MONO", "4", "5", "6", "7", "8",
		"RLE-MAP", "RLE-RGB", "RLE-MONO", "12", "13", "14", "15"};
	// cerr << "Targa type " << itype_names[0xf & header->ImgType]
	//	<< " bpp = " << int(header->PixelSize) << endl;
	
	if((header->Desc & TGA_DESC_ORG_MASK) != TGA_ORG_TOP_LEFT &&
		(header->Desc & TGA_DESC_ORG_MASK) != TGA_ORG_BOTTOM_LEFT)
	{
		cerr << "Not top/bottom left origin: image desc " << header->Desc << endl;
		delete [] fdata;
		return true;
	}
	
	wid = ((header->Width_hi) << 8) | header->Width_lo;
	hgt = ((header->Height_hi) << 8) | header->Height_lo;
	chan = header->PixelSize / 8; // 3 or 4
	if(chan == 2) chan = 3; // 16-bit means R5G6B5.
	if(header->ImgType == TGA_MAP)
		chan = header->CoSize / 8;
    
    is_uint = false;
    is_float = false;
    Pix = ImageAlloc();
	
	switch(header->ImgType)
	{
	case TGA_MAP:
		if(header->PixelSize == 8)
			decode_map8(encoded_pixels, Pix, size(), chan, color_map);
		else
		{
			cerr << "Bad color mapped index size: " << header->PixelSize << " bits/pixel\n";
			delete [] fdata;
			return true;
		}
		break;
	case TGA_MONO:
		if(header->PixelSize == 8)
			memcpy(Pix, encoded_pixels, size_bytes());
		else
		{
			cerr << "Bad pixel size: " << header->PixelSize << " bits/pixel\n";
			delete [] fdata;
			return true;
		}
		break;
	case TGA_RGB:
		switch(header->PixelSize)
		{
		case 16:
			decode_rgb16(encoded_pixels, Pix, wid, hgt, R5G6B5);
			break;
		case 24:
			decode_rgb24(encoded_pixels, Pix, wid, hgt);
			break;
		case 32:
			decode_rgb32(encoded_pixels, Pix, wid, hgt);
			break;
		default:
			cerr << "Bad pixel size: " << header->PixelSize << " bits/pixel\n";
			delete [] fdata;
			return true;
        }
        break;
    case TGA_RLEMONO:
        if(header->PixelSize == 8) {
            decode_rgb_rle8(encoded_pixels, Pix, wid, hgt);
        } else {
            cerr << "Bad pixel size: " << header->PixelSize << " bits/pixel\n";
            delete [] fdata;
            return true;
        }
        break;
    case TGA_RLERGB:
		switch(header->PixelSize)
		{
		case 16:
			decode_rgb_rle16(encoded_pixels, Pix, wid, hgt, R5G6B5);
			break;
		case 24:
			decode_rgb_rle24(encoded_pixels, Pix, wid, hgt);
			break;
		case 32:
			decode_rgb_rle32(encoded_pixels, Pix, wid, hgt);
			break;
		default:
			cerr << "Bad pixel size: " << header->PixelSize << " bits/pixel\n";
			delete [] fdata;
			return true;
		}
		break;
	default:
		 cerr << "Targa type " << itype_names[0xf & header->ImgType] <<
			" bpp = " << int(header->PixelSize) << endl;
		delete [] fdata;
		return true;
	}
    
    // Flip it vertically so that origin is always top left.
    if((header->Desc & TGA_DESC_ORG_MASK) == TGA_ORG_BOTTOM_LEFT) {
        int lsize = wid * chan;
        unsigned char *tbuf = new unsigned char[lsize];
        ASSERT_RM(tbuf, "memory alloc failed");
        
        for(int y=0; y<hgt/2; y++)
        {
            memcpy(tbuf, &Pix[y*lsize], lsize);
            memcpy(&Pix[y*lsize], &Pix[(hgt-y-1)*lsize], lsize);
            memcpy(&Pix[(hgt-y-1)*lsize], tbuf, lsize);
        }
        delete [] tbuf;
    }
	
	delete [] fdata;
	
	return false;
}

////////////////////////////////////////////////////////////////
// Save Targa

inline bool colors_equal(const unsigned char *a, const unsigned char *b, const int chan)
{
	bool same = true;
	for(int i=0; i<chan; i++)
		same = same && (*a++ == *b++);

	return same;
}

// Look-ahead for run-length encoding....
// Scan through *src characters
// If src[0] and src[1] are the same, return 0 and set count:
// how many match src[0] (including src[0] itself)
// If src[0] and src[1] differ, return 1 and set count: how many total
// non-consecutive values are there(not counting value which repeats)
// Thus: AAAAAAAB returns 0 with count = 7
// ABCDEFGG returns 1 with count = 6
// Never scan more than 128 ahead.
inline bool match(const unsigned char *src, int &count, const int chan)
{
	const unsigned char *prev_color;
	count = 0;
	
	if(colors_equal(src, src+chan, chan))
	{
		// RLE
		prev_color = src;
		while(colors_equal(src, prev_color, chan) && count < 128)
		{
			src += chan;
			count++;
		}
		return false;
	}
	else
	{
		// Raw
		prev_color = src;
		src += chan;
		while(!colors_equal(src, prev_color, chan) && count < 128)
		{
			count++;
			prev_color = src;
			src += chan;
		}

		return true;
	}
}

static int encode_rle(const unsigned char *src0, unsigned char *dest,
					  const int size, const int chan)
{
	const unsigned char *src = src0;
	int dp = 0;
	
	do
	{
		int count;
		bool raw = match(src, count, chan);
		if(count > 128) count = 128;
		
		if(raw)
		{
			dest[dp++] = count - 1;
			if(chan == 4) {
				for(int i=0; i<count; i++)
				{
					dest[dp++] = src[2]; // Blue
					dest[dp++] = src[1]; // Green
					dest[dp++] = src[0]; // Red
					dest[dp++] = src[3]; // Alpha
					src += chan;
				}
			} else if(chan == 3) {
				for(int i=0; i<count; i++)
				{
					dest[dp++] = src[2]; // Blue
					dest[dp++] = src[1]; // Green
					dest[dp++] = src[0]; // Red
					src += chan;
				}
			} else if(chan == 1) {
				for(int i=0; i<count; i++)
				{
					dest[dp++] = src[0]; // Intensity
					src += chan;
				}
			}
		}
		else
		{
			dest[dp++] = (count - 1) | 0x80; // The RLE flag.
			if(chan == 1)
			{
				dest[dp++] = src[0]; // Intensity
			}
			else 
			{
				dest[dp++] = src[2]; // Blue
				dest[dp++] = src[1]; // Green
				dest[dp++] = src[0]; // Red
				if(chan == 4)
					dest[dp++] = src[3]; // Alpha
			}
			src += chan * count;
		}
	}
	while(src <= src0 + size * chan);
	
	return dp;
}

bool ImageLoadSave::SaveTGA(const char *fname) const
{
	if(wid > 65535 || wid <= 0 || hgt > 65535 || hgt <= 0)
	{
		cerr << "Write_targa_file " << wid << "x" << hgt << " too big.\n";
		return true;
	}

	FILE *ft = fopen(fname, "wb");
	if(ft == NULL)
	{
		cerr << "Failed to open file `" << fname << "' for writing.\n";
		return true;
	}
	
	TGA_Header header;	
	header.ImageIDLength = 0;
	header.CoMapType = 0; // no colormap
	header.Index_lo = header.Index_hi = 0; // no colormap
	header.Length_lo = header.Length_hi = header.CoSize = 0; // no colormap
	header.X_org_lo = header.X_org_hi = header.Y_org_lo = header.Y_org_hi = 0; // 0,0 origin
	header.Width_lo = wid;
	header.Width_hi = wid >> 8;
	header.Height_lo = hgt;
	header.Height_hi = hgt >> 8;
	header.Desc = TGA_ORG_TOP_LEFT;

	switch(chan)
	{
	case 1:
		header.ImgType = TGA_RLEMONO;
		header.PixelSize = 8;
		break;
	case 3:
		header.ImgType = TGA_RLERGB;
		header.PixelSize = 24;
		break;
	case 4:
		header.ImgType = TGA_RLERGB;
		header.PixelSize = 32;
		header.Desc |= 8; // This many alpha bits.
		break;
	default:
		cerr << "Cannot save file of " << chan << " channels.\n";
		return true;
	}
	
    // Write the header
	fwrite(&header, sizeof(header), 1, ft);
	
    // Make the array for the outgoing data. If not compressible,
    // it could end up very big. Worst case: AAB.
	unsigned char *out_data = new unsigned char[2*size_bytes() + size() / 128];
	ASSERT_RM(out_data, "memory alloc failed");

    // Compress the data
	int rle_size = encode_rle(Pix, out_data, size(), chan);
	
    // Write the data
	fwrite(out_data, rle_size, 1, ft);

	fclose(ft);

    delete [] out_data;

	return false;
}
