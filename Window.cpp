#include "Globals.h"
#include "Kernel.h"
#include "Window.h"

#include "Button.h"

Window::Window(void) :
mMouseOverControl(NULL),
mCallback(NULL),
mCallbackClass(NULL),
mModal(false),
mMovable(true),
mSizable(false),
mMoved(false),
mSized(false),
mClosePressed(false),
mMinimizePressed(false),
mCloseVisible(true),
mMinimizeVisible(true),
mCloseEnabled(true),
mMinimizeEnabled(true),
mMaximumSize(0,0),
mMinimumSize(0,0),
mMovedPosition(0,0),
mSizedPosition(0,0)
{
}

Window::~Window(void)
{
}

bool Window::OnRender(void)
{
	if(!GetVisible())
		return false;

	Rect box = GetBoundingBox();

	GetKernel()->GetUserInterface()->DrawWindow(box,false,GetEnabled(),GetFocused());

	box.Inflate(-3,-3);
	
	Size size;
	GetKernel()->GetGraphics()->GetTextSize(GetKernel()->GetGraphics()->GetUIFont(),mTitle,&size);
	GetKernel()->GetGraphics()->DrawRect(Rect(box.left,box.top,box.right,box.top + 18),GetFocused() && GetEnabled() ? GetKernel()->GetUserInterface()->GetTitleColor() : GetKernel()->GetUserInterface()->GetInactiveTitleColor());
	GetKernel()->GetGraphics()->DrawText(GetKernel()->GetGraphics()->GetUIFont(),Point(box.left + 3,box.top + 18 / 2 - size.cy / 2),mTitle,GetFocused() && GetEnabled() ? GetKernel()->GetUserInterface()->GetTextColor() : GetKernel()->GetUserInterface()->GetInactiveTextColor());

	if(GetCloseVisible())
	{
		box = GetCloseRect();

		GetKernel()->GetUserInterface()->DrawButton(box,GetClosePressed(),GetCloseEnabled(),false);
		GetKernel()->GetUserInterface()->DrawCloseSymbol(box,GetClosePressed(),GetCloseEnabled(),false);
	}

	if(GetMinimizeVisible())
	{
		box = GetMinimizeRect();

		GetKernel()->GetUserInterface()->DrawButton(box,GetMinimizePressed(),GetMinimizeEnabled(),false);
		GetKernel()->GetUserInterface()->DrawMinimizeSymbol(box,GetMinimizePressed(),GetMinimizeEnabled(),false);
	}

	return Control::OnRender();
}

bool Window::OnEventMouse(Event event,WPARAM wparam,LPARAM lparam)
{
	if(!GetVisible() || !GetEnabled())
		return false;

	switch(event)
	{
	case EventLeftButtonDown:
		if(PointIn(*(Point*)wparam))
		{
			if(!GetFocused())
			{
				Focus();

				return true;
			}

			if(GetCloseVisible() && PointInClose(*(Point*)wparam))
			{
				SetClosePressed(true);

				return true;
			}

			if(GetMinimizeVisible() && PointInMinimize(*(Point*)wparam))
			{
				SetMinimizePressed(true);

				return true;
			}

			if(PointInTitle(*(Point*)wparam) && mMovable)
			{
				mMoved = true;
				mMovedPosition = *(Point*)wparam;

				SetCapture(GetKernel()->GetWindowHandle());

				return true;
			}
			
			if(PointInEdge(*(Point*)wparam) && mSizable)
			{
				mSized = true;
				mSizedPosition = *(Point*)wparam;

				SetCapture(GetKernel()->GetWindowHandle());

				return true;
			}

			//return true;	// Moved to Focus()
		}
		else if(!GetModal())
		{
			if(GetFocused())
				Unfocus();
		}

		break;

	case EventLeftButtonUp:
		if(GetClosePressed())
		{
			SetClosePressed(false);

			if(PointInClose(*(Point*)wparam))
				Close();

			return true;
		}

		if(GetMinimizePressed())
		{
			SetMinimizePressed(false);

			if(PointInMinimize(*(Point*)wparam))
				Minimize();

			return true;
		}

		if(mMoved || mSized)
		{
			mMoved = false;
			mSized = false;

			ReleaseCapture();

			return true;
		}

		break;

	case EventMouseMove:
		// Check mouse entering/leaving controls
		ControlHandle child = GetChildAtPoint(*(Point*)wparam);

		if(mMouseOverControl != child)
		{
			if(mMouseOverControl)
				mMouseOverControl->SetMouseOver(false);

			mMouseOverControl = child;

			if(mMouseOverControl)
				mMouseOverControl->SetMouseOver(true);
		}

		if(mMoved || mSized)
		{
			if(mMoved)
			{
				SetPosition(GetPosition() + (*(Point*)wparam - mMovedPosition));
				mMovedPosition = *(Point*)wparam;
			}
		
			if(mSized)
			{
				Size size;

				size.cx = mSize.cx + ((*(Point*)wparam).x - mSizedPosition.x);
				size.cy = mSize.cy + ((*(Point*)wparam).y - mSizedPosition.y);
				
				SetSize(size);

				mSizedPosition = *(Point*)wparam;
			}

			return true;
		}

		break;
	}

	return Control::OnEventMouse(event,wparam,lparam);
}

