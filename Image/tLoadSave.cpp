//////////////////////////////////////////////////////////////////////
// tLoadSave.cpp - Load and save images of many file formats
//
// Copyright David K. McAllister, Aug. 1997 - 2007.

// How loading works:
// tImage<>.Load() calls tLoad<>().
// tLoad<>() creates an ImageLoadSave and calls into it to do the loading.
// ImageLoadSave.Load() creates a tImage of the data type and num channels of the image file.
// Its baseImage is returned by ImageLoadSave.Load().
// tImage.Load() of the dest image converts the returned image to this one's type.
//
// For working with a baseImage rather than a known image type, call LoadtImage().
// This creates an ImageLoadSave and calls into it to do the loading.
// ImageLoadSave.Load() creates a tImage of the data type and num channels of the image file.
// Its baseImage is returned by ImageLoadSave.Load().
// LoadtImage() returns the created baseImage.

// How saving works:
// tImage<>.Save() calls tSave<>().
// tSave<>() looks at the source image and the chosen file format. 
// If the chosen format cannot support the image type, it creates another image of a supported type.
// It calls ImageLoadSave.Save(), which saves the image.
// If a converted image was created, it is deleted.

#include "Image/ImageLoadSave.h"
//#include "Image/RGBEio.h"
#include "Util/Utils.h"

// Load an image file into whatever kind of tImage is most appropriate.
// Returns a pointer to the baseImage. dynamic_cast will tell you what kind it really is.
// Throws a DMcError on failure.
baseImage *LoadtImage(const char *fname)
{
    ImageLoadSave loader;
    loader.Load(fname);
    ASSERT_R(loader.baseImg != NULL);

    baseImage *b = loader.baseImg;
    loader.baseImg = NULL; // swap it out of the loader so the loader, which created it, doesn't delete it.
    return b;
}

// Load an image and store it in the tImage outImg.
// Converts the loaded image to the type of outImg if necessary.
// Delete the old contents of outImg.
template <class Image_T> 
void tLoad(const char *fname, Image_T *outImg)
{
    // Delete the old contents of the target image.
    outImg->SetSize();

    // Load the image and keep it in its disk file format inside loader.baseImg.
    ImageLoadSave loader;
    loader.Load(fname);
    ASSERT_R(loader.baseImg != NULL); // Will get an exception before now if the load failed.
    baseImage *base = loader.baseImg;

    if(typeid(*base) == typeid(Image_T)) {
        // Don't call the operator= if the types match. Instead member-wise copy the tImage class, i.e. copy the Pix pointer.
        // This means that sometimes we just need to detach the old Pix, rather than delete [] it.
        outImg->SetImage(static_cast<typename Image_T::PixType *>(base->pv_virtual()), base->w_virtual(), base->h_virtual());
        base->ownPix_virtual(false);
    } else {
        // Convert from the type of the loaded image to type of outImg.
        // I must do it this way instead of calling a virtual function of the base class to assign it
        // because member function templates cannot be virtual.
        if(uc1Image *loadedImg = dynamic_cast<uc1Image *>(base)) { *outImg = *loadedImg; }
        if(uc2Image *loadedImg = dynamic_cast<uc2Image *>(base)) { *outImg = *loadedImg; }
        if(uc3Image *loadedImg = dynamic_cast<uc3Image *>(base)) { *outImg = *loadedImg; }
        if(uc4Image *loadedImg = dynamic_cast<uc4Image *>(base)) { *outImg = *loadedImg; }
        //if(us1Image *loadedImg = dynamic_cast<us1Image *>(base)) { *outImg = *loadedImg; }
        //if(us2Image *loadedImg = dynamic_cast<us2Image *>(base)) { *outImg = *loadedImg; }
        //if(us3Image *loadedImg = dynamic_cast<us3Image *>(base)) { *outImg = *loadedImg; }
        //if(us4Image *loadedImg = dynamic_cast<us4Image *>(base)) { *outImg = *loadedImg; }
        if(ui1Image *loadedImg = dynamic_cast<ui1Image *>(base)) { *outImg = *loadedImg; }
        //if(ui2Image *loadedImg = dynamic_cast<ui2Image *>(base)) { *outImg = *loadedImg; }
        //if(ui3Image *loadedImg = dynamic_cast<ui3Image *>(base)) { *outImg = *loadedImg; }
        //if(ui4Image *loadedImg = dynamic_cast<ui4Image *>(base)) { *outImg = *loadedImg; }
        if(f1Image  *loadedImg = dynamic_cast<f1Image  *>(base)) { *outImg = *loadedImg; }
        //if(f2Image  *loadedImg = dynamic_cast<f2Image  *>(base)) { *outImg = *loadedImg; }
        if(f3Image  *loadedImg = dynamic_cast<f3Image  *>(base)) { *outImg = *loadedImg; }
        //if(f4Image  *loadedImg = dynamic_cast<f4Image  *>(base)) { *outImg = *loadedImg; }
    }

    // loader will still point to base and we allow the loader destructor to delete base.
}

