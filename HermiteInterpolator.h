#pragma once

#include "Interpolator.h"

class HermiteInterpolator : public Interpolator
{
public:
	float Interpolate(unsigned long left,unsigned long right) const;
};
