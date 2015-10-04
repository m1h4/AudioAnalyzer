// Copyright 2004/2006 Marko Mihovilic

#pragma once

class Timer
{
public:
	Timer(void);
	~Timer(void);

	void		Reset(void);				// Resets the timer
	void		Start(void);				// Starts the timer
	void		Stop(void);					// Stop (or pause) the timer
	void		Advance(void);				// Advance the timer by 0.1 seconds

	double		GetAbsoluteTime(void);		// Get the absolute system time
	double		GetTime(void);				// Get the current time
	double		GetElapsedTime(void);		// Get the time that elapsed between GetElapsedTime() calls

	bool GetStopped(void) const { return mStopped; }

protected:
	bool mStopped;

	unsigned long long mTicksPerSec;
	unsigned long long mStopTime;
	unsigned long long mLastElapsedTime;
	unsigned long long mBaseTime;
};