// Instantiations. Need a case for every kind of image that will
// have the capability to load an image (all of them).
template void tLoad(const char *fname, uc1Image *outImg);
template void tLoad(const char *fname, uc2Image *outImg);
template void tLoad(const char *fname, uc3Image *outImg);
template void tLoad(const char *fname, uc4Image *outImg);
template void tLoad(const char *fname, us1Image *outImg);
template void tLoad(const char *fname, us2Image *outImg);
template void tLoad(const char *fname, us3Image *outImg);
template void tLoad(const char *fname, us4Image *outImg);
template void tLoad(const char *fname, ui1Image *outImg);
//template void tLoad(const char *fname, ui2Image *outImg);
//template void tLoad(const char *fname, ui3Image *outImg);
//template void tLoad(const char *fname, ui4Image *outImg);
template void tLoad(const char *fname, f1Image *outImg);
//template void tLoad(const char *fname, f2Image *outImg);
template void tLoad(const char *fname, f3Image *outImg);
//template void tLoad(const char *fname, f4Image *outImg);
//template void tLoad(const char *fname, h1Image *outImg);
//template void tLoad(const char *fname, h2Image *outImg);
//template void tLoad(const char *fname, h3Image *outImg);
//template void tLoad(const char *fname, h4Image *outImg);

