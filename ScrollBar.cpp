#include "Globals.h"
#include "Kernel.h"
#include "ScrollBar.h"

ScrollBar::ScrollBar(void) :
m_TrackCount(NULL),
m_TrackPosition(NULL),
m_TrackRange(NULL),
m_UpPressed(false),
m_UpEnabled(true),
m_UpVisible(true),
m_DownPressed(false),
m_DownEnabled(true),
m_DownVisible(true),
m_TrackPressed(false),
m_TrackEnabled(true),
m_TrackVisible(true)
{
}

ScrollBar::~ScrollBar(void)
{
}

bool ScrollBar::OnRender(void)
{
	if(!GetVisible())
		return false;

	Rect box = GetBoundingBox();

	if(GetUpVisible())
	{
		GetKernel()->GetUserInterface()->DrawButton(Rect(box.left,box.top,box.right,box.top + box.GetWidth()),GetUpPressed(),GetUpEnabled(),false);
		GetKernel()->GetUserInterface()->DrawArrowUpSymbol(Rect(box.left,box.top,box.right,box.top + box.GetWidth()),GetUpPressed(),GetUpEnabled(),false);

		box.top += box.GetWidth();
	}

	if(GetDownVisible())
	{
		GetKernel()->GetUserInterface()->DrawButton(Rect(box.left,box.bottom - box.GetWidth(),box.right,box.bottom),GetDownPressed(),GetDownEnabled(),false);
		GetKernel()->GetUserInterface()->DrawArrowDownSymbol(Rect(box.left,box.bottom - box.GetWidth(),box.right,box.bottom),GetDownPressed(),GetDownEnabled(),false);

		box.bottom -= box.GetWidth();
	}

	GetKernel()->GetGraphics()->DrawFillRect(box,Color(223.0f / 255.0f,219.0f / 255.0f,211.0f / 255.0f));

	float move = (float)box.GetHeight() / (float)GetTrackRange();

	if(GetTrackVisible())
		GetKernel()->GetUserInterface()->DrawButton(Rect(box.left,box.top + move * GetTrackPosition(),box.right,box.top + move * GetTrackPosition() + move),GetTrackPressed(),GetTrackEnabled(),false);

	return Control::OnRender();
}

bool ScrollBar::OnEventMouse(Event event,WPARAM wparam,LPARAM lparam)
{
	if(!GetVisible() || !GetEnabled())
		return false;

	switch(event)
	{
	case EventLeftButtonDoubleClick:
	case EventLeftButtonDown:
		if(PointIn(*(Point*)wparam))
		{
			if(!GetFocused())
				Focus();

			Rect box = GetBoundingBox();

			if(GetUpVisible() && GetUpEnabled() && Rect(box.left,box.top,box.right,box.top + box.GetWidth()).PointIn(*(Point*)wparam))
			{
				SetUpPressed(true);

				return true;
			}

			if(GetDownVisible() && GetDownEnabled() && Rect(box.left,box.bottom - box.GetWidth(),box.right,box.bottom).PointIn(*(Point*)wparam))
			{
				SetDownPressed(true);

				return true;
			}

			return true;
		}

		break;

	case EventLeftButtonUp:
		if(GetUpPressed())
		{
			SetUpPressed(false);

			Rect box = GetBoundingBox();

			if(GetUpVisible() && GetUpEnabled() && Rect(box.left,box.top,box.right,box.top + box.GetWidth()).PointIn(*(Point*)wparam))
				if(GetTrackPosition())
					SetTrackPosition(GetTrackPosition() - 1);

			return true;
		}

		if(GetDownPressed())
		{
			SetDownPressed(false);

			Rect box = GetBoundingBox();

			if(GetDownVisible() && GetDownEnabled() && Rect(box.left,box.bottom - box.GetWidth(),box.right,box.bottom).PointIn(*(Point*)wparam))
				if(GetTrackRange() && GetTrackPosition() < GetTrackRange() - 1)
					SetTrackPosition(GetTrackPosition() + 1);

			return true;
		}

		break;
	}

	return Control::OnEventMouse(event,wparam,lparam);
}