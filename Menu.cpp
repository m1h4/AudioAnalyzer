// Copyright 2004/2006 Marko Mihovilic

#include "Globals.h"
#include "Kernel.h"
#include "Menu.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void Menu::Render()
{
	static const float OutAnimTime = 0.5f;			// Default 0.5
	static const float SelAnimTime = 0.11f;		// Default 0.11
	static const unsigned long MaxMenuItems = 50;	// Default 50

	if(!mOpen && GetKernel()->GetAbsoluteTime() - GetOpenTime() > OutAnimTime)
		return;

	float anim = (GetKernel()->GetAbsoluteTime() - mOpenTime)/OutAnimTime;
	float anim2 = (GetKernel()->GetAbsoluteTime() - mSelectedTime)/SelAnimTime;

	if(mOpen)
		anim *= 4.0f;

	anim = min(1.0f,max(anim,0.0f));
	anim2 = min(1.0f,max(anim2,0.0f));

	if(!mOpen)
		anim = 1.0f - anim;

	// The first and last element to draw
	unsigned long from = max((long)GetSelected()-(long)MaxMenuItems/2,0);
	unsigned long to = min(from+MaxMenuItems,GetItemCount());

	float y = GetKernel()->GetGraphics()->GetDisplayHeight()/2;

	if(from && GetSelectedPrevious() < GetSelected())
		y -= anim*(GetKernel()->GetGraphics()->GetFontHeight(GetKernel()->GetGraphics()->GetBoldFont())*((to-from)+anim2)/2.0f);
	else if(from && GetSelectedPrevious() > GetSelected())
		y -= anim*(GetKernel()->GetGraphics()->GetFontHeight(GetKernel()->GetGraphics()->GetBoldFont())*((to-from)+(1.0f-anim2))/2.0f);
	else
		y -= anim*(GetKernel()->GetGraphics()->GetFontHeight(GetKernel()->GetGraphics()->GetBoldFont())*(to-from)/2.0f);

	for(unsigned long i = from; i < to; ++i)
	{
		String text;

		switch(GetItem(i)->GetType())
		{
		case MenuItem::Title:
		case MenuItem::Label:
			text = GetItem(i)->GetText();

			//if(GetItem(i)->GetType() == MenuItem::Title)
			//	text.ToUpper();

			if(text.GetLength())
			{
				text.Insert(0,L"|");
				text.Insert(text.GetLength(),L"|");
			}
			break;
		case MenuItem::Seperator:
			text = L"|";
			break;
		case MenuItem::Margin:
			break;
		}

		if(!text.GetLength())
		{
			y += GetKernel()->GetGraphics()->GetFontHeight(GetKernel()->GetGraphics()->GetBoldFont());
			continue;
		}

		Size size;
		GetKernel()->GetGraphics()->GetTextSize(GetKernel()->GetGraphics()->GetBoldFont(),text,&size);

		unsigned long numChars;
	
		if(mOpen)
			numChars = min((GetKernel()->GetAbsoluteTime() - mOpenTime)*200.0f,text.GetLength());
		else
			numChars = max(text.GetLength()-(GetKernel()->GetAbsoluteTime() - mOpenTime)*200.0f,0);

		if(numChars)
		{
			Rect rectText(Point(GetKernel()->GetGraphics()->GetDisplayWidth()-size.cx-5,y),size);
			Color base[2] = {GetKernel()->GetGraphics()->GetColorPreset()->text[0],GetKernel()->GetGraphics()->GetColorPreset()->text[1]};
			
			base[0].a *= (float)numChars/(float)text.GetLength();
			base[1].a *= (float)numChars/(float)text.GetLength();

			if(i == from && from)
			{
				base[0].a /= 2.0f;
				base[1].a /= 2.0f;
			}
			else if(i == to-1 && to != GetItemCount())
			{
				base[0].a /= 2.0f;
				base[1].a /= 2.0f;
			}

			text.Erase(numChars,text.GetLength() - numChars);

			GetKernel()->GetGraphics()->GetTextSize(GetKernel()->GetGraphics()->GetBoldFont(),text,&size);
			
			rectText = Rect(rectText.right - size.cx,rectText.top,rectText.right,rectText.bottom);

			if(i == GetSelected())
			{
				base[0].Cubic(GetKernel()->GetGraphics()->GetColorPreset()->background,anim2);
				base[1].Cubic(GetKernel()->GetGraphics()->GetColorPreset()->background,anim2);

				Rect rectBox(rectText);

				//rectBox.Inflate(-1,-1);
				//rectBox.SetHeight(rectBox.GetHeight()+2);

				//rectBox.Offset(rect.GetWidth()*(1.0f-anim2),0);

				rectBox.Inflate(cubicf(-rectBox.GetWidth()/2,0,anim2),0);
				//rectBox.Inflate(0,-rectBox.GetHeight()/2*(1.0f-anim2));
				//rectBox.SetWidthRight(rectBox.GetWidth()*anim2);

				GetKernel()->GetGraphics()->DrawShadowFillRect(rectBox,Color(base[0].a*anim2,GetKernel()->GetGraphics()->GetColorPreset()->text[0]));
				GetKernel()->GetGraphics()->DrawShadowCornerRect(rectBox,4,Color(base[0].a*anim2,GetKernel()->GetGraphics()->GetColorPreset()->background));
			}
			else if(i == GetSelectedPrevious())
			{
				base[0].Cubic(GetKernel()->GetGraphics()->GetColorPreset()->background,1.0f-anim2);
				base[1].Cubic(GetKernel()->GetGraphics()->GetColorPreset()->background,1.0f-anim2);

				Rect rectBox(rectText);

				//rectBox.Inflate(-1,-1);
				//rectBox.SetHeight(rectBox.GetHeight()+2);

				//rectBox.Offset(rect.GetWidth()*anim2,0);

				rectBox.Inflate(cubicf(0,-rectBox.GetWidth()/2,anim2),0);
				//rectBox.Inflate(0,-rectBox.GetHeight()/2*anim2);
				//rectBox.SetWidthRight(rectBox.GetWidth()*(1.0f-anim2));

				GetKernel()->GetGraphics()->DrawShadowFillRect(rectBox,Color(base[0].a*(1.0f-anim2),GetKernel()->GetGraphics()->GetColorPreset()->text[0]));
				GetKernel()->GetGraphics()->DrawShadowCornerRect(rectBox,4,Color(base[0].a*(1.0f-anim2),GetKernel()->GetGraphics()->GetColorPreset()->background));
			}

			if(GetItem(i)->GetStatic() && GetItem(i)->GetType() == MenuItem::Label)
			{
				base[0].Saturation(0.4f);
				base[1].Saturation(0.4f);

				base[0].Gamma(0.5f);
				base[1].Gamma(0.5f);
			}
			
			if(GetItem(i)->GetType() == MenuItem::Title)
			{
				Rect rectBox(rectText);

				rectBox.Inflate(-2,-1);
				rectBox.Offset(0,rectBox.GetHeight()+1);
				rectBox.SetHeight(1);
				GetKernel()->GetGraphics()->DrawShadowFillRect(rectBox,base[0]);
			}

			GetKernel()->GetGraphics()->DrawShadowColorText(GetKernel()->GetGraphics()->GetBoldFont(),Point(rectText.left,rectText.top),text,base);
			y += GetKernel()->GetGraphics()->GetFontHeight(GetKernel()->GetGraphics()->GetBoldFont())*anim;
		}
	}
}

