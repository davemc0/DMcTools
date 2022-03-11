#pragma once

#include "Image/QuantParams.h"

#include <string>

struct LoadSaveParams {
    std::string comment = "Written using DMcTools";
    QuantParams QP;               // Parameters for image quantization
    float exposure = 1.0f;        // Set outgoing exposure for HDR/RGBE
    bool wantPaletteInds = false; // True to return one-channel image in LoadGIF; no palette
    bool isR5G6B5 = false;        // True to interpret incoming 16 bits as R5G6B5 instead of X1R5G5B5 in TGA
    bool verbose = false;         // True for verbose debug prints
};
