// Copyright 2004/2006 Marko Mihovilic

#include "Globals.h"
#include "Kernel.h"
#include "Scope.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool Scope::Render()
{
	// This will contain the line coordinates
	Vector2 lines[ANALYZER_FFTBUFFERSIZE/2];

	// Set the line width
	static const float width = 2.0f;
	static const float radius = 160.0f;

	// Set the coordinates for the shadow
	for(unsigned long x = 0; x < ANALYZER_FFTBUFFERSIZE/2; ++x)
		lines[x] = Vector2(40.0f+2.0f+1.4f*x,250.0f+2.0f+GetKernel()->GetAudio()->GetScope(x)*radius);

	// Create the shadow color
	Color color(GetKernel()->GetGraphics()->GetColorPreset()->foreground);
	color.Shadow();

	// Draw the line shadow
	GetKernel()->GetGraphics()->DrawLines(lines,ANALYZER_FFTBUFFERSIZE/2,color,width);

	// Set the coordinates for the scope line
	for(unsigned long x = 0; x < ANALYZER_FFTBUFFERSIZE/2; ++x)
		lines[x] = Vector2(40.0f+1.4f*x,250.0f+GetKernel()->GetAudio()->GetScope(x)*radius);

	// Draw the scope line
	GetKernel()->GetGraphics()->DrawLines(lines,ANALYZER_FFTBUFFERSIZE/2,GetKernel()->GetGraphics()->GetColorPreset()->foreground,width);

	return true;
}

bool Scope::RenderOverlay()
{
	// Draw the borders of the scope line
	GetKernel()->GetGraphics()->DrawShadowText(GetKernel()->GetGraphics()->GetBoldFont(),Point(35,242),L"|",GetKernel()->GetGraphics()->GetColorPreset()->peak);
	GetKernel()->GetGraphics()->DrawShadowText(GetKernel()->GetGraphics()->GetBoldFont(),Point(35+720-1,242),L"|",GetKernel()->GetGraphics()->GetColorPreset()->peak);

	// Draw the first label
	GetKernel()->GetGraphics()->DrawShadowColorText(GetKernel()->GetGraphics()->GetBoldFont(),Point(35,456),L"|-\00224.00\001 ms",GetKernel()->GetGraphics()->GetColorPreset()->text);

	// Draw the middle label
	Size size;
	GetKernel()->GetGraphics()->GetTextSize(GetKernel()->GetGraphics()->GetBoldFont(),L"-12.00 ms",&size);
	Rect rect(35,456,35+720,456);
	GetKernel()->GetGraphics()->DrawShadowColorText(GetKernel()->GetGraphics()->GetBoldFont(),Point(rect.left + rect.GetWidth() / 2 - size.cx / 2,rect.top),L"-\00212.00\001 ms",GetKernel()->GetGraphics()->GetColorPreset()->text);

	// Draw the end label
	GetKernel()->GetGraphics()->GetTextSize(GetKernel()->GetGraphics()->GetBoldFont(),L"00.00 ms",&size);
	GetKernel()->GetGraphics()->DrawShadowColorText(GetKernel()->GetGraphics()->GetBoldFont(),Point(rect.right - size.cx,rect.top),L"\00200.00\001 ms|",GetKernel()->GetGraphics()->GetColorPreset()->text);

	// Draw the meter vertical lines
	for(unsigned long x = 0; x < 73; ++x)
	{
		if(x == 0)	// First
			GetKernel()->GetGraphics()->DrawShadowText(GetKernel()->GetGraphics()->GetBoldFont(),Point(35+x*10,441),L"|",GetKernel()->GetGraphics()->GetColorPreset()->text[0]);
		else if(x == 72/2)	// Middle
			GetKernel()->GetGraphics()->DrawShadowText(GetKernel()->GetGraphics()->GetBoldFont(),Point(35+x*10,441),L"|",GetKernel()->GetGraphics()->GetColorPreset()->text[0]);
		else if(x == 72)	// Last
			GetKernel()->GetGraphics()->DrawShadowText(GetKernel()->GetGraphics()->GetBoldFont(),Point(35+x*10,441),L"|",GetKernel()->GetGraphics()->GetColorPreset()->text[0]);
		else	// Normal
			GetKernel()->GetGraphics()->DrawShadowText(GetKernel()->GetGraphics()->GetBoldFont(),Point(35+x*10,441),L"|",GetKernel()->GetGraphics()->GetColorPreset()->peak);
	}

	return true;
}