bool Menu::KeyEvent(unsigned long key)
{
	switch(key)
	{
	case VK_UP:
		// Find the first item that has a non zero id (isn't static) and select it
		// start searching from the currently selected item to the first
		for(long i = GetSelected()-1; i > 0; --i)
		{
			if(!GetItem(i)->GetStatic())
			{
				SetSelected(i);
				return true;
			}
		}
		break;

	case VK_DOWN:
		// Find the first item that has a non zero id (isn't static) and select it
		// start searching from the currently selected item to the last
		for(unsigned long i = GetSelected()+1; i < GetItemCount(); ++i)
		{
			if(!GetItem(i)->GetStatic())
			{
				SetSelected(i);
				return true;
			}
		}
		break;

	case VK_RETURN:
		return ControlEvent(EVENT_PRESSED,GetSelected());

	case VK_BACK:
		SetOpen(false);
		break;
	}

	return false;
}

bool Menu::ControlEvent(unsigned long event,unsigned long sender)
{
	if(mEventHandler)
		return mEventHandler(event,GetItem(sender));

	return false;
}

bool Menu::MouseEvent(unsigned long event,const Point& delta)
{
	UNREFERENCED_PARAMETER(delta);

	static const unsigned long MaxMenuItems = 50;	// Default 50

	switch(event)
	{
	case 0:	// Mouse Move
		break;

	case 1:	// Mouse Left Button Down
		break;

	case 2:	// Mouse Left Button Up
		KeyEvent(VK_RETURN);
		break;

	case 3:	// Mouse Right Button Down
		break;

	case 4:	// Mouse Right Button Up
		break;

	case 5:	// Mouse Wheel Down
		KeyEvent(VK_DOWN);
		break;
	
	case 6:	// Mouse Wheel Up
		KeyEvent(VK_UP);
		break;
	}

	return false;
}

void Menu::SetOpen(bool open)
{ 
	if(open == mOpen)
		return;

	mOpen = open;
	mOpenTime = GetKernel()->GetAbsoluteTime();
}

void Menu::SetSelected(unsigned long index) 
{
	if(index % mItems.GetSize() == mSelected)
		return;

	mSelectedPrevious = mSelected;
	mSelected = index % mItems.GetSize();
	mSelectedTime = GetKernel()->GetAbsoluteTime();
}