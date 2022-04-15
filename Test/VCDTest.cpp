// Test VCD

#include "Image/ImageAlgorithms.h"
#include "Image/tImage.h"

void VCDLoop()
{
    //    f3Image InImg("C:/Users/davemc/Pictures/FiligreeBird2.jpg");
    f3Image InImg("Flattened.png");
    InImg.Save("vcdout00.png");

    for (int i = 1; i < 15; i++) {
        std::cerr << "Starting VCD pass " << i << std::endl;

        f3Image VCDImg;
        VCD(VCDImg, InImg, 5, 3, 0.25, 1);

        char Name[100];
        sprintf(Name, "vcdout%02d.png", i);
        VCDImg.Save(Name);

        InImg = VCDImg;
    }
}

bool VCDTest(int argc, char** argv)
{
    std::cerr << "Starting VCDTest\n";

    VCDLoop();

    std::cerr << "Ending VCDTest\n";

    return true;
}
