//////////////////////////////////////////////////////////////////////
// ucImage.cpp - Process ucImages.
// This code is basically deprecated. It shouldn't be used in new development.
// The tImage class is the replacement. It handles any data type.
//
// Copyright David K. McAllister, Aug. 1997.

#include <Image/ucImage.h>
#include <Image/ImageLoadSave.h>
#include <Util/Assert.h>
#include <Math/MiscMath.h>

bool ucImage::LoadGIF(const char *fname, bool w)
{
    ImageLoadSave loader;
    loader.LoadGIF(fname, w);
    
    // Grab the data from the loader.
    SetImage(loader.Pix, loader.wid, loader.hgt, loader.chan);
    loader.Pix = NULL;
    loader.wid = loader.hgt = loader.chan = 0;
    return (Pix == NULL);
}

bool ucImage::LoadTGA(const char *fname, bool R5G6B5)
{
    ImageLoadSave loader;
    loader.LoadTGA(fname, R5G6B5);
    
    // Grab the data from the loader.
    SetImage(loader.Pix, loader.wid, loader.hgt, loader.chan);
    loader.Pix = NULL;
    loader.wid = loader.hgt = loader.chan = 0;
    return (Pix == NULL);
}

bool ucImage::Load(const char *fname, int ch)
{
    ImageLoadSave loader;
    loader.ucLoad(fname, ch);
    
    // Grab the data from the loader.
    SetImage(loader.Pix, loader.wid, loader.hgt, loader.chan);
    loader.Pix = NULL;
    loader.wid = loader.hgt = loader.chan = 0;
    
    return (Pix == NULL);
}

// Returns DMC_OK on success; DMC_ERROR on error.
bool ucImage::Save(const char *fname) const
{
    ImageLoadSave saver;
    saver.SetImage(Pix, wid, hgt, chan);
    bool rslt = saver.Save(fname);
    saver.Pix = NULL; saver.wid = saver.hgt = saver.chan = 0;
    return rslt;
}

bool ucImage::SaveGIF(const char *fname, int MaxColorsWanted) const
{
    ImageLoadSave saver;
    saver.SetImage(Pix, wid, hgt, chan);
    bool rslt = saver.SaveGIF(fname, MaxColorsWanted);
    saver.Pix = NULL; saver.wid = saver.hgt = saver.chan = 0;
    return rslt;
}
bool ucImage::SavePPM(const char *fname) const
{
    ImageLoadSave saver;
    saver.SetImage(Pix, wid, hgt, chan);
    bool rslt = saver.SavePPM(fname);
    saver.Pix = NULL; saver.wid = saver.hgt = saver.chan = 0;
    return rslt;
}
bool ucImage::SaveBMP(const char *fname) const
{
    ImageLoadSave saver;
    saver.SetImage(Pix, wid, hgt, chan);
    bool rslt = saver.SaveBMP(fname);
    saver.Pix = NULL; saver.wid = saver.hgt = saver.chan = 0;
    return rslt;
}
bool ucImage::SaveTGA(const char *fname) const
{
    ImageLoadSave saver;
    saver.SetImage(Pix, wid, hgt, chan);
    bool rslt = saver.SaveTGA(fname);
    saver.Pix = NULL; saver.wid = saver.hgt = saver.chan = 0;
    return rslt;
}
#ifdef DMC_USE_TIFF
bool ucImage::SaveTIFF(const char *fname) const
{
    ImageLoadSave saver;
    saver.SetImage(Pix, wid, hgt, chan);
    bool rslt = saver.SaveTIFF(fname);
    saver.Pix = NULL; saver.wid = saver.hgt = saver.chan = 0;
    return rslt;
}
#endif
#ifdef DMC_USE_JPEG
bool ucImage::SaveJPEG(const char *fname) const
{
    ImageLoadSave saver;
    saver.SetImage(Pix, wid, hgt, chan);
    bool rslt = saver.SaveJPEG(fname);
    saver.Pix = NULL; saver.wid = saver.hgt = saver.chan = 0;
    return rslt;
}
#endif
#ifdef DMC_USE_PNG
bool ucImage::SavePNG(const char *fname) const
{
    ImageLoadSave saver;
    saver.SetImage(Pix, wid, hgt, chan);
    bool rslt = saver.SavePNG(fname);
    saver.Pix = NULL; saver.wid = saver.hgt = saver.chan = 0;
    return rslt;
}
#endif

// Convert the number of image channels in this image.
void ucImage::SetChan(const int ch)
{
    if(ch < 1 || ch > 4) return;
    
    unsigned char *P2 = doSetChan(ch, Pix);
    
    delete [] Pix;
    Pix = P2;
    
    chan = ch;
    size = wid * hgt;
    dsize = wid * hgt * chan;
}

