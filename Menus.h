#pragma once

#include "MenuRoot.h"

class Menus
{
public:
	Menus(void);
	~Menus(void);

	bool CreateMenus(void);
	bool GetCreated(void) const { return mCreated; }
	void DestroyMenus(void);

	static bool CALLBACK MenuEventHandler(unsigned long event,MenuItem* sender);
	static bool CALLBACK MenuVisEventHandler(unsigned long event,MenuItem* sender);
	static bool CALLBACK MenuCaptureEventHandler(unsigned long event,MenuItem* sender);
	static bool CALLBACK MenuPresetEventHandler(unsigned long event,MenuItem* sender);
	static bool CALLBACK MenuInfoEventHandler(unsigned long event,MenuItem* sender);
	static bool CALLBACK MenuExitEventHandler(unsigned long event,MenuItem* sender);
	static bool CALLBACK MenuOptionsEventHandler(unsigned long event,MenuItem* sender);

	static const wchar_t* CALLBACK MenuItemTextHandler(MenuItem* sender);
	static const wchar_t* CALLBACK MenuInfoItemTextHandler(MenuItem* sender);

	MenuRoot* GetMenuRoot(void) { return &mMenuRoot; }

protected:
	bool		mCreated;

	MenuRoot	mMenuRoot;
};