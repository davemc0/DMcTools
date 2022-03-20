//////////////////////////////////////////////////////////////////////
// tLoadSave.cpp - Load and save images of many file formats
//
// Copyright David K. McAllister, Aug. 1997 - 2007.

// How loading works:
// tImage<>.Load(), which creates an ImageLoadSave and calls into it to do the loading.
// ImageLoadSave.Load() creates a tImage of the data type and num channels of the image file.
// Its baseImage is returned by ImageLoadSave.Load().
// tImage.Load() of this image converts the returned image to this one's type and overwrites this image.
//
// For working with a baseImage rather than a known image type, call LoadtImage().
// This creates an ImageLoadSave and calls into it to do the loading.
// ImageLoadSave.Load() creates a tImage of the data type and num channels of the image file.
// Its baseImage is returned by ImageLoadSave.Load().
// LoadtImage() returns the created baseImage.

// How saving works:
// tImage<>.Save() looks at the source image and the chosen file format.
// If the chosen format cannot support the image type, it creates another image of a supported type.
// It calls ImageLoadSave.Save(), which saves the image.
// If a converted image was created, it is deleted.

#include "Image/ImageLoadSave.h"
#include "Image/tImage.h"
#include "Util/Assert.h"

// Load an image file into whatever kind of tImage is most appropriate.
// Returns a pointer to the baseImage. dynamic_cast will tell you what kind it really is.
// Throws a DMcError on failure.
baseImage* LoadtImage(const std::string& fname, LoadSaveParams SP)
{
    ImageLoadSave loader;
    loader.SP = SP;
    loader.Load(fname);
    ASSERT_R(loader.baseImg != NULL);

    baseImage* b = loader.baseImg;
    loader.baseImg = NULL; // Swap it out of the loader so the loader, which created it, doesn't delete it.
    return b;
}

// Load an image and store it in this; converts the loaded image to the type of this if necessary.
template <class Pixel_T> void tImage<Pixel_T>::Load(const std::string& fname, LoadSaveParams SP)
{
    clear(); // Delete the old contents of the target image

    // Load the image and keep it in its disk file format inside loader.baseImg.
    ImageLoadSave loader;
    loader.SP = SP;
    loader.Load(fname);
    ASSERT_R(loader.baseImg != NULL); // Will get an exception before now if the load failed.
    baseImage* base = loader.baseImg;

    if (typeid(*base) == typeid(tImage<Pixel_T>)) {
        // Don't call the operator= if the types match. Instead shallow copy the tImage class, including the Pix pointer.
        SetImage(static_cast<PixType*>(base->pv_virtual()), base->w_virtual(), base->h_virtual(), true);
        base->ownPix_virtual(false); // Prepare to detach Pix from base, rather than delete [] it.
    } else {
        // Convert from the type of the loaded image to type of this.
        if (f1Image* loadedImg = dynamic_cast<f1Image*>(base)) { *this = *loadedImg; }
        // if (f2Image* loadedImg = dynamic_cast<f2Image*>(base)) { *this = *loadedImg; }
        if (f3Image* loadedImg = dynamic_cast<f3Image*>(base)) { *this = *loadedImg; }
        if (f4Image* loadedImg = dynamic_cast<f4Image*>(base)) { *this = *loadedImg; }
        if (uc1Image* loadedImg = dynamic_cast<uc1Image*>(base)) { *this = *loadedImg; }
        if (uc2Image* loadedImg = dynamic_cast<uc2Image*>(base)) { *this = *loadedImg; }
        if (uc3Image* loadedImg = dynamic_cast<uc3Image*>(base)) { *this = *loadedImg; }
        if (uc4Image* loadedImg = dynamic_cast<uc4Image*>(base)) { *this = *loadedImg; }
        // if (us1Image* loadedImg = dynamic_cast<us1Image*>(base)) { *this = *loadedImg; }
        // if (us2Image* loadedImg = dynamic_cast<us2Image*>(base)) { *this = *loadedImg; }
        // if (us3Image* loadedImg = dynamic_cast<us3Image*>(base)) { *this = *loadedImg; }
        // if (us4Image* loadedImg = dynamic_cast<us4Image*>(base)) { *this = *loadedImg; }
        if (ui1Image* loadedImg = dynamic_cast<ui1Image*>(base)) { *this = *loadedImg; }
        // if (ui2Image* loadedImg = dynamic_cast<ui2Image*>(base)) { *this = *loadedImg; }
        // if (ui3Image* loadedImg = dynamic_cast<ui3Image*>(base)) { *this = *loadedImg; }
        // if (ui4Image* loadedImg = dynamic_cast<ui4Image*>(base)) { *this = *loadedImg; }
    }
    // Loader will still point to base and we allow the loader destructor to delete base.
}
// TODO: Comment out unused ones to save compile time and executable size
template void tImage<h1Pixel>::Load(const std::string& fname, LoadSaveParams SP);
template void tImage<h2Pixel>::Load(const std::string& fname, LoadSaveParams SP);
template void tImage<h3Pixel>::Load(const std::string& fname, LoadSaveParams SP);
template void tImage<h4Pixel>::Load(const std::string& fname, LoadSaveParams SP);
template void tImage<f1Pixel>::Load(const std::string& fname, LoadSaveParams SP);
template void tImage<f2Pixel>::Load(const std::string& fname, LoadSaveParams SP);
template void tImage<f3Pixel>::Load(const std::string& fname, LoadSaveParams SP);
template void tImage<f4Pixel>::Load(const std::string& fname, LoadSaveParams SP);
template void tImage<uc1Pixel>::Load(const std::string& fname, LoadSaveParams SP);
template void tImage<uc2Pixel>::Load(const std::string& fname, LoadSaveParams SP);
template void tImage<uc3Pixel>::Load(const std::string& fname, LoadSaveParams SP);
template void tImage<uc4Pixel>::Load(const std::string& fname, LoadSaveParams SP);
template void tImage<us1Pixel>::Load(const std::string& fname, LoadSaveParams SP);
template void tImage<us2Pixel>::Load(const std::string& fname, LoadSaveParams SP);
template void tImage<us3Pixel>::Load(const std::string& fname, LoadSaveParams SP);
template void tImage<us4Pixel>::Load(const std::string& fname, LoadSaveParams SP);
template void tImage<ui1Pixel>::Load(const std::string& fname, LoadSaveParams SP);
template void tImage<ui2Pixel>::Load(const std::string& fname, LoadSaveParams SP);
template void tImage<ui3Pixel>::Load(const std::string& fname, LoadSaveParams SP);
template void tImage<ui4Pixel>::Load(const std::string& fname, LoadSaveParams SP);

