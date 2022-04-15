#include "Image/PullPush.h"

#include "Image/tImage.h"
#include "Math/Random.h"

#include <iostream>

bool PullPushTest(int argc, char** argv)
{
    std::cerr << "Starting PullPushTest\n";

    ASSERT_R(argc > 1);
    uc3Image srcImg(argv[1]);
    std::cerr << srcImg.chan << std::endl;

    f3Image Im(srcImg.w(), srcImg.h());
    f1Image Wg(srcImg.w(), srcImg.h());
    Im.fill();
    Wg.fill();

    // Sparsely sample srcImg into Im and Wg
    int nSamples = srcImg.size() / 1000;
    for (int i = 0; i < nSamples; i++) {
        int j = LRand(srcImg.size());
        Im[j] = srcImg[j];
        Wg[j] = 1.0f;
    }

    Im.Save("preIm.png");
    Wg.Save("preWg.png");

    PullPush(Im.pp(), (float*)Wg.pp(), Im.w(), Im.h());

    Im.Save("outIm.png");
    Wg.Save("outWg.png");

    std::cerr << "Ending PullPushTest\n";

    return true;
}
