//////////////////////////////////////////////////////////////////////
// Quant.h - Quantize multi-channel images to a color map
//
// Copyright David K. McAllister, 1999-2007.

#pragma once

#include "Image/ColorMap.h"
#include "Image/QuantParams.h"
#include "Image/tPixel.h"

// Color histogram stuff.
template <class Pixel_T> struct CountedPixel {
    Pixel_T color;
    typename Pixel_T::MathType count;

    CountedPixel<Pixel_T>()
    {
        color = Pixel_T(0);
        count = 0;
    }
};

//////////////////////////////////////////////////////////////////////
// Median Cut Color Quantization Algorithm
// With Color Refinement by David K. McAllister
//////////////////////////////////////////////////////////////////////

template <class Pixel_T, class Index_T> class Quantizer {
    enum { Chan = Pixel_T::Chan };                          // The number of channels in a pixel
    typedef typename Pixel_T::ElType ElType;                // The type of an element of the pixel
    typedef typename Pixel_T::FloatMathType MathType;       // The type that this pixel uses for doing math
    typedef typename Pixel_T::FloatMathPixType MathPixType; // The type that this pixel uses for doing math with multiple channels

    const Pixel_T* Pix;     // The TrueColor image
    Index_T* IndexImg;      // The resulting Index image
    ColorMap<Pixel_T> CMap; // The resulting colormap

    QuantParams QP;

    size_t size;         // How many pixels are in the image
    bool isOneChan;      // True if the source image is gray scale
    MathType worstErr;   // The amount of error at the worst pixel
    Pixel_T worstPixInd; // The index of the pixel with the worst error

public:
    Quantizer(const Pixel_T* TrueColorImg, const size_t size_, bool isOneChan_, QuantParams QP_) :
        Pix(TrueColorImg), IndexImg(NULL), QP(QP_), size(size_), isOneChan(isOneChan_)
    {
    }

    ~Quantizer()
    {
        if (IndexImg != NULL) delete[] IndexImg;
    }

    // Public interface:
    Index_T* GetIndexImage();                                  // Allocate and return an IndexImg. Caller must delete IndexImg.
    ColorMap<Pixel_T>& GetColorMap();                          // Return the chosen color map
    void GetQuantizedTrueColorImage(Pixel_T* NewTrueColorImg); // Fill NewTrueColorImg based on CMap's colors and used IndexImg
    float GetHistogramCount();                                 // Does MakeHist and returns the fraction of histogram boxes that had pixels

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
    MathType Image3to1(std::vector<CountedPixel<MathPixType>>& Centroids);

    // Given a histogram (a list of all the quantized colors and their frequency),
    // find the color map entry that is closest to each histogram entry.
    // Output: Centroids is how many pixels chose each entry and the color sums for later computing the centroids.
    // Returns amount of numerical error.
    MathType Image3to1Fast(const std::vector<CountedPixel<Pixel_T>>& CHist, std::vector<CountedPixel<MathPixType>>& Centroids);

    // Given an initial colormap, refine it to reduce error.
    // If Fast is true, this operates on a histogram of the image, otherwise it operates on the actual pixels and ignores CHist.
    MathType RefineColorMap(const bool Fast, const std::vector<CountedPixel<Pixel_T>>& CHist);

    // Set the color map to these centroids.
    void CentroidsToColorMap(const std::vector<CountedPixel<MathPixType>>& Centroids);

    void MedianCut(std::vector<CountedPixel<Pixel_T>>& CHist);

    // Make an associative histogram of pixel colors by hashing the color values to map similar colors to a histogram entry.
    void MakeHist(std::vector<CountedPixel<Pixel_T>>& CHist);
};