template <class Pixel_T> void tImage<Pixel_T>::Save(const std::string& fname, LoadSaveParams SP) const
{
    ImageLoadSave saver;
    const baseImage* outImg = saver.ConvertToSaveFormat(fname.c_str(), this);

    saver.SetImage((unsigned char*)outImg->pv_virtual(), w_virtual(), h_virtual(), outImg->chan_virtual(),
                   (typeid(typename Pixel_T::ElType) == typeid(unsigned int)), (typeid(typename Pixel_T::ElType) == typeid(unsigned short)),
                   (typeid(typename Pixel_T::ElType) == typeid(float)));
    saver.SP = SP;

    try {
        saver.Save(fname);
    }
    catch (...) {
        saver.Pix = NULL;
        saver.wid = saver.hgt = saver.chan = 0;
        if (outImg != this) delete outImg; // If we had to create a converted image for saving, delete it.
        throw;
    }

    saver.Pix = NULL;
    saver.wid = saver.hgt = saver.chan = 0;
    if (outImg != this) delete outImg; // If we had to create a converted image for saving, delete it.
}
// TODO: Comment out unused ones to save compile time and executable size
template void tImage<h1Pixel>::Save(const std::string& fname, LoadSaveParams SP) const;
template void tImage<h2Pixel>::Save(const std::string& fname, LoadSaveParams SP) const;
template void tImage<h3Pixel>::Save(const std::string& fname, LoadSaveParams SP) const;
template void tImage<h4Pixel>::Save(const std::string& fname, LoadSaveParams SP) const;
template void tImage<f1Pixel>::Save(const std::string& fname, LoadSaveParams SP) const;
template void tImage<f2Pixel>::Save(const std::string& fname, LoadSaveParams SP) const;
template void tImage<f3Pixel>::Save(const std::string& fname, LoadSaveParams SP) const;
template void tImage<f4Pixel>::Save(const std::string& fname, LoadSaveParams SP) const;
template void tImage<uc1Pixel>::Save(const std::string& fname, LoadSaveParams SP) const;
template void tImage<uc2Pixel>::Save(const std::string& fname, LoadSaveParams SP) const;
template void tImage<uc3Pixel>::Save(const std::string& fname, LoadSaveParams SP) const;
template void tImage<uc4Pixel>::Save(const std::string& fname, LoadSaveParams SP) const;
template void tImage<us1Pixel>::Save(const std::string& fname, LoadSaveParams SP) const;
template void tImage<us2Pixel>::Save(const std::string& fname, LoadSaveParams SP) const;
template void tImage<us3Pixel>::Save(const std::string& fname, LoadSaveParams SP) const;
template void tImage<us4Pixel>::Save(const std::string& fname, LoadSaveParams SP) const;
template void tImage<ui1Pixel>::Save(const std::string& fname, LoadSaveParams SP) const;
template void tImage<ui2Pixel>::Save(const std::string& fname, LoadSaveParams SP) const;
template void tImage<ui3Pixel>::Save(const std::string& fname, LoadSaveParams SP) const;
template void tImage<ui4Pixel>::Save(const std::string& fname, LoadSaveParams SP) const;
