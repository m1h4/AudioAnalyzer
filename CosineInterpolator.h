#pragma once

#include "Interpolator.h"

class CosineInterpolator : public Interpolator
{
public:
	float Interpolate(unsigned long left,unsigned long right) const;
};
