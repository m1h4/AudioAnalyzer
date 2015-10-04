#pragma once

#include "Interpolator.h"

class CatmullRomInterpolator : public Interpolator
{
public:
	float Interpolate(unsigned long left,unsigned long right) const;
};
