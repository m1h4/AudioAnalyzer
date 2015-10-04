#pragma once

#include "Control.h"

class StaticText;

typedef StaticText* StaticTextHandle;

class StaticText : public Control
{
public:
	StaticText(void);
	~StaticText(void);

	virtual bool OnRender(void);

	virtual bool PointIn(const Point& point) { UNREFERENCED_PARAMETER(point); return false; }

	virtual void SetText(const String& text) { mText = text; }
	virtual const String& GetText(void) const { return mText; }

protected:
	String	mText;
};
