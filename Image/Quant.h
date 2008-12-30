//////////////////////////////////////////////////////////////////////
// Quant.h - Quantize multi-channel images to a color map
//
// Copyright David K. McAllister, 1999-2007.

#ifndef dmc_quant_h
#define dmc_quant_h

#include "Image/tPixel.h"
#include "Image/ColorMap.h"

// Color histogram stuff.
template<class Pixel_T>
struct CountedPixel
{
    Pixel_T color;
    typename Pixel_T::MathType count;

    CountedPixel<Pixel_T>() {color = Pixel_T(0); count = 0;}
};

//////////////////////////////////////////////////////////////////////
// Median Cut Color Quantization Algorithm
// With Color Refinement by David K. McAllister
//////////////////////////////////////////////////////////////////////

template<class Pixel_T, class Index_T>
class Quantizer
{
    enum {Chan = Pixel_T::Chan};                 // The number of channels in a pixel
    typedef typename Pixel_T::ElType ElType;     // The type of an element of the pixel
    typedef typename Pixel_T::FloatMathType MathType; // The type that this pixel uses for doing math
    typedef typename Pixel_T::FloatMathPixType MathPixType; // The type that this pixel uses for doing math with multiple channels

    const Pixel_T *Pix;     // The TrueColor image
    Index_T *IndexImg;    // The resulting Index image
    ColorMap<Pixel_T> CMap; // The resulting colormap
    size_t size;            // How many pixels are in the image
    bool IsGray;            // True if the source image is gray scale

    MathType WorstErr;      // The amount of error at the worst pixel
    Pixel_T WorstErrPix;    // The index of the pixe with the worst pixel

    size_t MaxColors;       // Maximum colors allowed
    bool MakeArtisticPalette; // True to make a palette that has more error, but includes the less-used but visibly important colors
    int MaxIter;            // Max iterations in refinement steps
    MathType StopEarlyOfs;  // The amount of difference between this and last iteration's error that allows the loop to still terminate

public:
    Quantizer(const Pixel_T *TrueColorImg, const size_t sz, const bool Gray = false) : Pix(TrueColorImg), size(sz), IsGray(Gray)
    { IndexImg = NULL; SetParams(); }

    ~Quantizer() { if(IndexImg != NULL) delete [] IndexImg; }

    // Public interface:
    Index_T *GetIndexImage(); // Allocate and return an IndexImg. Caller must delete IndexImg.
    ColorMap<Pixel_T> &GetColorMap(); // Return the chosen color map
    void GetQuantizedTrueColorImage(Pixel_T *NewTrueColorImg); // Fill NewTrueColorImg based on CMap's colors and used IndexImg
    float GetHistogramCount(); // Does MakeHist and returns the fraction of histogram boxes that had pixels

    // If you want your quantization to behave differently than the defaults to this function then call this function before quantizing.
    void SetParams(const size_t MaxCols = 256, const bool MakeArtistic = false, const int MaxIterat = 20, const MathType StopEarly = 0)
    {
        MaxColors = MaxCols;
        MakeArtisticPalette = MakeArtistic;
        MaxIter = MaxIterat;
        StopEarlyOfs = StopEarly;
        CMap.resize(0);
        if(IndexImg != NULL) delete [] IndexImg;
    }

private:
    // Perform the quantization
    void DoQuant();

    // See if there are <= MaxColors unique colors. If so, return true and fill in IndexImg.
    bool TrivialSolution();

    // Reduce the color map by computing the number of unique colors.
    // Also overwrite IndexImg with the new indices.
    void ReduceColorMap();

    // Map the TrueColor image to an Index_T image by assigning each pixel to the index of the closest color map entry. No dither.
    // Output: Centroids is how many pixels chose each entry and the color sums for later computing the centroids.
    // Returns amount of numerical error.
    MathType Image24to8(std::vector<CountedPixel<MathPixType> > &Centroids);
    MathType RefineColorMap();

    // Given a histogram (a list of all the quantized colors and their frequency),
    // find the color map entry that is closest to each histogram entry.
    // Output: Centroids is how many pixels chose each entry and the color sums for later computing the centroids.
    // Returns amount of numerical error.
    MathType Image24to8Fast(const std::vector<CountedPixel<Pixel_T> > &CHist, std::vector<CountedPixel<MathPixType> > &Centroids);
    MathType RefineColorMapFast(const std::vector<CountedPixel<Pixel_T> > &CHist);

    // Set the color map to these centroids.
    void CentroidsToColorMap(const std::vector<CountedPixel<MathPixType> > &Centroids);

    void MedianCut(std::vector<CountedPixel<Pixel_T> > &CHist);

    // Make an associative histogram of pixel colors by hashing the color values to map similar colors to a histogram entry.
    void MakeHist(std::vector<CountedPixel<Pixel_T> > &CHist);

    void DumpCMap(); // Print out the CMap.
};

#endif
