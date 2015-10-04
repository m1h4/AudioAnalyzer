#pragma once

class Control;

typedef Control* ControlHandle;

class Control
{
public:
	Control(void);
	virtual ~Control(void);

	// Control events
	enum Event
	{
		// Mouse button events
		EventLeftButtonDown,
		EventLeftButtonUp,
		EventRightButtonDown,
		EventRightButtonUp,
		EventMiddleButtonDown,
		EventMiddleButtonUp,
		EventLeftButtonDoubleClick,
		EventRightButtonDoubleClick,
		EventMiddleButtonDoubleClick,
		
		// Mouse movement events
		EventMouseMove,
		EventMouseEnter,
		EventMouseLeave,

		// Keyboard events
		EventChar,
		EventKeyUp,
		EventKeyDown,

		// Control events
		EventClose,
		EventParentChange,
		EventGotChild,
		EventLostChild,

		// Focus events
		EventGotFocus,
		EventLostFocus,

		// Engine events
		EventUpdate,
		EventRender,
		EventRenderCursor,

		// Size/Position events
		EventSizeChange,
		EventPositionChange,

		// Text/Tooltip events
		EventTextChange,
		EventTooltipChange,

		// Flag events
		EventVisibleChange,
		EventEnabledChange,

		// Mouse enter
		EventMouseOverChange,

		// ID
		EventIDChange,

		// Child
		EventChildCommand,
	};

	// Snap modes
	enum Snap
	{
		SnapNone = 0x00000000,
		SnapLeft = 0x00000001,
		SnapTop = 0x0000002,
		SnapRight = 0x000004,
		SnapBottom = 0x00008,
	};

	virtual bool OnRender(void);
	virtual bool OnRenderEditorMode(void);
	virtual bool OnUpdate(void);
	virtual bool OnEvent(Event /*event*/,WPARAM /*wparam*/ = NULL,LPARAM /*lparam*/ = NULL) { return false; /*Not Implemented*/ }
	virtual bool OnEventMouse(Event /*event*/,WPARAM /*wparam*/ = NULL,LPARAM /*lparam*/ = NULL) { return false; /*Not Implemented*/ }
	virtual bool OnEventKeyboard(Event /*event*/,WPARAM /*wparam*/ = NULL,LPARAM /*lparam*/ = NULL) { return false; /*Not Implemented*/ }

	virtual bool OnEventMouseEditor(Event event,WPARAM wparam = NULL,LPARAM lparam = NULL);

	virtual void SetVisible(bool visible) { if(mVisible == visible) return; mVisible = visible; OnEvent(EventVisibleChange,GetVisible()); }
	virtual bool GetVisible(void) const { if(GetParent()) return mVisible && GetParent()->GetVisible(); return mVisible; }

	virtual void SetEnabled(bool enabled) { if(mEnabled == enabled) return; mEnabled = enabled; OnEvent(EventEnabledChange,GetEnabled()); }
	virtual bool GetEnabled(void) const { if(GetParent()) return mEnabled && GetParent()->GetEnabled(); return mEnabled; }

	virtual void Focus(void) { if(GetParent()) GetParent()->FocusChild(GetParent()->GetChildIndex(this)); else SetFocused(true); }
	virtual void Unfocus(void) { SetFocused(false); }
	virtual void SetFocused(bool focused) { if(mFocused == focused) return; mFocused = focused; if(GetFocused()) OnEvent(EventGotFocus); else OnEvent(EventLostFocus); }
	virtual bool GetFocused(void) const { if(GetParent()) return mFocused && GetParent()->GetFocused(); return mFocused; }

	virtual bool SetFocusNext(void);
	virtual bool SetFocusPrevious(void);

	virtual void SetMouseOver(bool over) { if(mMouseOver == over) return; mMouseOver = over; OnEvent(EventMouseOverChange,GetMouseOver()); }
	virtual bool GetMouseOver(void) const { return mMouseOver; }
	
	virtual void SetPosition(const Point& position) { if(mPosition == position) return; mPosition = position; OnEvent(EventPositionChange,(WPARAM)&GetPosition()); }
	virtual const Point& GetPosition(void) const { return mPosition; }
	virtual Point GetScreenPosition(void) const;
	virtual Rect GetBoundingBox(void) const { return Rect(GetScreenPosition(),mSize); }

	virtual void ToScreen(Point* point) const;
	virtual void ToClient(Point* point) const;

	virtual void SetSize(const Size& size);
	virtual const Size& GetSize(void) const { return mSize; }

	virtual void SetParent(const ControlHandle& parent) { if(mParent == parent) return; mParent = parent; OnEvent(EventParentChange,(WPARAM)GetParent()); }
	virtual const ControlHandle& GetParent(void) const { return mParent; }

	virtual void SetID(unsigned long id) { if(mID == id) return; mID = id; OnEvent(EventIDChange,GetID()); }
	virtual unsigned long GetID(void) const { return mID; }

	virtual void SetEditorMode(bool mode) { mEditorMode = mode; }
	virtual bool GetEditorMode(void) const { if(mParent) return mParent->GetEditorMode() && GetFocused(); return mEditorMode; }

	virtual void AttachChild(const ControlHandle& child);
	virtual void DetachChild(unsigned long index);
	virtual ControlHandle GetChild(unsigned long index);
	virtual unsigned long GetChildCount(void) const { return mChilds.GetSize(); }
	virtual void FocusChild(unsigned long index);
	virtual void UnfocusChilds(void);
	virtual unsigned long GetFocusedChild(void);
	virtual unsigned long GetChildIndex(const ControlHandle& child);
	virtual unsigned long GetChildIndex(unsigned long id);
	virtual ControlHandle GetChildAtPoint(const Point& point);
	virtual bool ContainsChild(const ControlHandle& child);

	virtual bool PointIn(const Point& point) const;

	virtual bool Close(void);

	virtual void SetSnap(unsigned long snap) { mSnap = snap; }
	virtual unsigned long GetSnap(void) const { return mSnap; }

protected:
	Array<ControlHandle>	mChilds;
	ControlHandle			mParent;

	bool	mVisible;
	bool	mEnabled;
	bool	mFocused;
	bool	mMouseOver;
	bool	mEditorMode;

	unsigned long	mID;
	unsigned long	mSnap;

	Point	mPosition;
	Size	mSize;
};