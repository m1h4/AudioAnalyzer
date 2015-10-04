// Copyright 2004/2006 Marko Mihovilic

#include "Globals.h"
#include "Kernel.h"
#include "AmoebaScope.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool AmoebaScope::Render()
{
	// This will contain the line coordinates
	Vector2 lines[ANALYZER_FFTBUFFERSIZE];

	// Center of amoeba
	Vector2 center(GetKernel()->GetGraphics()->GetDisplayWidth()/2,GetKernel()->GetGraphics()->GetDisplayHeight()/2);

	//static const float radius = 160.0f;
	//static const float baseradius = 80.0f;
	static const float width = 2.0f;

	float radius = 80.0f + GetKernel()->GetAudio()->GetVu() * 100.0f;
	float baseradius = 30.0f + GetKernel()->GetAudio()->GetVu() * 100.0f;

	// Set the coordinates for the shadow
	//for(unsigned long x = 0; x < ANALYZER_FFTBUFFERSIZE/2; ++x)
	//	lines[x] = center + Vector2(sinf(M_PI * (float)x/(float)(ANALYZER_FFTBUFFERSIZE/2)) * (GetKernel()->GetAudio()->GetScope(x) * radius + 65.0f) * 2.0f + 1.0f,cosf(M_PI * (float)x/(float)(ANALYZER_FFTBUFFERSIZE/2)) * (GetKernel()->GetAudio()->GetScope(x) * radius + baseradius) + 1.0f);
	//for(unsigned long x = 0; x < ANALYZER_FFTBUFFERSIZE/2; ++x)
	//	lines[ANALYZER_FFTBUFFERSIZE/2+x] = center + Vector2(sinf(M_PI + M_PI * (float)x/(float)(ANALYZER_FFTBUFFERSIZE/2)) * (GetKernel()->GetAudio()->GetScope(ANALYZER_FFTBUFFERSIZE/2-1-x) * radius + 65.0f) * 2.0f + 1.0f,cosf(M_PI + M_PI * (float)x/(float)(ANALYZER_FFTBUFFERSIZE/2)) * (GetKernel()->GetAudio()->GetScope(ANALYZER_FFTBUFFERSIZE/2-1-x) * radius + baseradius) + 1.0f);
	for(unsigned long x = 0; x < ANALYZER_FFTBUFFERSIZE/2; ++x)
		lines[x] = center + Vector2(sinf(M_PI * (float)x/(float)(ANALYZER_FFTBUFFERSIZE/2)) * (GetKernel()->GetAudio()->GetScope(x) * radius * 1.4f + baseradius) * 2.0f,cosf(M_PI * (float)x/(float)(ANALYZER_FFTBUFFERSIZE/2)) * (GetKernel()->GetAudio()->GetScope(x) * radius * 1.4f + baseradius));
	for(unsigned long x = 0; x < ANALYZER_FFTBUFFERSIZE/2; ++x)
		lines[ANALYZER_FFTBUFFERSIZE/2+x] = center + Vector2(sinf(M_PI + M_PI * (float)x/(float)(ANALYZER_FFTBUFFERSIZE/2)) * (GetKernel()->GetAudio()->GetScope(ANALYZER_FFTBUFFERSIZE/2-1-x) * radius * 1.4f + baseradius) * 2.0f,cosf(M_PI + M_PI * (float)x/(float)(ANALYZER_FFTBUFFERSIZE/2)) * (GetKernel()->GetAudio()->GetScope(ANALYZER_FFTBUFFERSIZE/2-1-x) * radius * 1.4f + baseradius));

	// Create the shadow color
	//Color color(GetKernel()->GetGraphics()->GetColorPreset()->foreground);
	//color.Shadow();

	Color color(GetKernel()->GetGraphics()->GetColorPreset()->peak);

	// Draw the line shadow
	GetKernel()->GetGraphics()->DrawLines(lines,ANALYZER_FFTBUFFERSIZE,color,width);

	// Set the coordinates for the scope line
	for(unsigned long x = 0; x < ANALYZER_FFTBUFFERSIZE/2; ++x)
		lines[x] = center + Vector2(sinf(M_PI * (float)x/(float)(ANALYZER_FFTBUFFERSIZE/2)) * (GetKernel()->GetAudio()->GetScope(x) * radius + baseradius) * 2.0f,cosf(M_PI * (float)x/(float)(ANALYZER_FFTBUFFERSIZE/2)) * (GetKernel()->GetAudio()->GetScope(x) * radius + baseradius));
	for(unsigned long x = 0; x < ANALYZER_FFTBUFFERSIZE/2; ++x)
		lines[ANALYZER_FFTBUFFERSIZE/2+x] = center + Vector2(sinf(M_PI + M_PI * (float)x/(float)(ANALYZER_FFTBUFFERSIZE/2)) * (GetKernel()->GetAudio()->GetScope(ANALYZER_FFTBUFFERSIZE/2-1-x) * radius + baseradius) * 2.0f,cosf(M_PI + M_PI * (float)x/(float)(ANALYZER_FFTBUFFERSIZE/2)) * (GetKernel()->GetAudio()->GetScope(ANALYZER_FFTBUFFERSIZE/2-1-x) * radius + baseradius));

	// Draw the scope line
	GetKernel()->GetGraphics()->DrawLines(lines,ANALYZER_FFTBUFFERSIZE,GetKernel()->GetGraphics()->GetColorPreset()->foreground,width);

	return true;
}