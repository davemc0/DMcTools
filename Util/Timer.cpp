//////////////////////////////////////////////////////////////////////
// Timer.cpp - A wall clock timer
//
// Copyright David K. McAllister, Mar. 1998, 2022.

#include "Util/Timer.h"

#include <chrono>

Timer::TimePoint Timer::GetCurTime() { return std::chrono::high_resolution_clock::now(); }

// Create the timer as reset and started
Timer::Timer()
{
    going = true;
    startTime = GetCurTime();
    elapsedTime = Duration(0);
}

// Start or re-start the timer without updating elapsedTime
double Timer::Start()
{
    double elT = Read();
    going = true;
    startTime = GetCurTime();
    return elT;
}

// Stop the timer and update elapsedTime
double Timer::Stop()
{
    double elT = Read();
    going = false;
    elapsedTime += GetCurTime() - startTime;

    return elT;
}

// Read elapsed time, without stopping or starting the timer
// If timer is going, add in current time span, otherwise it's just elapsedTime.
double Timer::Read()
{
    if (going)
        return Duration(elapsedTime + (GetCurTime() - startTime)).count();
    else
        return elapsedTime.count();
}

// Reset the elapsed time to 0, without stopping or starting the timer
double Timer::Reset()
{
    double elT = Read();
    startTime = GetCurTime();
    elapsedTime = Duration(0);
    return elT;
}
