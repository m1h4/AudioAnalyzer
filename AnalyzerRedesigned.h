// Copyright 2004/2006 Marko Mihovilic

#pragma once

#include "Visualization.h"

class AnalyzerRedesigned : public Visualization
{
public:
	AnalyzerRedesigned() { mName = L"Digital Analyzer Redesigned"; }

	bool Render();
	bool RenderOverlay() { return true; }	// TODO Not yet implemented
};