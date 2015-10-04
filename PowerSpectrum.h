// Copyright 2004/2006 Marko Mihovilic

#pragma once

#include "Visualization.h"

class PowerSpectrum : public Visualization
{
public:
	PowerSpectrum() { SetName(L"Power Spectrum"); }

	bool Render();
	bool RenderOverlay();
};