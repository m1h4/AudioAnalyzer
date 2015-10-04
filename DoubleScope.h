// Copyright 2004/2006 Marko Mihovilic

#pragma once

#include "Visualization.h"

class DoubleScope : public Visualization
{
public:
	DoubleScope() { SetName(L"Double Scope"); }

	bool Render();
	bool RenderOverlay();
};