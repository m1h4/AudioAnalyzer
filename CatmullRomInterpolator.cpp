// Copyright 2004/2006 Marko Mihovilic

#include "Globals.h"
#include "CatmullRomInterpolator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

float CatmullRomInterpolator::Interpolate(unsigned long left,unsigned long right) const
{
	unsigned long leftleft = left > 0 ? left - 1 : left;
	unsigned long rightright = right < mStates.GetSize() - 1 ? right + 1 : right;

	return catmullromf(mValues[leftleft],mValues[left],mValues[right],mValues[rightright],(mTime - mStates[left]) / (mStates[right] - mStates[left]));
}