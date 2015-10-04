#pragma once

#include "Interpolator.h"

class CubicInterpolator : public Interpolator
{
public:
	float Interpolate(unsigned long left,unsigned long right) const;
};
