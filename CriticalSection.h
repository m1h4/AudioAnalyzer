#pragma once

class CriticalSection
{
public:
	CriticalSection(void) { InitializeCriticalSection(&mCriticalSection); }
	~CriticalSection(void) { DeleteCriticalSection(&mCriticalSection); }

	void Enter(void) { EnterCriticalSection(&mCriticalSection); }
	void Leave(void) { LeaveCriticalSection(&mCriticalSection); }

protected:
	CRITICAL_SECTION	mCriticalSection;
};

class EnterCriticalSection
{
public:
	EnterCriticalSection(CriticalSection& section) : mSection(section) { mSection.Enter(); }
	~EnterCriticalSection(void) { mSection.Leave(); }

protected:
	CriticalSection& mSection;
};