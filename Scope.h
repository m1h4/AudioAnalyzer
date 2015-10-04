// Copyright 2004/2006 Marko Mihovilic

#pragma once

#include "Visualization.h"

class Scope : public Visualization
{
public:
	Scope() { SetName(L"Scope"); }

	bool Render();
	bool RenderOverlay();
};
