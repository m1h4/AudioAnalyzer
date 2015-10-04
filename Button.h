#pragma once

#include "Control.h"

class Button;

typedef Button* ButtonHandle;

class Button : public Control
{
public:
	Button(void);
	~Button(void);

	enum ControlEvent
	{
		ControlEventClicked = 10,
	};

	virtual bool OnRender(void);
	virtual bool OnEventMouse(Event event,WPARAM wparam = NULL,LPARAM lparam = NULL);
	virtual bool OnEventKeyboard(Event event,WPARAM wparam = NULL,LPARAM lparam = NULL);

	virtual void SetText(const String& text) { mText = text; }
	virtual const String& GetText(void) const { return mText; }

	virtual void SetPressed(bool pressed) { mPressed = pressed; }
	virtual bool GetPressed(void) const { return mPressed; }

protected:
	String	mText;

	bool	mPressed;
};
