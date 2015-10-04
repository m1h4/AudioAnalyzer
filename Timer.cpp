// Copyright 2004/2006 Marko Mihovilic

#include "Globals.h"
#include "Kernel.h"
#include "Timer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Timer::Timer(void) :
mStopped(true),
mBaseTime(0),
mStopTime(0),
mTicksPerSec(0),
mLastElapsedTime(0)
{
	QueryPerformanceFrequency((PLARGE_INTEGER)&mTicksPerSec);

	TRACE(L"Timer frequency: %llu.\n", mTicksPerSec);
}

Timer::~Timer(void)
{
}

void Timer::Reset(void)
{
	// Get either the current time or the stop time
	unsigned long long time;

	if(mStopTime != 0)
		time = mStopTime;
	else
		QueryPerformanceCounter((PLARGE_INTEGER)&time);

	mBaseTime = time;
	mLastElapsedTime = time;
	mStopTime = 0;
	mStopped = false;
}

void Timer::Start(void)
{
	// Get either the current time or the stop time
	unsigned long long time;

	if(mStopTime != 0)
		time = mStopTime;
	else
		QueryPerformanceCounter((PLARGE_INTEGER)&time);

	if(mStopped)
		mBaseTime += time - mStopTime;

	mStopTime = 0;
	mLastElapsedTime = time;
	mStopped = false;
}

void Timer::Stop(void)
{
	if(mStopped)
		return;

	// Get either the current time or the stop time
	unsigned long long time;

	if(mStopTime != 0)
		time = mStopTime;
	else
		QueryPerformanceCounter((PLARGE_INTEGER)&time);

	if(mStopped)
		mBaseTime += time - mStopTime;

	mStopTime = time;
	mLastElapsedTime = time;
	mStopped = true;
}

void Timer::Advance(void)
{
	mStopTime += mTicksPerSec / 10;
}

double Timer::GetAbsoluteTime(void)
{
	// Get either the current time or the stop time
	unsigned long long time;

	if(mStopTime != 0)
		time = mStopTime;
	else
		QueryPerformanceCounter((PLARGE_INTEGER)&time);

	if(mStopped)
		mBaseTime += time - mStopTime;

	return time / (double)mTicksPerSec;
}

double Timer::GetTime(void)
{
	// Get either the current time or the stop time
	unsigned long long time;

	if(mStopTime != 0)
		time = mStopTime;
	else
		QueryPerformanceCounter((PLARGE_INTEGER)&time);

	if(mStopped)
		mBaseTime += time - mStopTime;

	return (double)(time - mBaseTime) / (double)mTicksPerSec;;
}

double Timer::GetElapsedTime(void)
{
	// Get either the current time or the stop time
	unsigned long long time;

	if(mStopTime != 0)
		time = mStopTime;
	else
		QueryPerformanceCounter((PLARGE_INTEGER)&time);

	if(mStopped)
		mBaseTime += time - mStopTime;

    double fElapsedTime = (double)(time - mLastElapsedTime) / (double)mTicksPerSec;
    mLastElapsedTime = time;

    return fElapsedTime;
}