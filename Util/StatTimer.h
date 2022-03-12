//////////////////////////////////////////////////////////////////////
// StatTimer.h - A timer used for a frame rate timer, etc.
//
// Copyright Dave McAllister, 2008.

#include "Util/Timer.h"

// Call StartEvent() at the start of each frame.
// It keeps track of when each call to NextFrame occurs, and you can get statistics on this.
class StatTimer {
    Timer Clock;
    float* EventTimes; // A list of the elapsed times between adjacent calls to NextEvent().
    bool IsGoing;      // Has StartEvent() been called yet?
    int NumEvents, MaxEvents;

public:
    StatTimer(int MaxEvents = 1000);
    ~StatTimer();

    // Empty the event log and reset the timer. The next StartEvent() will start it ticking again.
    void Reset();

    // Inserts the current time into the event log.
    void StartEvent();

    // N queries spanning up-to the most recent N frames.
    float GetMean(int N = 0x7fffffff);
    float GetMin(int N = 0x7fffffff);
    float GetMax(int N = 0x7fffffff);
    // double GetMedian(int N=0x7fffffff);
};
