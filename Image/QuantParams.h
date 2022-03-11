#pragma once

struct QuantParams {
    size_t maxColorPalette = 256;     // Maximum colors allowed
    int maxItersFast = 50;            // Max iterations in fast refinement step
    int maxIters = 10;                // Max iterations in full refinement step
    float targetErr = 0;              // Difference between this and last iteration's error that terminates the loop
    bool makeArtisticPalette = false; // True to make a palette that has more error, but includes the less-used but visibly important colors
};
