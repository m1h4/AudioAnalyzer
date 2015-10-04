// Copyright 2004/2006 Marko Mihovilic

#include "Globals.h"
#include "Kernel.h"
#include "CubicInterpolator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

float CubicInterpolator::Interpolate(unsigned long left,unsigned long right) const
{
	return cubicf(mValues[left],mValues[right],(mTime - mStates[left]) / (mStates[right] - mStates[left]));
}