//////////////////////////////////////////////////////////////////////
// Quant.h - Quantize color images to 256 colors for saving.
//
// Copyright David K. McAllister, Mar. 1999.

#ifndef _quant_h
#define _quant_h

#include "toolconfig.h"

#ifdef DMC_MACHINE_sgi
#include <iostream>
using namespace std;
#endif

#ifdef DMC_MACHINE_win
#include <basetsd.h>
#include <iostream>
using namespace std;
#endif

#ifdef DMC_MACHINE_hp
#include <iostream.h>
#endif

typedef unsigned char byte;

//////////////////////////////////////////////////////////////////////
// Color Quantization Class
//////////////////////////////////////////////////////////////////////

struct pixel
{
	byte r, g, b;

	inline bool operator==(const pixel &a) const
	{
		return a.r == r && a.g == g && a.b == b;
	}
};

// Color histogram stuff.
struct color_count
{
	pixel color;
	int value;
};

//////////////////////////////////////////////////////////////////////
// Median Cut Color Quantization Algorithm
// With Color Refinement by David K. McAllister
//////////////////////////////////////////////////////////////////////

// XXX Need to make this a templated class based on the pixel type.
struct Quantizer
{
	int size;                  // How many pixels are in the image
	int NumColors, MaxColors;
	pixel cmap[256];           // The colormap
	pixel *Pix;                // The 24-bit color image

    int WorstErr;
    pixel WorstErrPix;

	byte *Quant(byte *pic24, int sz, int newcolors, bool IsGray = false);

    // Replace the Pix24 image data with the 24-bit colors chosen as the palette
    void ReplaceImage(const byte *pix8);

private:
	// See if there are fewer than MaxCols unique colors.
	// If so, return pic8.
	bool TrivialSolution(int MaxCols, byte *);

	// Reduce the color map by computing the number of unique colors.
	// Also overwrite pic8 with the new indices.
	void ReduceColorMap(byte *pic8);

	// Given a color map, convert the 24-bit image to 8 bits. No dither.
	// Counts is how many of each ColorMap entry there are.
	// Returns error.
	DMCINT64 Image24to8(byte *pic8, int *Counts, int *R=NULL, int *G=NULL, int *B=NULL);
	DMCINT64 RefineColorMap(byte *pic8);

	DMCINT64 Image24to8Fast(color_count *chash, int hsize, int *Counts, int *R=NULL, int *G=NULL, int *B=NULL);
	DMCINT64 RefineColorMapFast(color_count *chash, int hsize);

	// Set the color map to these centroids.
	void CentroidsToColorMap(int *Count, int *R, int *G, int *B);

	void MedianCut(color_count* chv, int colors,
		int sum, int maxval, int newcolors);

	color_count* MakeHist(int &colors);
};

#endif
