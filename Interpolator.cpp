// Copyright 2004/2006 Marko Mihovilic

#include "Globals.h"
#include "Kernel.h"
#include "Interpolator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void Interpolator::Update(void)
{
	if(!mStates.GetSize())
		return;

	if(mTime == mStates[mDestination])
		return;

	if(mTime < mStates[mDestination])
	{
		mTime += mSpeed * GetKernel()->GetElapsedTime();

		if(mTime > mStates[mDestination])
			mTime = mStates[mDestination];
	}
	else
	{
		mTime -= mSpeed * GetKernel()->GetElapsedTime();

		if(mTime < mStates[mDestination])
			mTime = mStates[mDestination];
	}
}

float Interpolator::GetValue(void) const
{
	if(mTime <= mStates.Front())
		return mValues.Front();

	if(mTime >= mStates.Back())
		return mValues.Back();

	unsigned long left;
	unsigned long right;

	for(unsigned long i = 0; i < mStates.GetSize(); ++i)
	{
		if(mTime <= mStates[i])
		{
			right = i;
			left = i - 1;

			break;
		}
	}

	return Interpolate(left,right);
}