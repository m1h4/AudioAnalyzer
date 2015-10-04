#pragma once

#include "Interpolator.h"

class LinearInterpolator : public Interpolator
{
public:
	float Interpolate(unsigned long left,unsigned long right) const;
};
