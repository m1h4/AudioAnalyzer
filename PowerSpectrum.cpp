// Copyright 2004/2006 Marko Mihovilic

#include "Globals.h"
#include "Kernel.h"
#include "PowerSpectrum.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool PowerSpectrum::Render()
{
	static const unsigned int elements = 2;

	unsigned long left = 22;
	unsigned long step = (GetKernel()->GetGraphics()->GetDisplayWidth() - left * 2) / (ANALYZER_FFTBUFFERSIZE/4/elements - 2);
	unsigned long width = step - 1;
	unsigned long bottom = GetKernel()->GetGraphics()->GetDisplayHeight() - 30;
	
	left = (GetKernel()->GetGraphics()->GetDisplayWidth() - (ANALYZER_FFTBUFFERSIZE/4/elements - 2) * step)/2;

	// Draw the bars
	for(unsigned long x = 0; x < ANALYZER_FFTBUFFERSIZE/4/elements - 2; ++x)
	{
		float level = 0.0;

		for(unsigned long i = 0; i < elements; ++i)
			level += GetKernel()->GetAudio()->GetPowerSpectrum(1+x*elements+i);

		level /= (float)elements;

		Rect rect(left + x * step,bottom + level,left + x * step + width,bottom);
		
		Color color = ColorLerp(GetKernel()->GetGraphics()->GetColorPreset()->foreground,GetKernel()->GetGraphics()->GetColorPreset()->peak,min(1.0f,-level/400.0f));

		GetKernel()->GetGraphics()->DrawFillRect(rect,color,color,GetKernel()->GetGraphics()->GetColorPreset()->foreground,GetKernel()->GetGraphics()->GetColorPreset()->foreground);

		//rect.Inflate(-1,-1);
		//GetKernel()->GetGraphics()->DrawShadowRect(rect,GetKernel()->GetGraphics()->GetColorPreset()->peak);
	}

	return true;
}

bool PowerSpectrum::RenderOverlay()
{
	static const unsigned int elements = 2;

	unsigned long left = 22;
	unsigned long step = (GetKernel()->GetGraphics()->GetDisplayWidth() - left * 2) / (ANALYZER_FFTBUFFERSIZE/4/elements - 2);
	unsigned long width = step - 1;
	unsigned long bottom = GetKernel()->GetGraphics()->GetDisplayHeight() - 30;
	
	left = (GetKernel()->GetGraphics()->GetDisplayWidth() - (ANALYZER_FFTBUFFERSIZE/4/elements - 2) * step)/2;

	// Draw the bottom status text
	GetKernel()->GetGraphics()->DrawShadowColorText(GetKernel()->GetGraphics()->GetBoldFont(),Point(left,bottom + 1),L"|20 Hz -",GetKernel()->GetGraphics()->GetColorPreset()->text);

	Size size;
	GetKernel()->GetGraphics()->GetTextSize(GetKernel()->GetGraphics()->GetBoldFont(),L"- 11025 Hz -",&size);
	
	Rect rect(22,bottom + 1,GetKernel()->GetGraphics()->GetDisplayWidth() - left,0);

	GetKernel()->GetGraphics()->DrawShadowColorText(GetKernel()->GetGraphics()->GetBoldFont(),Point(rect.left + rect.GetWidth() / 2 - size.cx / 2,rect.top),L"- 11025 Hz -",GetKernel()->GetGraphics()->GetColorPreset()->text);

	GetKernel()->GetGraphics()->GetTextSize(GetKernel()->GetGraphics()->GetBoldFont(),L"- 22050 Hz|",&size);

	GetKernel()->GetGraphics()->DrawShadowColorText(GetKernel()->GetGraphics()->GetBoldFont(),Point(rect.right - size.cx,rect.top),L"- 22050 Hz|",GetKernel()->GetGraphics()->GetColorPreset()->text);

	return true;
}