bool Window::OnEventKeyboard(Event event,WPARAM wparam,LPARAM lparam)
{
	if(!GetVisible() || !GetEnabled())
		return false;

	if(event == EventKeyDown)
	{
		// Check tab focus switching
		if(wparam == VK_TAB)
		{
			if(HIWORD(GetKeyState(VK_SHIFT)))
				if(SetFocusPrevious())
					return true;
			
			if(SetFocusNext())
				return true;

			return true;
		}

		// Check cursor focus switching
		if(wparam == VK_DOWN || wparam == VK_LEFT)
			if(SetFocusNext())
				return true;

		// Check cursor focus switching
		if(wparam == VK_UP || wparam == VK_RIGHT)
			if(SetFocusPrevious())
				return true;
	}

	return Control::OnEventKeyboard(event,wparam,lparam);
}

bool Window::OnEvent(Event event,WPARAM wparam,LPARAM lparam)
{
	if(!GetVisible() || !GetEnabled())
		return false;

	ControlHandle child = NULL;
	unsigned long focused = GetFocusedChild();

	// First send the event to the focused control
	if(focused != 0xFFFFFFFF)
		if(mChilds[focused]->OnEvent(event,wparam,lparam))
			return true;

	switch(event)
	{
	case EventKeyDown:
	case EventKeyUp:
	case EventChar:
		// First send the event to the focused control
		if(focused != 0xFFFFFFFF)
			if(mChilds[focused]->OnEventKeyboard(event,wparam,lparam))
				return true;

		// Event not handled, pass it to us
		if(OnEventKeyboard(event,wparam,lparam))
			return true;

		break;

	case EventLeftButtonDown:
	case EventLeftButtonUp:
	case EventRightButtonDown:
	case EventRightButtonUp:
	case EventLeftButtonDoubleClick:
	case EventRightButtonDoubleClick:
	case EventMouseMove:
		// First send the event to the focused control
		if(focused != 0xFFFFFFFF)
			if(mChilds[focused]->OnEventMouse(event,wparam,lparam))
				return true;

		// The event didn't get handled, check if cursor is over a control
		// and if yes send the event to it
		child = GetChildAtPoint(*(Point*)wparam);
		if(child && child != this)
		{
			if(child->OnEvent(event,wparam,lparam))
				return true;

			if(child->OnEventMouse(event,wparam,lparam))
				return true;
		}

		// Event not handled, pass it to us
		if(OnEventMouse(event,wparam,lparam))
			return true;

		break;

	case EventChildCommand:
		// Fire callback
		if(mCallback)
		{
			if(mCallback((ControlHandle)lparam,wparam,NULL))
				return true;
		}
		else if(mCallbackClass)
		{
			if(mCallbackClass->OnCommandEvent((ControlHandle)lparam,wparam,NULL))
				return true;
		}

		break;
	}

	if(GetModal())
	{
		// Let the event be handled by the default handler
		if(Control::OnEvent(event,wparam,lparam))
			return true;

		// TODO Make the borders flash

		// The mouse move event must be passed to the UI server if we are a dialog
		// cause otherwise it would neven get handled by the server and the
		// mouse_leave/mouse_enter events wouldn't work
		if(event == EventMouseMove)
			if(PointIn(*(Point*)wparam))
				GetKernel()->GetUserInterface()->OnEventMouse(event,wparam,lparam);

		return true;
	}

	// Let the event be handled by the default handler
	return Control::OnEvent(event,wparam,lparam);
}

bool Window::PointInTitle(const Point& point) const
{
	Point position = GetScreenPosition();

	if(point.x < position.x || point.x > position.x + mSize.cx)
		return false;

	if(point.y < position.y || point.y > position.y + 18)
		return false;

	return true;
}

bool Window::PointInClient(const Point& point) const
{
	Point position = GetScreenPosition();

	if(point.x < position.x || point.x > position.x + mSize.cx)
		return false;

	if(point.y < position.y + 18 || point.y > position.y + mSize.cy)
		return false;

	return true;
}

bool Window::PointInEdge(const Point& point) const
{
	Point position = GetScreenPosition();

	if(point.x < position.x + mSize.cx - 8 || point.x > position.x + mSize.cx)
		return false;

	if(point.y < position.y + mSize.cy - 8 || point.y > position.y + mSize.cy)
		return false;

	return true;
}

bool Window::PointInClose(const Point& point) const
{
	return GetCloseRect().PointIn(point);
}

bool Window::PointInMinimize(const Point& point) const
{
	return GetMinimizeRect().PointIn(point);
}

void Window::Minimize(void)
{
	SetPosition(Point(0,mParent->GetSize().cy - 18 - 2 - 2));
}

void Window::SetSize(const Size& size)
{
	Size loc = size;

	if(mMaximumSize.cx && size.cx > mMaximumSize.cx)
		loc.cx = mMaximumSize.cx;
	if(mMaximumSize.cy && size.cy > mMaximumSize.cy)
		loc.cy = mMaximumSize.cy;

	if(mMinimumSize.cx && size.cx < mMinimumSize.cx)
		loc.cx = mMinimumSize.cx;
	if(mMinimumSize.cy && size.cy < mMinimumSize.cy)
		loc.cy = mMinimumSize.cy;

	Control::SetSize(loc);
}

void Window::DetachChild(unsigned long index)
{
	if(index >= mChilds.GetSize())
		return;

	if(mChilds[index] == mMouseOverControl)
		mMouseOverControl = NULL;

	Control::DetachChild(index);
}