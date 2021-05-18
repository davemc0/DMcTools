//////////////////////////////////////////////////////////////////////
// Timer.cpp - A wall clock timer
//
// Copyright David K. McAllister, Mar. 1998.

#include "Util/Timer.h"

#include "Math/MiscMath.h"
#include "toolconfig.h"

#include <algorithm>

#ifdef DMC_MACHINE_win
#include <sys/timeb.h>
#include <windows.h>

#ifndef _WIN64
#define DMC_USE_PENTIUM_TIMER
#endif

#endif

#define DMC_2E32 4294967296.0

#if defined(DMC_MACHINE_gcc)
#include <limits.h>
#include <sys/times.h>
#include <unistd.h>

#ifdef CLK_TCK
static double clock_interval = 1. / double(CLK_TCK);
#else
static double clock_interval = 1. / double(CLOCKS_PER_SEC);
#endif

#endif

// TODO: Resolve the Pentium nonsense. Find an intrinsic for the query.
#ifdef DMC_USE_PENTIUM_TIMER
DMC_DECL void GetPentiumCounter(DWORD& hix, DWORD& lox)
{
    DWORD hi, lo;

    __asm
        {
        xor   eax,eax                          // VC won't realize that eax is modified w/out this
                                         //   instruction to modify the val.
                //   Problem shows up in release mode builds
        _emit 0x0F // Pentium high-freq counter to edx;eax
        _emit 0x31
        mov   hi,edx
        mov   lo,eax
        xor   edx,edx // so VC gets that edx is modified
        }

    hix = hi;
    lox = lo;
}

// Fast, accurate timer.
double Timer::GetCurTime()
{
    DWORD hi, lo;
    GetPentiumCounter(hi, lo);
    hi -= high_bias;

    double PThisTicks = double(hi) * DMC_2E32 + double(lo);

    double ThisTime = PThisTicks * Multiplier;

    return ThisTime;
}

#elif defined(DMC_MACHINE_win)

// Slow, inaccurate timer.
double Timer::GetCurTime()
{
    struct _timeb timebuffer;
    _ftime(&timebuffer);

    time_t sec = timebuffer.time;
    unsigned short milli = timebuffer.millitm;
    double dtime = double(sec) + double(milli) * 0.001;

    return dtime;
}

#elif defined(DMC_MACHINE_gcc)

double Timer::GetCurTime()
{
    struct tms buffer;
    double dtime = double(times(&buffer)) * clock_interval;
    return dtime;
}

#else
#error Need a timer for this architecture.
#endif

// Create the timer. It is stopped.
Timer::Timer()
{
#ifdef DMC_USE_PENTIUM_TIMER
    // Initialize the high performance Pentium timer.
    LARGE_INTEGER freq, tim0q, tim1q;
    DWORD hi0, lo0, hi1, lo1;
    QueryPerformanceFrequency(&freq);

    freqHigh = freq.HighPart;
    freqLow = freq.LowPart;

    QueryPerformanceCounter(&tim0q);
    GetPentiumCounter(hi0, lo0);
    Sleep(100);
    QueryPerformanceCounter(&tim1q);
    GetPentiumCounter(hi1, lo1);

    high_bias = hi0;

    double QRefTicks = (double(tim1q.HighPart - tim0q.HighPart) * DMC_2E32) + double(tim1q.LowPart) - double(tim0q.LowPart); // High probably 0.
    double QTicksPerSec = double(freq.HighPart) * DMC_2E32 + double(freq.LowPart);

    // double RefSecs = QRefTicks / QTicksPerSec;
    double PRefTicks = double(hi1 - hi0) * DMC_2E32 + double(lo1) - double(lo0);
    Multiplier = QRefTicks / (QTicksPerSec * PRefTicks);
#endif

    Going = false;
    StartTime = GetCurTime();
    ElapsedTime = 0;
}

// Start or re-start the timer.
double Timer::Start()
{
    Going = true;
    StartTime = GetCurTime();

    return ElapsedTime;
}

// Stop the timer and set ElapsedTime to be the total time it's run so far.
double Timer::Stop()
{
    Going = false;
    double CurTime = GetCurTime();
    ElapsedTime += CurTime - StartTime;
    StartTime = CurTime;

    return ElapsedTime;
}

// Return elapsed time on the timer.
// If the clock is still going, add in how long it's been going
// since the last time it was started.
// If it's not going, it's just ElapsedTime.
double Timer::Read()
{
    if (Going)
        return ElapsedTime + (GetCurTime() - StartTime);
    else
        return ElapsedTime;
}

// Reset the elapsed time to 0.
// Doesn't start or stop the clock. This is like Dad's old
// silver stopwatch. Return the elapsed time *before* it was reset.
double Timer::Reset()
{
    double CurTime = GetCurTime();
    double El = ElapsedTime + (CurTime - StartTime);
    StartTime = CurTime;
    ElapsedTime = 0;

    return El;
}

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
        float t = Clock.Reset();
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
        // cerr << i << "=" << EventTimes[i] << ' ';
    }
    // cerr << endl;

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
