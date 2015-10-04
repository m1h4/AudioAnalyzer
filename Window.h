#pragma once

#include "Control.h"

typedef bool (CALLBACK* CommandCallback) (const ControlHandle& sender,WPARAM wparam,LPARAM lparam);

class CommandCallbackClass
{
public:
	virtual ~CommandCallbackClass() {}

	virtual bool OnCommandEvent(const ControlHandle& sender,WPARAM wparam,LPARAM lparam) = NULL;
};

class Window;

typedef Window* WindowHandle;

class Window : public Control
{
public:
	Window(void);
	virtual ~Window(void);

	virtual bool OnRender(void);
	virtual bool OnEvent(Event event,WPARAM wparam = NULL,LPARAM lparam = NULL);
	virtual bool OnEventMouse(Event event,WPARAM wparam = NULL,LPARAM lparam = NULL);
	virtual bool OnEventKeyboard(Event event,WPARAM wparam = NULL,LPARAM lparam = NULL);

	virtual void DetachChild(unsigned long index);

	virtual void SetSize(const Size& size);

	virtual void SetTitle(const String& title) { mTitle = title; }
	virtual const String& GetTitle(void) const { return mTitle; }

	virtual void SetModal(bool modal) { mModal = modal; }
	virtual bool GetModal(void) const { return mModal; }

	virtual void SetMovable(bool movable) { mMovable = movable; }
	virtual bool GetMovable(void) const { return mMovable; }

	virtual void SetSizable(bool sizable) { mSizable = sizable; }
	virtual bool GetSizable(void) const { return mSizable; }

	virtual void SetMaximumSize(const Size& size) { mMaximumSize = size; }
	virtual const Size& GetMaximumSize(void) const { return mMaximumSize; }

	virtual void SetMinimumSize(const Size& size) { mMinimumSize = size; }
	virtual const Size& GetMinimumSize(void) const { return mMinimumSize; }

	virtual bool PointInTitle(const Point& point) const;
	virtual bool PointInClient(const Point& point) const;
	virtual bool PointInEdge(const Point& point) const;
	virtual bool PointInClose(const Point& point) const;
	virtual bool PointInMinimize(const Point& point) const;

	virtual void SetCallback(const CommandCallback& callback) { mCallback = callback; }
	virtual const CommandCallback& GetCallback(void) const { return mCallback; }

	virtual void SetCallbackClass(CommandCallbackClass* callback) { mCallbackClass = callback; }
	virtual CommandCallbackClass* GetCallbackClass(void) const { return mCallbackClass; }

	virtual Rect GetCloseRect(void) const { Rect box = GetBoundingBox(); return Rect(box.right - 5 - 16,box.top + 5,box.right - 5,box.top + 5 + 14); }
	virtual Rect GetMinimizeRect(void) const { Rect box = GetBoundingBox(); return Rect(box.right - 5 - 16 - 2 - 16,box.top + 5,box.right - 5 - 16 - 2,box.top + 5 + 14); }

	virtual void SetClosePressed(bool pressed) { mClosePressed = pressed; }
	virtual bool GetClosePressed(void) const { return mClosePressed; }

	virtual void SetMinimizePressed(bool pressed) { mMinimizePressed = pressed; }
	virtual bool GetMinimizePressed(void) const { return mMinimizePressed; }

	virtual void SetCloseEnabled(bool enabled) { mCloseEnabled = enabled; }
	virtual bool GetCloseEnabled(void) const { return mCloseEnabled && GetEnabled(); }

	virtual void SetMinimizeEnabled(bool enabled) { mMinimizeEnabled = enabled; }
	virtual bool GetMinimizeEnabled(void) const { return mMinimizeEnabled && GetEnabled(); }

	virtual void SetCloseVisible(bool visible) { mCloseVisible = visible; }
	virtual bool GetCloseVisible(void) const { return mCloseVisible && GetVisible(); }

	virtual void SetMinimizeVisible(bool visible) { mMinimizeVisible = visible; }
	virtual bool GetMinimizeVisible(void) const { return mMinimizeVisible && GetVisible(); }

	virtual void Minimize(void);

protected:
	String	mTitle;

	bool	mModal;

	bool	mMovable;
	bool	mSizable;

	bool	mMoved;
	bool	mSized;

	bool	mClosePressed;
	bool	mMinimizePressed;
	bool	mCloseVisible;
	bool	mMinimizeVisible;
	bool	mCloseEnabled;
	bool	mMinimizeEnabled;

	Size	mMaximumSize;
	Size	mMinimumSize;

	Point	mMovedPosition;
	Point	mSizedPosition;

	ControlHandle	mMouseOverControl;

	CommandCallback	mCallback;
	CommandCallbackClass* mCallbackClass;
};
