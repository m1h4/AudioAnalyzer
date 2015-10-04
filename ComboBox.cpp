#include "Globals.h"
#include "Kernel.h"
#include "ComboBox.h"

ComboBox::ComboBox(void) :
mSelectedItem(NULL),
mFocusedItem(NULL),
mOpen(false)
{
}

ComboBox::~ComboBox(void)
{
	if(mItems.GetSize())
		mItems.Delete();
}

bool ComboBox::OnRender(void)
{
	if(!GetVisible())
		return false;

	Rect box = GetBoundingBox();

	GetKernel()->GetUserInterface()->DrawComboBox(box,false,GetEnabled(),GetFocused());

	box.Inflate(-2,-2);

	Rect box2(box.right - box.GetHeight() + 1,box.top,box.right,box.bottom);
	box.right -= box.GetHeight() - 1;

	GetKernel()->GetUserInterface()->DrawButton(box2,GetPressed(),GetEnabled(),false);
	GetKernel()->GetUserInterface()->DrawArrowDownSymbol(box2,GetPressed(),GetEnabled(),GetFocused());

	box.Inflate(-1,-1);

	if(GetFocused() && GetEnabled() && !GetOpen())
		GetKernel()->GetGraphics()->DrawFillRect(box,GetKernel()->GetUserInterface()->GetSelectionColor());

	if(GetItemCount())
	{
		box.left += 2;
		GetKernel()->GetUserInterface()->DrawComboBoxText(box,GetItem(GetSelectedItem()),false,GetEnabled(),GetFocused() && !GetOpen());
	}

	if(GetOpen())
	{
		unsigned long height = GetKernel()->GetGraphics()->GetFontHeight(GetKernel()->GetGraphics()->GetUIFont());
		Rect menu(GetScreenPosition() + Point(0,GetSize().cy),Size(GetSize().cx,height * mItems.GetSize() + 2));

		GetKernel()->GetGraphics()->DrawRect(menu,GetKernel()->GetUserInterface()->GetBorderColor());

		menu.Inflate(-1,-1);

		GetKernel()->GetGraphics()->DrawFillRect(menu,GetKernel()->GetUserInterface()->GetClientColor());

		for(unsigned long i = 0; i < mItems.GetSize(); ++i)
		{
			if(i == mFocusedItem)
			{
				GetKernel()->GetGraphics()->DrawFillRect(Rect(menu.left,menu.top + height * i,menu.right,menu.top + height * i + height),GetKernel()->GetUserInterface()->GetSelectionColor());
				GetKernel()->GetGraphics()->DrawText(GetKernel()->GetGraphics()->GetUIFont(),Point(menu.left + 2,menu.top + height * i),GetItem(i),Color(1.0f,1.0f,1.0f));
			}
			else
				GetKernel()->GetGraphics()->DrawText(GetKernel()->GetGraphics()->GetUIFont(),Point(menu.left + 2,menu.top + height * i),GetItem(i),GetKernel()->GetUserInterface()->GetTextColor());
		}
	}

	return Control::OnRender();
}

bool ComboBox::OnEventMouse(Event event,WPARAM wparam,LPARAM lparam)
{
	if(!GetVisible() || !GetEnabled())
		return false;

	switch(event)
	{
	case EventLeftButtonDown:
		if(GetOpen())
		{
			unsigned long height = GetKernel()->GetGraphics()->GetFontHeight(GetKernel()->GetGraphics()->GetUIFont());
			Rect menu(GetScreenPosition() + Point(0,GetSize().cy),Size(GetSize().cx,height * mItems.GetSize() + 2));
			if(!menu.PointIn(*(Point*)wparam))
			{
				SetOpen(false);
				break;
			}
			
			menu.Inflate(-1,-1);

			if(menu.PointIn(*(Point*)wparam))
			{
				if(GetFocusedItem() != GetSelectedItem())
				{
					SetSelectedItem(GetFocusedItem());
					if(GetParent())
						GetParent()->OnEvent(EventChildCommand,ControlEventSelectionChange,(LPARAM)this);
				}
			}

			SetOpen(false);

			return true;
		}

		break;

	case EventMouseMove:
		if(GetOpen())
		{
			unsigned long height = GetKernel()->GetGraphics()->GetFontHeight(GetKernel()->GetGraphics()->GetUIFont());
			Rect menu(GetScreenPosition() + Point(0,GetSize().cy),Size(GetSize().cx,height * mItems.GetSize() + 2));
			if(!menu.PointIn(*(Point*)wparam))
				break;
			
			menu.Inflate(-1,-1);

			for(unsigned long i = 0; i < mItems.GetSize(); ++i)
			{
				Rect rect(menu.left,menu.top + height * i,menu.right,menu.top + height * i + height);

				if(rect.PointIn(*(Point*)wparam))
				{
					SetFocusedItem(i);
					break;
				}
			}

			return true;
		}

		break;
	}

	return Button::OnEventMouse(event,wparam,lparam);
}

bool ComboBox::OnEventKeyboard(Event event,WPARAM wparam,LPARAM lparam)
{
	if(!GetVisible() || !GetEnabled())
		return false;

	switch(event)
	{
	case EventKeyDown:
		if(GetOpen())
		{
			if(wparam == VK_UP)
			{
				if(GetFocusedItem())
					SetFocusedItem(GetFocusedItem() - 1);

				return true;
			}

			if(wparam == VK_DOWN)
			{
				if(GetFocusedItem() < GetItemCount() - 1)
					SetFocusedItem(GetFocusedItem() + 1);

				return true;
			}

			if(wparam == VK_HOME)
			{
				SetFocusedItem(0);

				return true;
			}

			if(wparam == VK_END)
			{
				SetFocusedItem(GetItemCount() - 1);

				return true;
			}

			if(wparam == VK_RETURN)
			{
				if(GetFocusedItem() != GetSelectedItem())
				{
					SetSelectedItem(GetFocusedItem());
					if(GetParent())
						GetParent()->OnEvent(EventChildCommand,ControlEventSelectionChange,(LPARAM)this);
				}

				SetOpen(false);

				return true;
			}
		}
		else
		{
			if(wparam == VK_UP)
			{
				if(GetSelectedItem())
				{
					SetSelectedItem(GetSelectedItem() - 1);

					if(GetParent())
						GetParent()->OnEvent(EventChildCommand,ControlEventSelectionChange,(LPARAM)this);
				}

				return true;
			}

			if(wparam == VK_DOWN)
			{
				if(GetSelectedItem() < GetItemCount() - 1)
				{
					SetSelectedItem(GetSelectedItem() + 1);

					if(GetParent())
						GetParent()->OnEvent(EventChildCommand,ControlEventSelectionChange,(LPARAM)this);
				}

				return true;
			}

			if(wparam == VK_HOME)
			{
				SetSelectedItem(0);

				if(GetParent())
					GetParent()->OnEvent(EventChildCommand,ControlEventSelectionChange,(LPARAM)this);

				return true;
			}

			if(wparam == VK_END)
			{
				SetSelectedItem(GetItemCount() - 1);

				if(GetParent())
					GetParent()->OnEvent(EventChildCommand,ControlEventSelectionChange,(LPARAM)this);

				return true;
			}
		}

		break;
	}

	return Button::OnEventKeyboard(event,wparam,lparam);
}