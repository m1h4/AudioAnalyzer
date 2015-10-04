#include "Globals.h"
#include "Kernel.h"
#include "Button.h"

Button::Button(void) :
mPressed(false)
{
}

Button::~Button(void)
{
}

bool Button::OnRender(void)
{
	if(!GetVisible())
		return false;

	Rect box = GetBoundingBox();

	GetKernel()->GetUserInterface()->DrawButton(box,GetPressed(),GetEnabled(),GetFocused());
	GetKernel()->GetUserInterface()->DrawButtonText(box,mText,GetPressed(),GetEnabled(),GetFocused());

	return Control::OnRender();
}

bool Button::OnEventMouse(Event event,WPARAM wparam,LPARAM lparam)
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

			SetPressed(true);

			return true;
		}

		break;

	case EventLeftButtonUp:
		if(GetPressed())
		{
			SetPressed(false);

			if(PointIn(*(Point*)wparam))
				if(GetParent())
					GetParent()->OnEvent(EventChildCommand,ControlEventClicked,(LPARAM)this);

			return true;
		}

		break;
	}

	return Control::OnEventMouse(event,wparam,lparam);
}

bool Button::OnEventKeyboard(Event event,WPARAM wparam,LPARAM lparam)
{
	if(!GetVisible() || !GetEnabled())
		return false;

	switch(event)
	{
	case EventKeyDown:
		if(wparam == VK_SPACE)
		{
			SetPressed(true);

			return true;
		}

		break;

	case EventKeyUp:
		if(wparam == VK_SPACE)
		{
			if(GetPressed())
			{
				SetPressed(false);

				if(GetParent())
					GetParent()->OnEvent(EventChildCommand,ControlEventClicked,(LPARAM)this);
			}

			return true;
		}

		break;
	}

	return Control::OnEventKeyboard(event,wparam,lparam);
}