#include "Image/tImage.h"
#include "Math/MiscMath.h"
#include "Util/Timer.h"

#include <fstream>
#include <iostream>

bool ImageReadSpeedTest(int argc, char** argv) { return true; }

bool ImageWriteSpeedTest(int argc, char** argv)
{
    const int NUM_TIMES_TO_WRITE = 100;

    uc3Image TestIm("C:\\Users\\DaveMc\\Pictures\\Stuff\\Olympus\\HiRes\\0802\\D20080215_143356.jpg");

    Timer T;
    T.Reset();

    T.Start();
    for (int i = 0; i < NUM_TIMES_TO_WRITE; i++) { TestIm.Save("C:\\Windows\\Temp\\testout.jpg"); }

    float Sec = T.Read();

    std::cerr << "Average write time = " << (Sec / float(NUM_TIMES_TO_WRITE)) << std::endl;

    return true;
}

bool ImageRWSpeedTest(int argc, char** argv)
{
    std::cerr << "Starting ImageRWSpeedTest\n";

    ImageReadSpeedTest(argc, argv);
    ImageWriteSpeedTest(argc, argv);

    std::cerr << "Ending ImageRWSpeedTest\n";

    return true;
}
