// Copyright 2004/2006 Marko Mihovilic

#pragma once

#include "Visualization.h"

class VuMeter : public Visualization
{
public:
	VuMeter() { SetName(L"Vu Meter"); }

	bool RenderVu(const Vector2& center,double radius,double alpha);
	bool Render();
};
