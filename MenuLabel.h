// Copyright 2004/2006 Marko Mihovilic

#pragma once

#include "Menuitem.h"

typedef const wchar_t* (CALLBACK *MenuItemTextHandler) (MenuItem* sender);

class MenuLabel : public MenuItem
{
public:
	MenuLabel() : mTextHandler(NULL) {}
	MenuLabel(Type type,const wchar_t* text,unsigned long id,bool stati,MenuItemTextHandler handler) { SetType(type); if(text) SetText(text); if(id) SetId(id); else SetId(0); SetStatic(stati); if(handler) SetTextHandler(handler); else SetTextHandler(NULL); }

	void SetTextHandler(MenuItemTextHandler handler) { mTextHandler = handler; }
	MenuItemTextHandler GetTextHandler() const { return mTextHandler; }

	// Return the get text handler if one was set, else the text string
	const wchar_t* GetText() { if(mTextHandler) return mTextHandler(this); return mText; }
	void SetText(const wchar_t* text) { mText = text; }

protected:
	String					mText;
	MenuItemTextHandler		mTextHandler;
};
