#include "Globals.h"
#include "Kernel.h"
#include "LazySpectrum.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool LazySpectrumA::Render()
{
	float left = 25.0f/800.0f * GetKernel()->GetGraphics()->GetDisplayWidth();
	float top = 40.0f/450.0f * GetKernel()->GetGraphics()->GetDisplayHeight();
	float width = 36.0f/800.0f * GetKernel()->GetGraphics()->GetDisplayWidth();
	float height = 20.0f/450.0f * GetKernel()->GetGraphics()->GetDisplayHeight();
	float cx = 64.0f/800.0f * GetKernel()->GetGraphics()->GetDisplayWidth();
	float cy = 32.0f/450.0f * GetKernel()->GetGraphics()->GetDisplayHeight();

	const unsigned long levels = 19;
	const unsigned long bands = 20;

	Graphics* gr = GetKernel()->GetGraphics();
	Audio* au = GetKernel()->GetAudio();

	Color background = GetKernel()->GetGraphics()->GetColorPreset()->foreground;
	background.a *= 0.2f;
	background.Saturation(0.5f);
	background.Brighten(0.5f);

	// Draw the background blocks
	for(unsigned long x = 0; x < bands; ++x)
		for(unsigned long y = 0; y < levels; ++y)
			gr->DrawTexture(gr->GetBandTexture(3),Rect(Point(left + x * width, top + y * height), Size(cx,cy)), background);

	for(unsigned long x = 0; x < bands/2; ++x)
	{
		// Draw the blocks
		for(unsigned long y = 0; y < levels; ++y)
		{
			Color color[2] = {GetKernel()->GetGraphics()->GetColorPreset()->foreground,GetKernel()->GetGraphics()->GetColorPreset()->foreground};
			float level[2] = {0.0f,0.0f};

			// Non-linear frequency mapping
			unsigned long lower = GetFrequencySubspectrum(bands/2 - (x + 1),bands/2,ANALYZER_FFTBUFFERSIZE/4,2,2);
			unsigned long upper = GetFrequencySubspectrum(bands/2 - x,bands/2,ANALYZER_FFTBUFFERSIZE/4,2,2);

			for(unsigned long k = lower; k < upper; ++k)
				level[0] += au->GetLazySpectrum(0, k);

			level[0] /= upper - lower;

			lower = GetFrequencySubspectrum(x,bands/2,ANALYZER_FFTBUFFERSIZE/4,2,2);
			upper = GetFrequencySubspectrum(x + 1,bands/2,ANALYZER_FFTBUFFERSIZE/4,2,2);

			for(unsigned long k = lower; k < upper; ++k)
				level[1] += au->GetLazySpectrum(1, k);
			
			level[1] /= upper - lower;

			if(level[0]/20.0f-y > levels*2/3)
				color[0] = GetKernel()->GetGraphics()->GetColorPreset()->peak;
			if(level[1]/20.0f-y > levels*2/3)
				color[1] = GetKernel()->GetGraphics()->GetColorPreset()->peak;

			level[0] = level[0]/20.0f - levels + y;
			level[1] = level[1]/20.0f - levels + y;

			color[0].a *= max(0.0f, min(1, level[0])) * min(1.0f,0.5f+(float)(levels-y)/(float)levels);
			color[1].a *= max(0.0f, min(1, level[1])) * min(1.0f,0.5f+(float)(levels-y)/(float)levels);

			gr->DrawTexture(gr->GetBandTexture(2),Rect(Point(left + x * width,top + y * height),Size(cx,cy)),color[0]);
			gr->DrawTexture(gr->GetBandTexture(2),Rect(Point(left + x * width + bands/2*width,top + y * height),Size(cx,cy)),color[1]);
		}
	}

	return true;
}

bool LazySpectrumB::Render()
{
	float hscale = GetKernel()->GetGraphics()->GetDisplayWidth()/(float)WINDOW_WIDTH;
	float vscale = GetKernel()->GetGraphics()->GetDisplayHeight()/(float)WINDOW_HEIGHT;

	const float lmargin = 21 * hscale;
	const float tmargin = 30 * vscale;

	const float hmargin = 2 * hscale;
	const float vmargin = 2 * vscale;
	
	const float width = 36 * hscale;
	const float height = 11 * vscale;

	const unsigned long levels = 35;
	const unsigned long bands = 20;

	Color background = GetKernel()->GetGraphics()->GetColorPreset()->foreground;
	background.a *= 0.2f;
	background.Saturation(0.5f);
	background.Brighten(0.5f);

	Graphics* gr = GetKernel()->GetGraphics();
	Audio* au = GetKernel()->GetAudio();

	for(unsigned long x = 0; x < bands/2; ++x)
	{
		for(unsigned long y = 0; y < levels; ++y)
		{
			Color color[2] = {gr->GetColorPreset()->foreground,gr->GetColorPreset()->foreground};

			float level[2] = {0.0f,0.0f};

			// Linear frequency mapping
			//for(unsigned long k = x*ANALYZER_FFTBUFFERSIZE/4/bands*2; k < (x+1)*ANALYZER_FFTBUFFERSIZE/4/bands*2; ++k)
			//{
			//	level[0] += au->GetLazySpectrum(0, ANALYZER_FFTBUFFERSIZE/4-1 - k);
			//	level[1] += au->GetLazySpectrum(1, k);
			//}
			//
			//level[0] /= ANALYZER_FFTBUFFERSIZE/4/bands*2;
			//level[1] /= ANALYZER_FFTBUFFERSIZE/4/bands*2;

			// Non-linear frequency mapping
			unsigned long lower = GetFrequencySubspectrum(bands/2 - (x + 1),bands/2,ANALYZER_FFTBUFFERSIZE/4,2,2);
			unsigned long upper = GetFrequencySubspectrum(bands/2 - x,bands/2,ANALYZER_FFTBUFFERSIZE/4,2,2);

			for(unsigned long k = lower; k < upper; ++k)
				level[0] += au->GetLazySpectrum(0, k);

			level[0] /= upper - lower;

			lower = GetFrequencySubspectrum(x,bands/2,ANALYZER_FFTBUFFERSIZE/4,2,2);
			upper = GetFrequencySubspectrum(x + 1,bands/2,ANALYZER_FFTBUFFERSIZE/4,2,2);

			for(unsigned long k = lower; k < upper; ++k)
				level[1] += au->GetLazySpectrum(1, k);
			
			level[1] /= upper - lower;

			if(level[0]/12.0f-y > levels*2/3)
				color[0] = GetKernel()->GetGraphics()->GetColorPreset()->peak;
			if(level[1]/12.0f-y > levels*2/3)
				color[1] = GetKernel()->GetGraphics()->GetColorPreset()->peak;

			level[0] = level[0]/12.0f - levels + y;
			level[1] = level[1]/12.0f - levels + y;

			color[0].a *= min(1.0f,0.5f+(float)(levels-y)/(float)levels);
			color[1].a *= min(1.0f,0.5f+(float)(levels-y)/(float)levels);

			color[0].Lerp(background, max(0.0f, min(1, 1 - level[0])));
			color[1].Lerp(background, max(0.0f, min(1, 1 - level[1])));

			gr->DrawFillRect(Rect(Point(lmargin + width*x + hmargin*x, tmargin + y*height + vmargin*y),Size(width,height)), color[0]);
			gr->DrawFillRect(Rect(Point(lmargin + width*bands/2 + hmargin*bands/2 + width*x + hmargin*x, tmargin + y*height + vmargin*y),Size(width,height)), color[1]);
		}
	}

	return true;
}
