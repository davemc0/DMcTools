//////////////////////////////////////////////////////////////////////
// Timer.cpp - A wall clock timer.
//
// Copyright David K. McAllister, Mar. 1998.
//#define MIDL_PASS

#include "toolconfig.h"
#include <Util/Timer.h>
#include <Math/MiscMath.h>

#ifdef DMC_MACHINE_win
#include <windows.h>
#include <sys/timeb.h>

#define DMC_USE_PENTIUM_TIMER
#endif

#ifdef DMC_MACHINE_sgi
#include <sys/times.h>
#include <limits.h>
#include <unistd.h>

static double clock_interval = 1./double(CLK_TCK);
#endif

#ifdef DMC_MACHINE_hp
#include <limits.h>
#include <sys/times.h>

static double clock_interval = 1./double(CLK_TCK);
#endif

#ifdef DMC_MACHINE_win
#ifdef DMC_USE_PENTIUM_TIMER
__forceinline  void GetPentiumCounter(DWORD &hix, DWORD &lox)
{
    DWORD hi, lo;

	__asm
	{
		xor   eax,eax	          // VC won't realize that eax is modified w/out this
								  //   instruction to modify the val.
								  //   Problem shows up in release mode builds
		_emit 0x0F		          // Pentium high-freq counter to edx;eax
		_emit 0x31
        mov   hi,edx
		mov   lo,eax
		xor   edx,edx	          // so VC gets that edx is modified
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

    double PThisTicks = double(hi) * 4294967296.0 + double(lo);

    double ThisTime = PThisTicks * Multiplier;

    return ThisTime;
}

#else

// Slow, inaccurate timer.
double Timer::GetCurTime()
{
	struct _timeb timebuffer;
	_ftime( &timebuffer );
	
	time_t sec = timebuffer.time;
	unsigned short milli = timebuffer.millitm;
	double dtime = double(sec) + double(milli) * 0.001;
	
	return dtime;
}

#endif
#endif

#ifdef DMC_MACHINE_sgi
double Timer::GetCurTime()
{
	struct tms buffer;
	double dtime=double(times(&buffer)) * clock_interval;
	return dtime;
}
#endif

#ifdef DMC_MACHINE_hp
double Timer::GetCurTime()
{
	struct tms buffer;
	double dtime=double(times(&buffer)) * clock_interval;
	return dtime;
}
#endif

// Create the timer. It is stopped.
Timer::Timer()
{
#ifdef DMC_USE_PENTIUM_TIMER
    // Initialize the high performance Pentium timer.
    LARGE_INTEGER freq, tim0q, tim1q;
    DWORD hi0, lo0, hi1, lo1;
    QueryPerformanceFrequency(&freq);
//#define DMC_LAPTOP
#ifdef DMC_LAPTOP
    freqHigh = freq.x.HighPart;
    freqLow = freq.x.LowPart;
#else
    freqHigh = freq.HighPart;
    freqLow = freq.LowPart;
#endif

    QueryPerformanceCounter( &tim0q );
    GetPentiumCounter(hi0, lo0);
    Sleep(100);
    GetPentiumCounter(hi1, lo1);
    QueryPerformanceCounter( &tim1q );

    high_bias = hi0;
    
#ifdef DMC_LAPTOP
    double QRefTicks = (double(tim1q.x.HighPart - tim0q.x.HighPart) * 4294967296.0) +
        double(tim1q.x.LowPart) - double(tim0q.x.LowPart); // High probably 0.
    double QTicksPerSec = double(freq.x.HighPart) * 4294967296.0 + double(freq.x.LowPart);
#else
    double QRefTicks = (double(tim1q.HighPart - tim0q.HighPart) * 4294967296.0) +
        double(tim1q.LowPart) - double(tim0q.LowPart); // High probably 0.
    double QTicksPerSec = double(freq.HighPart) * 4294967296.0 + double(freq.LowPart);
#endif

    // double RefSecs = QRefTicks / QTicksPerSec;
    double PRefTicks = double(hi1 - hi0) * 4294967296.0 + double(lo1) - double(lo0);
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
	if(Going)
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

StatTimer::StatTimer(int _MaxEvents)
{
    MaxEvents = _MaxEvents;
    EventTimes = new float[MaxEvents];
    NumEvents = 0;
    IsGoing = false;
}

StatTimer::~StatTimer()
{
    if(EventTimes)
        delete [] EventTimes;
}

void StatTimer::Reset()
{
    NumEvents = 0;
    IsGoing = false;
}

void StatTimer::StartEvent()
{
    if(IsGoing) {
        float t = Clock.Reset();
        memmove(&EventTimes[1], EventTimes, sizeof(float) * (MaxEvents-1));
        EventTimes[0] = t;
        if(NumEvents < MaxEvents)
            NumEvents++;
    } else {
        Clock.Reset();
        Clock.Start();
        IsGoing = true;
        NumEvents = 0;
    }
}

float StatTimer::GetMean(int N)
{
    int NN = Min(N, NumEvents, MaxEvents);

    float AccT = 0;
    int i=0;
    for(i=0; i<NN; i++) {
        AccT += EventTimes[i];
        //cerr << i << "=" << EventTimes[i] << ' ';
    }
    //cerr << endl;

    if(i)
        return AccT / float(NN);
    else
        return 0.0;
}

float StatTimer::GetMax(int N)
{
    int NN = Min(N, NumEvents, MaxEvents);

    float AccT = 0;
    for(int i=0; i<NN; i++)
        AccT = Max(AccT, EventTimes[i]);

    return AccT;
}

float StatTimer::GetMin(int N)
{
    int NN = Min(N, NumEvents, MaxEvents);

    float AccT = DMC_MAXFLOAT;
    for(int i=0; i<NN; i++)
        AccT = Min(AccT, EventTimes[i]);

    return AccT;
}
