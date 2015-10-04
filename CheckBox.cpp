#include "Globals.h"
#include "Kernel.h"
#include "CheckBox.h"

CheckBox::CheckBox(void) :
mChecked(false)
{
}

CheckBox::~CheckBox(void)
{
}

bool CheckBox::OnRender(void)
{
	if(!GetVisible())
		return false;

	Point position = GetScreenPosition();
	Rect box = Rect(Point(position.x,position.y + mSize.cy / 2 - 13 / 2),Size(13,13));

	GetKernel()->GetUserInterface()->DrawComboBox(box,GetPressed(),GetEnabled(),GetFocused());

	if(GetChecked())
		GetKernel()->GetUserInterface()->DrawCheckSymbol(box,false,GetEnabled(),GetFocused());

	box.left += 13 + 4;

	GetKernel()->GetUserInterface()->DrawCheckBoxText(box,mText,false,GetEnabled(),GetFocused());

	return Control::OnRender();
}

void CheckBox::SetPressed(bool pressed)
{
	if(!pressed && GetPressed())
	{
		SetChecked(!GetChecked());

		if(GetParent())
			GetParent()->OnEvent(EventChildCommand,ControlEventCheckChange,(LPARAM)this);
	}
	
	return Button::SetPressed(pressed);
}