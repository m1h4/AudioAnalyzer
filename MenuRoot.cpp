// Copyright 2004/2006 Marko Mihovilic

#include "Globals.h"
#include "Kernel.h"
#include "MenuRoot.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void MenuRoot::RenderMenus()
{
	// Render all menus, they wont render themselves if they are closed
	for(unsigned long i = 0; i < mMenus.GetSize(); ++i)
		mMenus[i]->Render();
}

bool MenuRoot::KeyEvent(unsigned long key)
{
	if(!GetCurrentMenu()->GetOpen())
		return false;

	// Send the event to the current menu
	return mMenus[mCurrent]->KeyEvent(key);
}

bool MenuRoot::MouseEvent(unsigned long event,const Point& delta)
{
	if(!GetCurrentMenu()->GetOpen())
		return false;

	// Send the event to the current menu
	return mMenus[mCurrent]->MouseEvent(event,delta);
}