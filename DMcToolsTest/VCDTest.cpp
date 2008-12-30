// Test VCD

#include "Image/ImageAlgorithms.h"
#include "Image/tImage.h"

using namespace std;

void VCDLoop()
{
    f3Image InImg("D:/Pictures/Stuff/By Us/MyArt/WoodFloor1.tif");
    InImg.Save("vcdout00.jpg");

    for(int i=1; i<15; i++) {
        cerr << "Starting VCD pass " << i << endl;

        f3Image VCDImg;
        VCD(VCDImg, InImg, 5, 3, 0.25, 1);

        char Name[100];
        sprintf(Name, "vcdout%02d.jpg", i);
        VCDImg.Save(Name);

        InImg = VCDImg;
    }
}

bool VCDTest(int argc, char **argv)
{
    cerr << "VCDTest\n";
    VCDLoop();

    return true;
}
