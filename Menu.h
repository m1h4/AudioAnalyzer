// Copyright 2004/2006 Marko Mihovilic

#pragma once

#include "MenuItem.h"

typedef bool (CALLBACK *MenuEventHandler) (unsigned long event,MenuItem* sender);

#define EVENT_PRESSED 0x00000001

class Menu
{
public:
	Menu() : mOpen(false), mSelected(0), mSelectedPrevious(0), mEventHandler(NULL),mOpenTime(NULL),mSelectedTime(NULL) {}
	~Menu() { if(mItems.GetSize()) mItems.Delete(); }

	void AddItem(MenuItem* item) { mItems.InsertBack(item); }
	unsigned long GetItemCount() const { return mItems.GetSize(); }
	MenuItem* GetItem(unsigned long index) { if(index >= mItems.GetSize()) return NULL; return mItems[index]; }
	MenuItem* GetItemId(unsigned long id) { for(unsigned long i = 0; i < mItems.GetSize(); i++) if(mItems[i]->GetId() == id) return mItems[i]; return NULL; }

	void SetSelected(unsigned long index);
	void SetSelectedFirstValid() { for(unsigned long i = 0; i < mItems.GetSize(); ++i) { if(!mItems[i]->GetStatic()) { SetSelected(i); return; } } }
	unsigned long GetSelected() const { return mSelected; } 
	unsigned long GetSelectedPrevious() const { return mSelectedPrevious; }
	double GetSelectedTime() const { return mSelectedTime; }

	bool KeyEvent(unsigned long key);
	bool MouseEvent(unsigned long event,const Point& delta);	// Event: 0-Mouse Move,1-Left Button,2-Right Button
	bool ControlEvent(unsigned long event,unsigned long sender);

	void Render();

	void SetEventHandler(MenuEventHandler handler) { mEventHandler = handler; }

	void SetOpen(bool open);
	bool GetOpen() const { return mOpen; }
	double GetOpenTime() const { return mOpenTime; }

protected:
	Array<MenuItem*>	mItems;

	unsigned long		mSelected;
	unsigned long		mSelectedPrevious;
	double				mSelectedTime;

	MenuEventHandler	mEventHandler;

	bool				mOpen;
	double				mOpenTime;
};