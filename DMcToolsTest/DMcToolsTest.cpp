///////////////////////////////////////////////////////////////
// DMcToolsTest.cpp - Testing framework for parts of DMcTools that need testing
//
// Copyright 2008 by David K. McAllister

#include "Util/Assert.h"

#include <iostream>
#include <string>
using namespace std;

extern bool ImageRWSpeedTest(int argc, char **argv);
extern bool DiskReadSpeedTest(int argc, char **argv);
extern bool DiskWriteTest(int argc, char **argv);
extern bool GaussianTest(int argc, char **argv);
extern bool HashStringTest(int argc, char **argv);
extern bool KDTreeTest(int argc, char **argv);
extern bool Matrix44Test(int argc, char **argv);
extern bool PullPushTest(int argc, char **argv);
extern bool tImageTest(int argc, char **argv);
extern bool VCDTest(int argc, char **argv);
extern bool TimerTest(int argc, char **argv);

namespace {

    void Usage(const char *message = NULL, const bool Exit = true)
    {
        if(message)
            cerr << "\nERROR: " << message << endl;

        cerr << "Program options:\n";
        cerr << "-testall                Run all tests, with same args given to all of them\n";
        cerr << "-ImageRWSpeedTest\n";
        cerr << "-DiskReadSpeedTest\n";
        cerr << "-DiskWriteTest\n";
        cerr << "-GaussianTest\n";
        cerr << "-HashStringTest\n";
        cerr << "-KDTreeTest\n";
        cerr << "-Matrix44Test\n";
        cerr << "-PullPushTest\n";
        cerr << "-TimerTest\n";
        cerr << "-tImageTest\n";
        cerr << "-\n";
        cerr << "-\n";

        if(Exit)
            exit(1);
    }

    // Take a whack at the argument vector.
    void Args(int argc, char **argv)
    {
        for(int i=1; i<argc; i++) {
            if(string(argv[i]) == "-h" || string(argv[i]) == "-help") {
                Usage();
            } else if(string(argv[i]) == "-testall") {
                ImageRWSpeedTest(argc-i, &(argv[i]));
                DiskReadSpeedTest(argc-i, &(argv[i]));
                GaussianTest(argc-i, &(argv[i]));
                HashStringTest(argc-i, &(argv[i]));
                KDTreeTest(argc-i, &(argv[i]));
                Matrix44Test(argc-i, &(argv[i]));
                PullPushTest(argc-i, &(argv[i]));
                TimerTest(argc-i, &(argv[i]));
                tImageTest(argc-i, &(argv[i]));
                VCDTest(argc-i, &(argv[i]));
                DiskWriteTest(argc-i, &(argv[i]));

                return;
            }
            else if(string(argv[i]) == "-ImageRWSpeedTest") { ImageRWSpeedTest(argc-i, &(argv[i])); }
            else if(string(argv[i]) == "-DiskReadSpeedTest") { DiskReadSpeedTest(argc-i, &(argv[i])); }
            else if(string(argv[i]) == "-DiskWriteTest") { DiskWriteTest(argc-i, &(argv[i])); }
            else if(string(argv[i]) == "-GaussianTest") { GaussianTest(argc-i, &(argv[i])); }
            else if(string(argv[i]) == "-HashStringTest") { HashStringTest(argc-i, &(argv[i])); }
            else if(string(argv[i]) == "-KDTreeTest") { KDTreeTest(argc-i, &(argv[i])); }
            else if(string(argv[i]) == "-Matrix44Test") { Matrix44Test(argc-i, &(argv[i])); }
            else if(string(argv[i]) == "-PullPushTest") { PullPushTest(argc-i, &(argv[i])); }
            else if(string(argv[i]) == "-TimerTest") { TimerTest(argc-i, &(argv[i])); }
            else if(string(argv[i]) == "-tImageTest") { tImageTest(argc-i, &(argv[i])); }
            else if(string(argv[i]) == "-VCDTest") { VCDTest(argc-i, &(argv[i])); }
            else if(argv[i][0] == '-') {
                Usage("Unknown option.");
            } else {
                // A non-"-" arg
                Usage("Unknown option.");
            }
        }
    }

};

int main(int argc, char **argv)
{
    try {
        Args(argc, argv);
        cerr << "Finished with all tests.\n";
    }
    catch (DMcError &Er) {
        cerr << "DMcError caught in main(): " << Er.Er << endl;
        throw Er;
    }
    catch (...) {
        cerr << "Exception caught in main(). Bye.\n";
        throw;
    }
}
