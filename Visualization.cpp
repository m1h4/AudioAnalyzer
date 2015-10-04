#include "Globals.h"
#include "Kernel.h"
#include "Visualization.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool Visualization::RenderBackground()
{
	// Draw generic color preset background
	if(!GetCommandLineData()->GetVar(L"/blur",false))
	{
		if(GetKernel()->GetGraphics()->GetColorPreset()->backgroundtl.a > 0.0f || GetKernel()->GetGraphics()->GetColorPreset()->backgroundtr.a > 0.0f || GetKernel()->GetGraphics()->GetColorPreset()->backgroundbl.a > 0.0f || GetKernel()->GetGraphics()->GetColorPreset()->backgroundbr.a > 0.0f)
			GetKernel()->GetGraphics()->DrawFillRect(Rect(0,0,GetKernel()->GetGraphics()->GetDisplayWidth(),GetKernel()->GetGraphics()->GetDisplayHeight()),GetKernel()->GetGraphics()->GetColorPreset()->backgroundtl,GetKernel()->GetGraphics()->GetColorPreset()->backgroundtr,GetKernel()->GetGraphics()->GetColorPreset()->backgroundbr,GetKernel()->GetGraphics()->GetColorPreset()->backgroundbl);
		else
			GetKernel()->GetGraphics()->DrawFillRect(Rect(0,0,GetKernel()->GetGraphics()->GetDisplayWidth(),GetKernel()->GetGraphics()->GetDisplayHeight()),GetKernel()->GetGraphics()->GetColorPreset()->background);
	}
	else
	{
		Graphics* gr = GetKernel()->GetGraphics();

		gr->GetDevice()->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		gr->GetDevice()->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
		gr->GetDevice()->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_REVSUBTRACT);

		gr->DrawFillRect(Rect(0.0f,0.0f,gr->GetDisplayWidth(),gr->GetDisplayHeight()),Color(max(0.0f,min(1.0f,GetKernel()->GetElapsedTime()/1.5f)),1.0f,1.0f,1.0f));

		gr->SpriteFlush();

		gr->GetDevice()->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		gr->GetDevice()->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
		gr->GetDevice()->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);
	}

	return true;
}