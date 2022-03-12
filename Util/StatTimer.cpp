//////////////////////////////////////////////////////////////////////
// StatTimer.h - A timer used for a frame rate timer, etc.
//
// Copyright Dave McAllister, 2008.

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

void StatTimer::StartEvent()
{
    if (IsGoing) {
        float t = (float)Clock.Reset();
        memmove(&EventTimes[1], EventTimes, sizeof(float) * (MaxEvents - 1));
        EventTimes[0] = t;
        if (NumEvents < MaxEvents) NumEvents++;
    } else {
        Clock.Reset();
        Clock.Start();
        IsGoing = true;
        NumEvents = 0;
    }
}

float StatTimer::GetMean(int N)
{
    int NN = dmcm::Min(N, NumEvents, MaxEvents);

    float AccT = 0;
    int i = 0;
    for (i = 0; i < NN; i++) {
        AccT += EventTimes[i];
        // std::cerr << i << "=" << EventTimes[i] << ' ';
    }
    // std::cerr << std::endl;

    if (i)
        return AccT / float(NN);
    else
        return 0.0;
}

float StatTimer::GetMax(int N)
{
    int NN = dmcm::Min(N, NumEvents, MaxEvents);

    float AccT = 0;
    for (int i = 0; i < NN; i++) AccT = std::max(AccT, EventTimes[i]);

    return AccT;
}

float StatTimer::GetMin(int N)
{
    int NN = dmcm::Min(N, NumEvents, MaxEvents);

    float AccT = DMC_MAXFLOAT;
    for (int i = 0; i < NN; i++) AccT = std::min(AccT, EventTimes[i]);

    return AccT;
}
