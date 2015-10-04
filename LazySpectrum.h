#pragma once

#pragma once

#include "Visualization.h"

class LazySpectrumA : public Visualization
{
public:
	LazySpectrumA() { mName = L"Lazy Spectrum A"; }

	bool Render();
	bool RenderOverlay() { return true; }	// TODO Not yet implemented
};

class LazySpectrumB : public Visualization
{
public:
	LazySpectrumB() { mName = L"Lazy Spectrum B"; }

	bool Render();
	bool RenderOverlay() { return true; }	// TODO Not yet implemented
};