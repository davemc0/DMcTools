///////////////////////////////////////////////////////////////
// DMcToolsTest.cpp - Testing framework for parts of DMcTools that need testing
//
// Copyright 2008 by David K. McAllister

#include "Util/Assert.h"

#include <iostream>
#include <string>

extern bool DiskReadTest(int argc, char** argv);
extern bool DiskWriteTest(int argc, char** argv);
extern bool GaussianTest(int argc, char** argv);
extern bool HashStringTest(int argc, char** argv);
extern bool ImageRWSpeedTest(int argc, char** argv);
extern bool KDTreeTest(int argc, char** argv);
extern bool MappingsTest(int argc, char** argv);
extern bool Matrix44Test(int argc, char** argv);
extern bool PullPushTest(int argc, char** argv);
extern bool TimerTest(int argc, char** argv);
extern bool VCDTest(int argc, char** argv);
extern bool tImageTest(int argc, char** argv);

namespace {

void Usage(const char* message = NULL, const bool Exit = true)
{
    if (message) std::cerr << "\nERROR: " << message << std::endl;

    std::cerr << "Program options:\n";
    std::cerr << "-testall                Run all tests, with same args given to all of them\n";
    std::cerr << "-DiskReadTest\n";
    std::cerr << "-DiskWriteTest\n";
    std::cerr << "-GaussianTest\n";
    std::cerr << "-HashStringTest\n";
    std::cerr << "-ImageRWSpeedTest\n";
    std::cerr << "-KDTreeTest\n";
    std::cerr << "-MappingsTest\n";
    std::cerr << "-Matrix44Test\n";
    std::cerr << "-PullPushTest\n";
    std::cerr << "-TimerTest\n";
    std::cerr << "-tImageTest\n";
    std::cerr << "-VCDTest\n";

    if (Exit) exit(1);
}

bool AllTests(int argc, char** argv)
{
    std::cerr << "Starting AllTests\n";

    bool ok = true, allok = true;

    ok = DiskWriteTest(argc, argv);
    allok = allok && ok;
    ok = DiskReadTest(argc, argv);
    allok = allok && ok;
    ok = GaussianTest(argc, argv);
    allok = allok && ok;
    ok = HashStringTest(argc, argv);
    allok = allok && ok;
    ok = ImageRWSpeedTest(argc, argv);
    allok = allok && ok;
    ok = KDTreeTest(argc, argv);
    allok = allok && ok;
    ok = MappingsTest(argc, argv);
    allok = allok && ok;
    ok = Matrix44Test(argc, argv);
    allok = allok && ok;
    ok = PullPushTest(argc, argv);
    allok = allok && ok;
    ok = TimerTest(argc, argv);
    allok = allok && ok;
    ok = tImageTest(argc, argv);
    allok = allok && ok;
    ok = VCDTest(argc, argv);
    allok = allok && ok;

    std::cerr << "Ending AllTests\n";

    return allok;
}

// Take a whack at the argument vector.
void Args(int argc, char** argv)
{
    if (argc == 1) {
        AllTests(0, argv);
        return;
    }

    for (int i = 1; i < argc; i++) {
        std::string starg(argv[i]);

        if (starg == "-h" || starg == "-help") {
            Usage();
        } else if (starg == "-DiskReadTest") {
            DiskReadTest(argc - i, &(argv[i]));
        } else if (starg == "-DiskWriteTest") {
            DiskWriteTest(argc - i, &(argv[i]));
        } else if (starg == "-GaussianTest") {
            GaussianTest(argc - i, &(argv[i]));
        } else if (starg == "-HashStringTest") {
            HashStringTest(argc - i, &(argv[i]));
        } else if (starg == "-ImageRWSpeedTest") {
            ImageRWSpeedTest(argc - i, &(argv[i]));
        } else if (starg == "-KDTreeTest") {
            KDTreeTest(argc - i, &(argv[i]));
        } else if (starg == "-MappingsTest") {
            MappingsTest(argc - i, &(argv[i]));
        } else if (starg == "-Matrix44Test") {
            Matrix44Test(argc - i, &(argv[i]));
        } else if (starg == "-PullPushTest") {
            PullPushTest(argc - i, &(argv[i]));
        } else if (starg == "-TimerTest") {
            TimerTest(argc - i, &(argv[i]));
        } else if (starg == "-tImageTest") {
            tImageTest(argc - i, &(argv[i]));
        } else if (starg == "-VCDTest") {
            VCDTest(argc - i, &(argv[i]));
        } else if (argv[i][0] == '-') {
            Usage("Unknown option.");
        } else {
            // A non-"-" arg
            Usage("Unknown option.");
        }
    }
}
}; // namespace

int main(int argc, char** argv)
{
    try {
        Args(argc, argv);
        std::cerr << "Finished with all tests.\n";
    }
    catch (DMcError& Er) {
        std::cerr << "DMcError caught in main(): " << Er.Er << std::endl;
        throw Er;
    }
    catch (...) {
        std::cerr << "Exception caught in main(). Bye.\n";
        throw;
    }
}
