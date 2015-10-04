// Copyright 2004/2006 Marko Mihovilic

#pragma once

#include "Visualization.h"

class AmoebaScope : public Visualization
{
public:
	AmoebaScope() { SetName(L"Amoeba Scope"); }

	bool Render();
};