// Return a new pixel buffer based on P, but with ch channels.
unsigned char *ucImage::doSetChan(const int ch, unsigned char *P)
{
    int i, i2;
    
    int dsize1 = wid * hgt * ch;
    
    // Change the image's parameters.
    unsigned char *P2 = new unsigned char[dsize1];
    ASSERT_RM(P2, "memory alloc failed");
    
    if(ch == chan)
    {
        memcpy(P2, P, dsize1);
        return P2;
    }
    
    // Change the number of channels.
    switch(chan)
    {
    case 1:
        {
            switch(ch)
            {
            case 2:
                for(i=i2=0; i<dsize; i++)
                {
                    P2[i2++] = P[i];
                    P2[i2++] = 255;
                }
                break;
            case 3:
                for(i=i2=0; i<dsize; i++)
                {
                    P2[i2++] = P[i];
                    P2[i2++] = P[i];
                    P2[i2++] = P[i];
                }
                break;
            case 4:
                for(i=i2=0; i<dsize; i++)
                {
                    P2[i2++] = P[i];
                    P2[i2++] = P[i];
                    P2[i2++] = P[i];
                    P2[i2++] = 255;
                }
                break;
            }
        }
        break;
    case 2:
        {
            switch(ch)
            {
            case 1:
                for(i=i2=0; i<dsize; i+=2)
                {
                    P2[i2++] = P[i];
                }
                break;
            case 3:
                for(i=i2=0; i<dsize; i+=2)
                {
                    P2[i2++] = P[i];
                    P2[i2++] = P[i];
                    P2[i2++] = P[i];
                }
                break;
            case 4:
                for(i=i2=0; i<dsize; i++)
                {
                    P2[i2++] = P[i];
                    P2[i2++] = P[i];
                    P2[i2++] = P[i];
                    P2[i2++] = P[++i];
                }
                break;
            }
        }
        break;
    case 3:
        {
            switch(ch)
            {
            case 1:
                for(i=i2=0; i<dsize; )
                {
                    P2[i2++] = (unsigned char)((77 * int(P[i]) + 150 * int(P[i+1]) + 29 * int(P[i+2])) >> 8);
                    i+=3;
                }
                break;
            case 2:
                for(i=i2=0; i<dsize; )
                {
                    P2[i2++] = (unsigned char)((77 * int(P[i]) + 150 * int(P[i+1]) + 29 * int(P[i+2])) >> 8);
                    P2[i2++] = 255;
                    i+=3;
                }
                break;
            case 4:
                for(i=i2=0; i<dsize; )
                {
                    P2[i2++] = P[i++];
                    P2[i2++] = P[i++];
                    P2[i2++] = P[i++];
                    P2[i2++] = 255;
                }
                break;
            }
        }
        break;
    case 4:
        {
            switch(ch)
            {
            case 1:
                for(i=i2=0; i<dsize; i++)
                {
                    P2[i2++] = (unsigned char)((77 * int(P[i]) + 150 * int(P[i+1]) + 29 * int(P[i+2])) >> 8);
                    i+=3;
                }
                break;
            case 2:
                for(i=i2=0; i<dsize; )
                {
                    P2[i2++] = (unsigned char)((77 * int(P[i]) + 150 * int(P[i+1]) + 29 * int(P[i+2])) >> 8);
                    i+=3;
                    P2[i2++] = P[i++];
                }
                break;
            case 3:
                for(i=i2=0; i<dsize; i++)
                {
                    P2[i2++] = P[i++];
                    P2[i2++] = P[i++];
                    P2[i2++] = P[i++];
                }
                break;
            }
        }
        break;
 }
 
 return P2;
}

// Convert this image to the given parameters.
void ucImage::Set(const int _w, const int _h, const int _ch,
                const bool init)
{
    int w = _w, h = _h, ch = _ch;
    
    if(w < 0) w = wid;
    if(h < 0) h = hgt;
    if(ch < 0) ch = chan;
    
    int dsize1 = w * h * ch;
    
    if(Pix)
    {
        // Deal with existing picture.
        if(dsize1 <= 0)
        {
            delete [] Pix;
            Pix = NULL;
            wid = hgt = chan = size = dsize = 0;
            return;
        }
        
        // Copies the pixels to a new array of the right num. color channels.
        unsigned char *P2 = doSetChan(ch, Pix);
        delete [] Pix;
        Pix = P2;
        chan = ch;
        dsize = wid * hgt * chan;
        size = wid * hgt;
        
        // Rescale to the new width and height.
        Resize(w, h);
    }
    else
    {
        if(dsize1 > 0)
        {
            wid = w;
            hgt = h;
            chan = ch;
            dsize = dsize1;
            size = wid * hgt;
            Pix = new unsigned char[dsize];
            ASSERT_RM(Pix != NULL, "memory alloc failed");
            if(init)
                fill(0);
        }
        else
        {
            Pix = NULL;
            wid = hgt = chan = size = dsize = 0;
        }
    }
}