// Save a tImage. Need to know all the details about it.
template <class Image_T>
void tSave(const char *fname, const Image_T &Img, const saveParams SP)
{
    const baseImage *OutImg = static_cast<const baseImage *>(&Img);

	// XXX We should put this file format capability info in ImageLoadSave.
    // Put the image into a format that can be saved in the chosen file format
    int exts = GetExtensionVal(fname);
    const bool isUC = !Img.is_signed() && Img.is_integer() && Img.size_element()==1;
    const int ch = Img.chan();
    if(exts==GIF_ || exts==JPG_ || exts==BMP_) { // 1 3 uc
        if(ch==2 || (ch==1 && !isUC)) OutImg = new tImage<tPixel<unsigned char, 1> >(Img);
        else if(ch==4 || (ch==3 && !isUC)) OutImg = new tImage<tPixel<unsigned char, 3> >(Img);
    } else if(exts==TIF_) { // 1 2 3 4 uc, us, ui, f
        // These formats all work fine, except a bug with two-channel.
        // The following bug fix doesn't work because 2->4 channels replicates channel 0.
        //if(ch==2) OutImg = new tImage<tPixel<unsigned char, 4> >(Img);
    } else if(exts==TGA_) { // 1 3 4 uc
        if(ch==2 || (ch==1 && !isUC)) OutImg = new tImage<tPixel<unsigned char, 1> >(Img);
        else if((ch==4 || ch==3) && !isUC) OutImg = new tImage<tPixel<unsigned char, Image_T::PixType::Chan> >(Img);
    } else if(exts==PNG_) { // 1 2 3 4 uc
        if(!isUC) OutImg = new tImage<tPixel<unsigned char, Image_T::PixType::Chan> >(Img);
    } else if(exts==HDR_) { // 3f
        if(ch!=3 || Img.is_integer()) OutImg = new tImage<tPixel<float, 3> >(Img);
    } else if(exts==MAT_) { // 1 2 3 4 us
        if(!(Img.is_integer() && Img.size_element()==2)) OutImg = new tImage<tPixel<unsigned short, Image_T::PixType::Chan> >(Img);
    } else if(exts==PPM_ || exts==PFM_ || exts==PAM_ || exts==PSM_ || exts==PFM_ || exts==PZM_) { // 1s 2s 3s 4s 1f 3f 1uc 3uc 4uc
        if(!Img.is_integer()) {
            if(ch!=1 && ch!=3) OutImg = new tImage<tPixel<float, 3> >(Img); // f2 f4
        } else {
            if((Img.size_element() != 2 || ch > 4) && (isUC && ch != 1 && ch != 3 && ch != 4))
                OutImg = new tImage<tPixel<unsigned char, 3> >(Img); // uc2 ui1 ui2 ui3 ui4
        }
    } 

    ImageLoadSave saver;
    saver.SetImage((unsigned char *)OutImg->pv_virtual(), Img.w_virtual(), Img.h_virtual(),
        OutImg->chan_virtual(), (typeid(typename Image_T::PixType::ElType)==typeid(unsigned int)),
        (typeid(typename Image_T::PixType::ElType)==typeid(unsigned short)),
        (typeid(typename Image_T::PixType::ElType)==typeid(float)));
	saver.SP = SP;
	saver.Save(fname);
    saver.Pix = NULL; saver.wid = saver.hgt = saver.chan = 0;

    // If we had to create a converted image for saving, delete it.
    if(&Img != OutImg)
        delete OutImg;

    // If an error occurs while saving, a heap-allocated OutImg will not be deleted.
}

// Instantiations. Need a case for every kind of image that will
// have the capability to save an image (all of them).
  template void tSave(const char *fname, const uc1Image &outImg, const saveParams SP);
  template void tSave(const char *fname, const uc2Image &outImg, const saveParams SP);
  template void tSave(const char *fname, const uc3Image &outImg, const saveParams SP);
  template void tSave(const char *fname, const uc4Image &outImg, const saveParams SP);
  template void tSave(const char *fname, const us1Image &outImg, const saveParams SP);
  template void tSave(const char *fname, const us2Image &outImg, const saveParams SP);
  template void tSave(const char *fname, const us3Image &outImg, const saveParams SP);
  template void tSave(const char *fname, const us4Image &outImg, const saveParams SP);
  template void tSave(const char *fname, const ui1Image &outImg, const saveParams SP);
//template void tSave(const char *fname, const ui2Image &outImg, const saveParams SP);
//template void tSave(const char *fname, const ui3Image &outImg, const saveParams SP);
//template void tSave(const char *fname, const ui4Image &outImg, const saveParams SP);
  template void tSave(const char *fname, const  f1Image &outImg, const saveParams SP);
//template void tSave(const char *fname, const  f2Image &outImg, const saveParams SP);
  template void tSave(const char *fname, const  f3Image &outImg, const saveParams SP);
//template void tSave(const char *fname, const  f4Image &outImg, const saveParams SP);
  template void tSave(const char *fname, const  h1Image &outImg, const saveParams SP);
//template void tSave(const char *fname, const  h2Image &outImg, const saveParams SP);
  template void tSave(const char *fname, const  h3Image &outImg, const saveParams SP);
//template void tSave(const char *fname, const  h4Image &outImg, const saveParams SP);
