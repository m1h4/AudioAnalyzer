// Copyright 2004/2006 Marko Mihovilic

#include "Globals.h"
#include "Kernel.h"
#include "AnalyzerRedesigned.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool AnalyzerRedesigned::Render()
{
	float left = 25.0f/800.0f * GetKernel()->GetGraphics()->GetDisplayWidth();
	float bottom = 395.0f/450.0f * GetKernel()->GetGraphics()->GetDisplayHeight();
	float width = 36.0f/800.0f * GetKernel()->GetGraphics()->GetDisplayWidth();
	float height = 20.0f/450.0f * GetKernel()->GetGraphics()->GetDisplayHeight();
	float cx = 64.0f/800.0f * GetKernel()->GetGraphics()->GetDisplayWidth();
	float cy = 32.0f/450.0f * GetKernel()->GetGraphics()->GetDisplayHeight();

	Graphics* gr = GetKernel()->GetGraphics();
	Audio* au = GetKernel()->GetAudio();

	Color background = GetKernel()->GetGraphics()->GetColorPreset()->foreground;
	background.a *= 0.2f;
	background.Saturation(0.5f);
	background.Brighten(0.5f);
	
	Color color;

	for(unsigned long x = 0; x < ANALYZER_NUMBANDS; ++x)
	{
		// Draw the background blocks
		for(unsigned long y = 0; y < 19; ++y)
		{
			// Set the scale factor for the bands
			gr->DrawTexture(gr->GetBandTexture(1),Rect(Point(left + x * width, bottom - y * height), Size(cx,cy)), background);
		}
	}

	for(unsigned long x = 0; x < ANALYZER_NUMBANDS; ++x)
	{
		// Draw the blocks
		for(unsigned long y = 0; y < 19; ++y)
		{
			color = GetKernel()->GetGraphics()->GetColorPreset()->foreground;
			color.a = min(1.0f,GetKernel()->GetAudio()->GetEq(x)/-20.0f-y);	// Totaly change the color's alpha
			color.a *= min(1.0f,0.5f+(float)y/19.0f);
			gr->DrawTexture(gr->GetBandTexture(0),Rect(Point(left + x * width, bottom - y * height), Size(cx,cy)), color);
		}
	}
	
	for(unsigned long x = 0; x < ANALYZER_NUMBANDS; ++x)
	{
		// Draw the peak
		float alpha = GetKernel()->GetAbsoluteTime() - gr->GetDrawPeaksTime();
		if(alpha > 0.2f)
			alpha = 1.0f;
		else
			alpha /= 0.2f;

		if(!GetKernel()->GetGraphics()->GetDrawPeaks())
			alpha = 1.0f - alpha;

		float peak = (au->GetPeaks(0,x).position + au->GetPeaks(1,x).position)/2.0f;

		color = Color(gr->GetColorPreset()->peak.a*min(1.0f,peak/20.0f),gr->GetColorPreset()->peak);
		color.a *= cubicf(0.0f,1.0f,alpha);
		gr->DrawTexture(gr->GetBandTexture(0),Rect(Point(left + x * width,bottom - height * ((int)(peak/20.0f + 0.5f))),Size(cx,cy)),color);
	}

	return true;
}