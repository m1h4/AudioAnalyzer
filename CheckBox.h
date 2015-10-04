#pragma once

#include "Button.h"

class CheckBox;

typedef CheckBox* CheckBoxHandle;

class CheckBox : public Button
{
public:
	CheckBox(void);
	~CheckBox(void);

	enum ControlEvent
	{
		ControlEventCheckChange = 20,
	};

	virtual bool OnRender(void);

	virtual void SetPressed(bool pressed);

	virtual void SetChecked(bool checked) { mChecked = checked; }
	virtual bool GetChecked(void) const { return mChecked; }

protected:
	bool	mChecked;
};
