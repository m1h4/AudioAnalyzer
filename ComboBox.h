#pragma once

#include "Button.h"

class ComboBox;

typedef ComboBox* ComboBoxHandle;

class ComboBox : public Button
{
public:
	ComboBox(void);
	~ComboBox(void);

	enum ControlEvent
	{
		ControlEventSelectionChange = 30,
	};

	virtual bool OnRender(void);
	virtual bool OnEventMouse(Event event,WPARAM wparam = NULL,LPARAM lparam = NULL);
	virtual bool OnEventKeyboard(Event event,WPARAM wparam = NULL,LPARAM lparam = NULL);

	virtual void AddItem(const wchar_t* item) { mItems.InsertBack(new String(item)); }
	virtual void RemoveItem(unsigned long index) { if(index >= mItems.GetSize()) return; mItems.Delete(index); }
	virtual const String& GetItem(unsigned long index) const { return *mItems[index]; }	// TODO Check for out-of-bounds
	virtual unsigned long GetItemCount(void) const { return mItems.GetSize(); }

	virtual void SetPressed(bool pressed) { if(pressed && !GetPressed()) SetOpen(true); Button::SetPressed(pressed); }

	virtual void SetSelectedItem(unsigned long index) { if(index >= mItems.GetSize()) return; mSelectedItem = index; }
	virtual unsigned long GetSelectedItem(void) const { return mSelectedItem; }

	virtual void SetOpen(bool open) { mOpen = open; SetFocusedItem(GetSelectedItem()); }
	virtual bool GetOpen(void) const { return mOpen; }

	virtual void SetFocusedItem(unsigned long focused) { mFocusedItem = focused; }
	virtual unsigned long GetFocusedItem(void) const { return mFocusedItem; }

protected:
	Array<String*>	mItems;

	bool	mOpen;

	unsigned long	mSelectedItem;
	unsigned long	mFocusedItem;
};
