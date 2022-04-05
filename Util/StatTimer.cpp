//////////////////////////////////////////////////////////////////////
// StatTimer.cpp - A timer used for a frame rate timer with a sliding window of frame times
//
// Copyright David K. McAllister, 2008.

#include "Util/StatTimer.h"

#include "Math/MiscMath.h"

StatTimer::StatTimer(int MaxEvents_)
{
    MaxEvents = MaxEvents_;
    EventTimes = new float[MaxEvents];
    NumEvents = 0;
    IsGoing = false;
}

StatTimer::~StatTimer()
{
    if (EventTimes) delete[] EventTimes;
}

void StatTimer::Reset()
{
    NumEvents = 0;
    IsGoing = false;
}

float StatTimer::Event()
{
    float frameTime = 0;
    if (IsGoing) {
        frameTime = (float)Clock.Reset();
        memmove(&EventTimes[1], EventTimes, sizeof(float) * (MaxEvents - 1));
        EventTimes[0] = frameTime;
        if (NumEvents < MaxEvents) NumEvents++;
    } else {
        Clock.Reset();
        Clock.Start();
        IsGoing = true;
        NumEvents = 0;
    }

    return frameTime;
}

float StatTimer::GetMean()
{
    float AccT = 0;
    for (int i = 0; i < NumEvents; i++) AccT += EventTimes[i];

    if (NumEvents)
        return AccT / float(NumEvents);
    else
        return 0.0;
}

float StatTimer::GetMax()
{
    float AccT = 0;
    for (int i = 0; i < NumEvents; i++) AccT = std::max(AccT, EventTimes[i]);

    return AccT;
}

float StatTimer::GetMin()
{
    float AccT = DMC_MAXFLOAT;
    for (int i = 0; i < NumEvents; i++) AccT = std::min(AccT, EventTimes[i]);

    return AccT;
}
