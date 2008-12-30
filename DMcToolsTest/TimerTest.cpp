// Test Timer

#include "Util/Timer.h"

#include <iostream>

using namespace std;

void TimerLoop()
{
    Timer Tim;
    Tim.Reset();
    Tim.Start();
    while(1) {
        cerr << Tim.Read() << endl;
    }
}

bool TimerTest(int argc, char **argv)
{
    cerr << "TimerTest\n";
    TimerLoop();

    return true;
}
