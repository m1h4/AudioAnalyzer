// Copyright 2004/2006 Marko Mihovilic

#pragma once

#include "Menu.h"

class MenuRoot
{
public:
	MenuRoot() : mCurrent(0) {}
	~MenuRoot() { if(mMenus.GetSize()) mMenus.Delete(); }

	void RenderMenus();

	bool KeyEvent(unsigned long key);
	bool MouseEvent(unsigned long event,const Point& delta);

	void SetCurrentMenu(unsigned long current) { unsigned long old = mCurrent; mCurrent = current % mMenus.GetSize(); if(old != mCurrent) mMenus[mCurrent]->SetOpen(mMenus[old]->GetOpen()); mMenus[old]->SetOpen(!mMenus[old]->GetOpen()); }
	unsigned long GetCurrentMenuIndex() const { return mCurrent; }
	Menu* GetCurrentMenu() { if(mCurrent >= mMenus.GetSize()) return NULL; return mMenus[mCurrent]; }

	void AddMenu(Menu* menu) { mMenus.InsertBack(menu); }
	unsigned long GetMenuCount() const { return mMenus.GetSize(); }
	Menu* GetMenu(unsigned long index) { if(index >= mMenus.GetSize()) return NULL; return mMenus[index]; }

protected:
	Array<Menu*>		mMenus;
	unsigned long		mCurrent;
};
