#include "Globals.h"
#include "CosineInterpolator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

float CosineInterpolator::Interpolate(unsigned long left,unsigned long right) const
{
	return cospf(mValues[left],mValues[right],(mTime - mStates[left]) / (mStates[right] - mStates[left]));
}