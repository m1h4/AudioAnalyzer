// Copyright 2004/2006 Marko Mihovilic

#include "Globals.h"
#include "Kernel.h"
#include "VuMeter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool VuMeter::RenderVu(const Vector2& center,double radius,double alpha)
{
	static const int total = 16;
	static const float width = 2.0f;

	for(int i = 0; i < total-2; ++i)
		GetKernel()->GetGraphics()->DrawLine(Vector2(center.x + cos(0.04 + (M_PI/total*i) - M_PI / 1.1) * radius,center.y + sin(0.04 + (M_PI/total*i) - M_PI / 1.1) * radius),Vector2(center.x + cos(0.04 + (M_PI/total*i) - M_PI / 1.1) * radius * (1.1 + i * 0.01),center.y + sin(0.04 + (M_PI/total*i) - M_PI / 1.1) * radius * (1.1 + i * 0.01)),Color(GetKernel()->GetGraphics()->GetColorPreset()->peak.a * (min(1.0f,0.2f + float(i)/float(total))),GetKernel()->GetGraphics()->GetColorPreset()->peak),width);

	//GetKernel()->GetGraphics()->DrawLine(Vector2(center.x,center.y),Vector2(center.x + cos(alpha) * radius,center.y + sin(alpha) * radius),GetKernel()->GetGraphics()->GetColorPreset().foreground);
	//GetKernel()->GetGraphics()->DrawLine(Vector2(center.x,center.y),Vector2(center.x + cos(alpha+0.02) * radius,center.y + sin(alpha+0.02) * radius),GetKernel()->GetGraphics()->GetColorPreset().foreground);
	//GetKernel()->GetGraphics()->DrawLine(Vector2(center.x,center.y),Vector2(center.x + cos(alpha+0.04) * radius,center.y + sin(alpha+0.04) * radius),GetKernel()->GetGraphics()->GetColorPreset().foreground);

	//GetKernel()->GetGraphics()->DrawLine(Vector2(center.x + cos(alpha) * radius / 2.0f,center.y + sin(alpha) * radius / 2.0f),Vector2(center.x + cos(alpha) * radius,center.y + sin(alpha) * radius),GetKernel()->GetGraphics()->GetColorPreset().foreground);
	//GetKernel()->GetGraphics()->DrawLine(Vector2(center.x + cos(alpha+0.04) * radius / 2.0f,center.y + sin(alpha+0.04) * radius / 2.0f),Vector2(center.x + cos(alpha) * radius,center.y + sin(alpha) * radius),GetKernel()->GetGraphics()->GetColorPreset().foreground);
	//GetKernel()->GetGraphics()->DrawLine(Vector2(center.x + cos(alpha+0.08) * radius / 2.0f,center.y + sin(alpha+0.08) * radius / 2.0f),Vector2(center.x + cos(alpha) * radius,center.y + sin(alpha) * radius),GetKernel()->GetGraphics()->GetColorPreset().foreground);

	D3DSURFACE_DESC desc;
	GetKernel()->GetGraphics()->GetDailTexture(0).GetTexture()->GetLevelDesc(0,&desc);

	Color background = GetKernel()->GetGraphics()->GetColorPreset()->foreground;
	background.a *= 0.2f;
	background.Saturation(0.5f);
	background.Brighten(0.5f);

	GetKernel()->GetGraphics()->DrawTexture(GetKernel()->GetGraphics()->GetDailTexture(1),Point(center.x - desc.Width/2,center.y - desc.Height/2 - 55),Point(center.x,center.y - desc.Height/2 + 55),alpha + M_PI/2.0,background);
	GetKernel()->GetGraphics()->DrawTexture(GetKernel()->GetGraphics()->GetDailTexture(0),Point(center.x - desc.Width/2,center.y - desc.Height/2 - 55),Point(center.x,center.y - desc.Height/2 + 55),alpha + M_PI/2.0,GetKernel()->GetGraphics()->GetColorPreset()->foreground);

	return true;
}

bool VuMeter::Render()
{
	// Set the line width

	// Center of amoeba
	Point center(GetKernel()->GetGraphics()->GetDisplayWidth()/2,GetKernel()->GetGraphics()->GetDisplayHeight()/2);

	static const unsigned int blocks = 16;

	Vector2 pos1(center.x - 20,center.y + 200);
	Vector2 pos2(center.x + 20,center.y + 200);
	Vector2 size(25,50);

	for(unsigned int i = 0; i < blocks; ++i)
	{
		Color color1(GetKernel()->GetGraphics()->GetColorPreset()->foreground);
		color1.a = min(1.0f,GetKernel()->GetAudio()->GetVu(0)*blocks*1.1f - i);	// Totaly change the color's alpha
		color1.a *= min(1.0f,0.3f+(float)i/(float)blocks);

		Color color2(GetKernel()->GetGraphics()->GetColorPreset()->foreground);
		color2.a = min(1.0f,GetKernel()->GetAudio()->GetVu(1)*blocks*1.1f - i);	// Totaly change the color's alpha
		color2.a *= min(1.0f,0.3f+(float)i/(float)blocks);

		Color color3(GetKernel()->GetGraphics()->GetColorPreset()->foreground);
		color3.a *= min(1.0f,1.0f-(float)i/(float)blocks);
		color3.a *= 0.5f;

		Rect rect1(Point(pos1.x - size.x/2,pos1.y - size.y/2),Size(size.x,size.y));
		GetKernel()->GetGraphics()->DrawTexture(GetKernel()->GetGraphics()->GetBeatTexture(1),rect1,color3);
		GetKernel()->GetGraphics()->DrawTexture(GetKernel()->GetGraphics()->GetBeatTexture(0),rect1,color1);
		
		Rect rect2(Point(pos2.x - size.x/2,pos2.y - size.y/2),Size(size.x,size.y));
		GetKernel()->GetGraphics()->DrawTexture(GetKernel()->GetGraphics()->GetBeatTexture(1),rect2,color3);
		GetKernel()->GetGraphics()->DrawTexture(GetKernel()->GetGraphics()->GetBeatTexture(0),rect2,color2);

		double old = size.y;

		size.x *= 1.08f;
		size.y *= 1.08f;

		pos1.x -= size.x * 0.45f;
		pos1.y -= (size.y - old) / 2.0;

		pos2.x += size.x * 0.45f;
		pos2.y -= (size.y - old) / 2.0;
	}

	static const double radius = 100;

	double alpha1 = M_PI * GetKernel()->GetAudio()->GetVu(0) - M_PI / 1.1;
	double alpha2 = M_PI * GetKernel()->GetAudio()->GetVu(1) - M_PI / 1.1;

	RenderVu(Vector2(center.x - 120,center.y + 20),radius,alpha1);
	RenderVu(Vector2(center.x + 120,center.y + 20),radius,alpha2);

	return true;
}