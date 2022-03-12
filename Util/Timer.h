//////////////////////////////////////////////////////////////////////
// Timer.h - A wall clock timer
//
// Copyright David K. McAllister, Mar. 1998, 2022.

// Simulates my dad's old stop watch. It had a stop / start switch and a reset button that were independent. Constructing a Timer resets it and starts it.
// Reset() it if you don't want to start it until later. Then press Stop() and then Read() it. Also, you can Read() it while it's going, and you can Start() and
// Stop() it multiple times without Reset()ing it, and the time will accumulate. Also, you can Reset() it while it is going and it will reset to 0 and go from
// there. All functions return the elapsed time before the function is applied.

#pragma once

#include <chrono>

class Timer {
public:
    Timer();

    double Start();
    double Stop();
    double Read();
    double Reset();

private:
    typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint;
    typedef std::chrono::duration<double> Duration;

    TimePoint startTime;  // Time point when the timer was most recently started
    Duration elapsedTime; // Total duration that it was started then stopped
    bool going;           // True if the timer is going now

    // Get time point relative to some baseline in the past
    TimePoint GetCurTime();
};
