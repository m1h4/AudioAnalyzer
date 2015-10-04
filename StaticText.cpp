#include "Globals.h"
#include "Kernel.h"
#include "StaticText.h"

StaticText::StaticText(void)
{
}

StaticText::~StaticText(void)
{
}

bool StaticText::OnRender(void)
{
	if(!GetVisible())
		return false;

	Rect box = GetBoundingBox();

	//GetKernel()->GetUserInterface()->DrawBorder(box);
	GetKernel()->GetUserInterface()->DrawStaticText(box,mText,false,GetEnabled(),GetFocused());

	return Control::OnRender();
}
