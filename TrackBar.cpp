#include "Globals.h"
#include "Kernel.h"
#include "TrackBar.h"

TrackBar::TrackBar(void) :
mRange(0),
mValue(0),
mTracking(false)
{
}

TrackBar::~TrackBar(void)
{
}

bool TrackBar::OnRender(void)
{
	if(!GetVisible())
		return false;

	Rect box = GetBoundingBox();

	GetKernel()->GetUserInterface()->DrawTrackBar(box,GetTrackBox(),GetTrackRange(),GetTrackValue(),false,GetEnabled(),GetFocused());

	return Control::OnRender();
}

bool TrackBar::OnEventMouse(Event event,WPARAM wparam,LPARAM lparam)
{
	if(!GetVisible() || !GetEnabled())
		return false;

	switch(event)
	{
	case EventLeftButtonDown:
		if(PointInTrack(*(Point*)wparam))
		{
			if(!GetFocused())
				Focus();

			mTracking = true;
			mTrackingValue = mValue;
			mTrackingPosition = ((Point*)wparam)->x - GetTrackBox().GetCenter().x;

			SetCapture(GetKernel()->GetWindowHandle());

			return true;
		}

		if(PointIn(*(Point*)wparam))
		{
			if(!GetFocused())
				Focus();

			if(((Point*)wparam)->x > GetTrackBox().GetCenter().x)
			{
				if(GetTrackValue() < GetTrackRange())
					SetTrackValue(GetTrackValue() + 1);
			}
			else
			{
				if(GetTrackValue())
					SetTrackValue(GetTrackValue() - 1);
			}

			return true;
		}

		break;

	case EventLeftButtonUp:
		if(mTracking)
		{
			mTracking = false;

			ReleaseCapture();

			return true;
		}
		break;

	case EventMouseMove:
		if(mTracking)
		{
			Point delta = *(Point*)wparam;

			delta.x -= GetScreenPosition().x + mTrackingPosition;

			float val = (float)GetBoundingBox().GetWidth() / (float)GetTrackRange();

			if((float)delta.x / val > (float)GetTrackRange())
				SetTrackValue(GetTrackRange());
			else if((float)delta.x / val < 0.0f)
				SetTrackValue(0);
			else
				SetTrackValue((float)delta.x / val);

			if(GetParent())
				GetParent()->OnEvent(EventChildCommand,ControlEventTrackChange,(LPARAM)this);
		}

		break;
	}

	return Control::OnEventMouse(event,wparam,lparam);
}

bool TrackBar::OnEventKeyboard(Event event,WPARAM wparam,LPARAM lparam)
{
	if(!GetVisible() || !GetEnabled())
		return false;

	switch(event)
	{
	case EventKeyDown:
		if(wparam == VK_DOWN || wparam == VK_LEFT)
		{
			if(GetTrackValue())
				SetTrackValue(GetTrackValue() - 1);

			if(GetParent())
				GetParent()->OnEvent(EventChildCommand,ControlEventTrackChange,(LPARAM)this);

			return true;
		}

		if(wparam == VK_UP || wparam == VK_RIGHT)
		{
			if(GetTrackValue() < GetTrackRange())
				SetTrackValue(GetTrackValue() + 1);

			if(GetParent())
				GetParent()->OnEvent(EventChildCommand,ControlEventTrackChange,(LPARAM)this);

			return true;
		}

		if(wparam == VK_HOME)
		{
			SetTrackValue(0);

			if(GetParent())
				GetParent()->OnEvent(EventChildCommand,ControlEventTrackChange,(LPARAM)this);

			return true;
		}

		if(wparam == VK_END)
		{
			SetTrackValue(GetTrackRange());

			if(GetParent())
				GetParent()->OnEvent(EventChildCommand,ControlEventTrackChange,(LPARAM)this);

			return true;
		}
		break;
	}

	return Control::OnEventKeyboard(event,wparam,lparam);
}

Rect TrackBar::GetTrackBox(void) const
{
	Rect box = GetBoundingBox();

	Size size(8,box.GetHeight());

	return Rect(Point(box.left + box.GetWidth() * GetTrackValue() / GetTrackRange() - size.cx / 2,box.top + box.GetHeight() / 2 - size.cy / 2 + 2),size);
}