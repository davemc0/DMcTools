//////////////////////////////////////////////////////////////////////
// StatTimer.h - A timer used for a frame rate timer with a sliding window of frame times
//
// Copyright David K. McAllister, 2008.

#include "Util/Timer.h"

// Call Event() at the start of each frame.
// It keeps track of when each call to NextFrame occurs, and you can get statistics on this.
class StatTimer {
    Timer Clock;
    float* EventTimes; // A list of the elapsed times between adjacent calls to NextEvent().
    bool IsGoing;      // Has Event() been called yet?
    int NumEvents, MaxEvents;

public:
    StatTimer(int MaxEvents = 100);
    ~StatTimer();

    // Empty the event log and reset the timer. The next Event() will start it ticking again.
    void Reset();

    // Inserts the current time into the event log. Returns time of this frame.
    float Event();

    // Get stats on the most recent up-to-MaxEvents frames.
    float GetMean();
    float GetMin();
    float GetMax();
};
