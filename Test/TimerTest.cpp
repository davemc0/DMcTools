// Test Timer

#include "Util/Timer.h"

#include <iostream>

void TimerLoop()
{
    const int LoopCount = 100000000;

    Timer Tim;
    Tim.Reset();
    Tim.Start();

    double diffSum = 0, diffSumSqr = 0, last = Tim.Read();
    int unique = 0;

    Timer Tester;

    for (int i = 0; i < LoopCount; i++) {
        double d = Tim.Read();
        double diff = d - last;
        if (diff > 0) {
            diffSum += diff;
            diffSumSqr += diff * diff;
            unique++;
            last = d;
        }
    }
    double runTime = Tester.Read();

    std::cerr << LoopCount << " samples in " << runTime << " secs: " << (LoopCount / runTime) << " samples/sec" << std::endl;
    std::cerr << unique << " clock ticks detected: " << (unique / runTime) << " ticks/sec" << std::endl;
}

bool TimerTest(int argc, char** argv)
{
    std::cerr << "Starting TimerTest\n";

    TimerLoop();

    std::cerr << "Ending TimerTest\n";

    return true;
}