// Convert this image to the given parameters.
void ucImage::Set(const ucImage &Img, const int _w, const int _h,
                const int _ch, const bool init)
{
    int w = _w, h = _h, ch = _ch;
    
    if(w < 0) w = Img.wid;
    if(h < 0) h = Img.hgt;
    if(ch < 0) ch = Img.chan;
    
    int dsize1 = w * h * ch;
    
    if(Pix)
        delete [] Pix;
    
    // Deal with existing picture.
    if(dsize1 <= 0)
    {
        wid = hgt = chan = size = dsize = 0;
        Pix = NULL;
        return;
    }
    
    if(Img.Pix)
    {
        // Copies the pixels to a new array of the right num. color channels.
        wid = Img.wid;
        hgt = Img.hgt;
        chan = Img.chan;
        size = wid * hgt;
        dsize = wid * hgt * chan;
        
        Pix = doSetChan(ch, Img.Pix);
        chan = ch;
        dsize = wid * hgt * chan;
        
        // Rescale to the new width and height.
        Resize(w, h);
    }
    else
    {
        if(dsize1 > 0)
        {
            wid = w;
            hgt = h;
            chan = ch;
            dsize = dsize1;
            size = wid * hgt;
            Pix = new unsigned char[dsize];
            ASSERT_RM(Pix, "memory alloc failed");
            if(init)
                fill(0);
        }
    }
}

ucImage ucImage::operator+(const ucImage &Im) const
{
    ASSERT_R(chan == Im.chan && wid == Im.wid && hgt == Im.hgt);
    
    ucImage Out(wid, hgt, chan);
    
    for(int i=0; i<dsize; i++)
    {
        unsigned short sum = Pix[i] + Im.Pix[i];
        Out.Pix[i] = (sum <= 255) ? sum : 255;
    }
    
    return Out;
}

ucImage &ucImage::operator+=(const ucImage &Im)
{
    ASSERT_R(chan == Im.chan && wid == Im.wid && hgt == Im.hgt);
    
    for(int i=0; i<dsize; i++)
    {
        unsigned short sum = Pix[i] + Im.Pix[i];
        Pix[i] = (sum <= ((unsigned char)255)) ? sum : ((unsigned char)255);
    }
    
    return *this;
}

// Copy channel number src of image img to channel dest of this image.
// Things work fine if Im is *this.
void ucImage::SpliceChan(const ucImage &Im, const int src, const int dest)
{
    ASSERT_R(wid == Im.wid && hgt == Im.hgt);
    ASSERT_R(src < Im.chan && dest < chan);
    
    int tind = dest;
    int sind = src;
    for(; tind < dsize ; )
        Pix[tind += chan] = Im.Pix[sind += Im.chan];
}

void ucImage::VFlip()
{
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

// Copy a rectangle of size bwid x bhgt with upper-right corner srcx,srcy in Im
// to upper-right corner dstx,dsty in *this.
// The two images must have the same number of channels.
// It's ok if the two images are the same.
void ucImage::CopyChunk(const ucImage &Im, const int srcx, const int srcy,
						const int dstx, const int dsty, const int bwid, const int bhgt)
{
    ASSERT_R(chan == Im.chan);
	ASSERT_R(srcx>=0 && dstx>=0 && srcy >= 0 && dsty >= 0);
	ASSERT_R(bwid>0 && bhgt>0);
    
	int srcxmax = Min(srcx+bwid, Im.wid);
	int srcymax = Min(srcy+bhgt, Im.hgt);
	
	int dstxmax = Min(dstx+bwid, wid);
	int dstymax = Min(dsty+bhgt, hgt);
	
	if(srcxmax-srcx < dstxmax - dstx)
		srcxmax = srcx + (dstxmax - dstx);
	if(srcymax-srcy < dstymax - dsty)
		srcymax = srcy + (dstymax - dsty);
	if(dstxmax-dstx < srcxmax - srcx)
		dstxmax = dstx + (srcxmax - srcx);
	if(dstymax-dsty < srcymax - srcy)
		dstymax = dsty + (srcymax - srcy);

	for(int sy = srcy, dy = dsty; sy < srcymax; sy++, dy++) {
		memmove(chp(srcx, sy), Im.chp(dstx, dy), (srcxmax - srcx) * chan);
	}
}
