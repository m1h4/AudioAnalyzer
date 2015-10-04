// Copyright 2004/2006 Marko Mihovilic

#include "Globals.h"
#include "Kernel.h"
#include "MenuLabel.h"
#include "Menus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Menus::Menus(void)
{
}

Menus::~Menus(void)
{
}

bool Menus::CreateMenus()
{
	// Build the menus
	Menu* menuMain = new Menu;
	ASSERT(menuMain);

	menuMain->AddItem(new MenuLabel(MenuItem::Title,L"main menu",0,true,NULL));
	menuMain->AddItem(new MenuLabel(MenuItem::Margin,NULL,0,true,NULL));
	menuMain->AddItem(new MenuLabel(MenuItem::Label,L"visualizations",1,false,NULL));
	menuMain->AddItem(new MenuLabel(MenuItem::Label,L"color presets",2,false,NULL));
	menuMain->AddItem(new MenuLabel(MenuItem::Seperator,NULL,0,true,NULL));
	menuMain->AddItem(new MenuLabel(MenuItem::Label,L"options",3,false,NULL));
	menuMain->AddItem(new MenuLabel(MenuItem::Label,L"capture source",4,false,NULL));
	menuMain->AddItem(new MenuLabel(MenuItem::Seperator,NULL,0,true,NULL));
	menuMain->AddItem(new MenuLabel(MenuItem::Label,L"info",5,false,NULL));
	menuMain->AddItem(new MenuLabel(MenuItem::Seperator,NULL,0,true,NULL));
	menuMain->AddItem(new MenuLabel(MenuItem::Label,L"close",6,false,NULL));

	menuMain->SetSelectedFirstValid();
	menuMain->SetEventHandler(MenuEventHandler);

	mMenuRoot.AddMenu(menuMain);

	Menu* menuExit = new Menu;
	ASSERT(menuExit);

	menuExit->AddItem(new MenuLabel(MenuItem::Title,L"close",0,true,NULL));
	menuExit->AddItem(new MenuLabel(MenuItem::Margin,NULL,0,true,NULL));
	menuExit->AddItem(new MenuLabel(MenuItem::Label,L"yes",1,false,NULL));
	menuExit->AddItem(new MenuLabel(MenuItem::Label,L"no",2,false,NULL));

	menuExit->SetSelected(3);	// No
	menuExit->SetEventHandler(MenuExitEventHandler);

	mMenuRoot.AddMenu(menuExit);

	Menu* menuCapture = new Menu;
	ASSERT(menuCapture);

	menuCapture->AddItem(new MenuLabel(MenuItem::Title,L"capture source menu",0,true,NULL));
	menuCapture->AddItem(new MenuLabel(MenuItem::Margin,NULL,0,true,NULL));

	// Add the capture sources
	for(unsigned long i = 0; i < GetKernel()->GetAudio()->GetSourcesCount(); ++i)
		menuCapture->AddItem(new MenuLabel(MenuItem::Label,GetKernel()->GetAudio()->GetSource(i),i+1,false,NULL));

	if(!GetKernel()->GetAudio()->GetSourcesCount())
		menuCapture->AddItem(new MenuLabel(MenuItem::Label,L"no capture sources",1,true,NULL));

	menuCapture->AddItem(new MenuLabel(MenuItem::Seperator,NULL,0,true,NULL));
	menuCapture->AddItem(new MenuLabel(MenuItem::Label,L"back",GetKernel()->GetAudio()->GetSourcesCount()+1,false,NULL));

	menuCapture->SetSelectedFirstValid();
	menuCapture->SetEventHandler(MenuCaptureEventHandler);

	mMenuRoot.AddMenu(menuCapture);

	Menu* menuOptions = new Menu;
	ASSERT(menuOptions);

	menuOptions->AddItem(new MenuLabel(MenuItem::Title,L"options menu",0,true,NULL));
	menuOptions->AddItem(new MenuLabel(MenuItem::Margin,NULL,0,true,NULL));
	menuOptions->AddItem(new MenuLabel(MenuItem::Label,NULL,1,false,MenuItemTextHandler));
	menuOptions->AddItem(new MenuLabel(MenuItem::Label,NULL,2,false,MenuItemTextHandler));
	menuOptions->AddItem(new MenuLabel(MenuItem::Label,NULL,3,false,MenuItemTextHandler));
	menuOptions->AddItem(new MenuLabel(MenuItem::Label,NULL,4,false,MenuItemTextHandler));
	menuOptions->AddItem(new MenuLabel(MenuItem::Label,NULL,5,false,MenuItemTextHandler));
	menuOptions->AddItem(new MenuLabel(MenuItem::Label,NULL,6,false,MenuItemTextHandler));
	menuOptions->AddItem(new MenuLabel(MenuItem::Label,NULL,7,false,MenuItemTextHandler));
	menuOptions->AddItem(new MenuLabel(MenuItem::Seperator,NULL,0,true,NULL));
	menuOptions->AddItem(new MenuLabel(MenuItem::Label,NULL,8,false,MenuItemTextHandler));
	menuOptions->AddItem(new MenuLabel(MenuItem::Seperator,NULL,0,true,NULL));
	menuOptions->AddItem(new MenuLabel(MenuItem::Label,NULL,9,false,MenuItemTextHandler));
	menuOptions->AddItem(new MenuLabel(MenuItem::Label,NULL,10,false,MenuItemTextHandler));
	menuOptions->AddItem(new MenuLabel(MenuItem::Label,NULL,11,false,MenuItemTextHandler));
	menuOptions->AddItem(new MenuLabel(MenuItem::Seperator,NULL,0,true,NULL));
	menuOptions->AddItem(new MenuLabel(MenuItem::Label,L"write config",12,false,NULL));
	menuOptions->AddItem(new MenuLabel(MenuItem::Label,L"read config",13,false,NULL));
	menuOptions->AddItem(new MenuLabel(MenuItem::Seperator,NULL,0,true,NULL));
	menuOptions->AddItem(new MenuLabel(MenuItem::Label,L"back",14,false,NULL));

	menuOptions->SetSelectedFirstValid();
	menuOptions->SetEventHandler(MenuOptionsEventHandler);

	mMenuRoot.AddMenu(menuOptions);

	Menu* menuInfo = new Menu;
	ASSERT(menuInfo);

	menuInfo->AddItem(new MenuLabel(MenuItem::Title,L"info menu",0,true,NULL));
	menuInfo->AddItem(new MenuLabel(MenuItem::Margin,NULL,0,true,NULL));
	menuInfo->AddItem(new MenuLabel(MenuItem::Label,NULL,1,true,MenuInfoItemTextHandler));
	menuInfo->AddItem(new MenuLabel(MenuItem::Label,NULL,2,true,MenuInfoItemTextHandler));
	menuInfo->AddItem(new MenuLabel(MenuItem::Margin,NULL,0,true,NULL));
	menuInfo->AddItem(new MenuLabel(MenuItem::Label,L"audio analyzer v\002" TOSTRINGW(ANALYZER_VERSION_MAJOR) L"." TOSTRINGW(ANALYZER_VERSION_MINOR) L"\001",0,true,NULL));
	menuInfo->AddItem(new MenuLabel(MenuItem::Label,L"marko mihovilic",0,true,NULL));
	menuInfo->AddItem(new MenuLabel(MenuItem::Margin,NULL,0,true,NULL));
	menuInfo->AddItem(new MenuLabel(MenuItem::Label,L"back",6,false,NULL));

	menuInfo->SetSelectedFirstValid();
	menuInfo->SetEventHandler(MenuInfoEventHandler);

	mMenuRoot.AddMenu(menuInfo);

	Menu* menuVis = new Menu;
	ASSERT(menuVis);

	menuVis->AddItem(new MenuLabel(MenuItem::Title,L"visualizations menu",0,true,NULL));
	menuVis->AddItem(new MenuLabel(MenuItem::Margin,NULL,0,true,NULL));

	// Add the visualizations
	for(unsigned long i = 0; i < GetKernel()->GetGraphics()->GetVisualizationCount(); ++i)
		menuVis->AddItem(new MenuLabel(MenuItem::Label,GetKernel()->GetGraphics()->GetVisualizationName(i),i+1,false,NULL));

	menuVis->AddItem(new MenuLabel(MenuItem::Seperator,NULL,0,true,NULL));
	menuVis->AddItem(new MenuLabel(MenuItem::Label,L"back",GetKernel()->GetGraphics()->GetVisualizationCount()+1,false,NULL));

	menuVis->SetSelectedFirstValid();
	menuVis->SetEventHandler(MenuVisEventHandler);

	mMenuRoot.AddMenu(menuVis);

	Menu* menuPreset = new Menu;
	ASSERT(menuPreset);

	menuPreset->AddItem(new MenuLabel(MenuItem::Title,L"color presets menu",0,true,NULL));
	menuPreset->AddItem(new MenuLabel(MenuItem::Margin,NULL,0,true,NULL));

	// Add the color presets
	for(unsigned long i = 0; i < GetKernel()->GetGraphics()->GetColorPresetCount(); ++i)
		menuPreset->AddItem(new MenuLabel(MenuItem::Label,GetKernel()->GetGraphics()->GetColorPresetName(i),i+1,false,NULL));

	menuPreset->AddItem(new MenuLabel(MenuItem::Seperator,NULL,0,true,NULL));
	menuPreset->AddItem(new MenuLabel(MenuItem::Label,L"back",GetKernel()->GetGraphics()->GetColorPresetCount()+1,false,NULL));

	menuPreset->SetSelectedFirstValid();
	menuPreset->SetEventHandler(MenuPresetEventHandler);

	mMenuRoot.AddMenu(menuPreset);

	return true;
}

void Menus::DestroyMenus()
{
}

bool Menus::MenuPresetEventHandler(unsigned long event,MenuItem* sender)
{
	UNREFERENCED_PARAMETER(event);

	// Back
	if(sender->GetId() == GetKernel()->GetGraphics()->GetColorPresetCount()+1)
	{
		GetKernel()->GetMenus()->GetMenuRoot()->SetCurrentMenu(0); // Main menu
		return true;
	}

	// Set the selected preset
	if(sender->GetId())
		return GetKernel()->GetGraphics()->SetColorPreset(sender->GetId()-1);

	return false;
}

bool Menus::MenuVisEventHandler(unsigned long event,MenuItem* sender)
{
	UNREFERENCED_PARAMETER(event);

	// Back
	if(sender->GetId() == GetKernel()->GetGraphics()->GetVisualizationCount()+1)
	{
		GetKernel()->GetMenus()->GetMenuRoot()->SetCurrentMenu(0); // Main menu
		return true;
	}

	// Set the selected vis
	if(sender->GetId())
		return GetKernel()->GetGraphics()->SetVisualization(sender->GetId()-1);

	return false;
}

bool Menus::MenuCaptureEventHandler(unsigned long event,MenuItem* sender)
{
	UNREFERENCED_PARAMETER(event);

	// Back
	if(sender->GetId() == GetKernel()->GetAudio()->GetSourcesCount()+1)
	{
		GetKernel()->GetMenus()->GetMenuRoot()->SetCurrentMenu(0); // Main menu
		return true;
	}

	// Set the selected vis
	if(sender->GetId())
		return GetKernel()->GetAudio()->SetSource(sender->GetId()-1);

	return false;
}

bool Menus::MenuEventHandler(unsigned long event,MenuItem* sender)
{
	UNREFERENCED_PARAMETER(event);

	switch(sender->GetId())
	{
	case 1: // Visualizations menu
		GetKernel()->GetMenus()->GetMenuRoot()->SetCurrentMenu(5);	// Vis menu
		break;

	case 2: // Color presets menu
		GetKernel()->GetMenus()->GetMenuRoot()->SetCurrentMenu(6);	// Presets menu
		break;

	case 3: // Options
		GetKernel()->GetMenus()->GetMenuRoot()->SetCurrentMenu(3);	// Options menu
		return true;

	case 4: // Capture Source
		GetKernel()->GetMenus()->GetMenuRoot()->SetCurrentMenu(2);	// Capture Source menu
		return true;

	case 5: // Info
		GetKernel()->GetMenus()->GetMenuRoot()->SetCurrentMenu(4);	// Info menu
		return true;

	case 6: // Close
		//PostQuitMessage(0);
		GetKernel()->GetMenus()->GetMenuRoot()->SetCurrentMenu(1);	// Exit menu
		return true;
	}

	return false;
}

bool Menus::MenuOptionsEventHandler(unsigned long event,MenuItem* sender)
{
	UNREFERENCED_PARAMETER(event);

	switch(sender->GetId())
	{
	case 1:	// Draw peaks
		GetKernel()->GetGraphics()->SetDrawPeaks(!GetKernel()->GetGraphics()->GetDrawPeaks());
		return true;

	case 2:	// Draw help text
		GetKernel()->GetGraphics()->SetDrawHelp(!GetKernel()->GetGraphics()->GetDrawHelp());
		return true;

	case 3:	// Draw about text
		GetKernel()->GetGraphics()->SetDrawAbout(!GetKernel()->GetGraphics()->GetDrawAbout());
		return true;

	case 4: // Draw output strings
		GetKernel()->GetGraphics()->SetDrawOutputStrings(!GetKernel()->GetGraphics()->GetDrawOutputStrings());
		return true;

	case 5:	// Draw fps
		GetKernel()->GetGraphics()->SetDrawFps(!GetKernel()->GetGraphics()->GetDrawFps());
		return true;

	case 6:	// Draw visualization name
		GetKernel()->GetGraphics()->SetDrawVisualizationName(!GetKernel()->GetGraphics()->GetDrawVisualizationName());
		return true;

	case 7: // Draw timer
		GetKernel()->GetGraphics()->SetDrawTimer(!GetKernel()->GetGraphics()->GetDrawTimer());
		return true;

	case 8:	// Keyboard LED
		GetKernel()->SetLed(!GetKernel()->GetLed());
		return true;

	case 9: // Eq Falloff
		if(GetKernel()->GetAudio()->GetEqFalloff() == 0)
			GetKernel()->GetAudio()->SetEqFalloff(1);
		else if(GetKernel()->GetAudio()->GetEqFalloff() == 1)
			GetKernel()->GetAudio()->SetEqFalloff(-1);
		else if(GetKernel()->GetAudio()->GetEqFalloff() == -1)
			GetKernel()->GetAudio()->SetEqFalloff(0);
		return true;

	case 10: // Peak Falloff
		if(GetKernel()->GetAudio()->GetPeakFalloff() == 0)
			GetKernel()->GetAudio()->SetPeakFalloff(1);
		else if(GetKernel()->GetAudio()->GetPeakFalloff() == 1)
			GetKernel()->GetAudio()->SetPeakFalloff(-1);
		else if(GetKernel()->GetAudio()->GetPeakFalloff() == -1)
			GetKernel()->GetAudio()->SetPeakFalloff(0);
		return true;

	case 11: // Peak Delay
		if(GetKernel()->GetAudio()->GetPeakDelay() == 0)
			GetKernel()->GetAudio()->SetPeakDelay(1);
		else if(GetKernel()->GetAudio()->GetPeakDelay() == 1)
			GetKernel()->GetAudio()->SetPeakDelay(-1);
		else if(GetKernel()->GetAudio()->GetPeakDelay() == -1)
			GetKernel()->GetAudio()->SetPeakDelay(0);
		return true;

	case 12: // Write config
		GetKernel()->WriteConfig(NULL);
		return true;

	case 13: // Read config
		GetKernel()->ReadConfig(NULL);
		return true;

	case 14: // Back
		GetKernel()->GetMenus()->GetMenuRoot()->SetCurrentMenu(0);
		return true;
	}

	return false;
}

bool Menus::MenuExitEventHandler(unsigned long event,MenuItem* sender)
{
	UNREFERENCED_PARAMETER(event);

	switch(sender->GetId())
	{
	case 1: // Yes
		PostQuitMessage(0);
		return true;
		
	case 2: // No
		GetKernel()->GetMenus()->GetMenuRoot()->SetCurrentMenu(0);	// Main menu
		return true;
	}

	return false;
}

const wchar_t* Menus::MenuInfoItemTextHandler(MenuItem* sender)
{
	static String string;

	switch(sender->GetId())
	{
	case 1: // Current vis
		string = GetKernel()->GetGraphics()->GetVisualizationName(GetKernel()->GetGraphics()->GetVisualizationIndex());
		string.ToLower();
		return string;

	case 2: // Curent color preset
		string = GetKernel()->GetGraphics()->GetColorPresetName(GetKernel()->GetGraphics()->GetColorPresetIndex());
		string.ToLower();
		return string;
	}

	return false;
}

bool Menus::MenuInfoEventHandler(unsigned long event,MenuItem* sender)
{
	UNREFERENCED_PARAMETER(event);

	switch(sender->GetId())
	{
	case 6: // Back
		GetKernel()->GetMenus()->GetMenuRoot()->SetCurrentMenu(0); // Main menu
		return true;
	}

	return false;
}

const wchar_t* Menus::MenuItemTextHandler(MenuItem* sender)
{
	switch(sender->GetId())
	{
	case 1: // Draw Peaks
		if(GetKernel()->GetGraphics()->GetDrawPeaks())
			return L"draw peaks: \002on\001";
		else
			return L"draw peaks: \002off\001";

	case 2: // Draw help text
		if(GetKernel()->GetGraphics()->GetDrawHelp())
			return L"draw help text: \002on\001";
		else
			return L"draw help text: \002off\001";

	case 3: // Draw about text
		if(GetKernel()->GetGraphics()->GetDrawAbout())
			return L"draw about text: \002on\001";
		else
			return L"draw about text: \002off\001";

	case 4: // Draw output strings
		if(GetKernel()->GetGraphics()->GetDrawOutputStrings())
			return L"draw output: \002on\001";
		else
			return L"draw output: \002off\001";

	case 5: // Draw fps
		if(GetKernel()->GetGraphics()->GetDrawFps())
			return L"draw fps: \002on\001";
		else
			return L"draw fps: \002off\001";

	case 6: // Draw visualization name
		if(GetKernel()->GetGraphics()->GetDrawVisualizationName())
			return L"draw visualization name: \002on\001";
		else
			return L"draw visualization name: \002off\001";

	case 7: // Draw timer
		if(GetKernel()->GetGraphics()->GetDrawTimer())
			return L"draw timer: \002on\001";
		else
			return L"draw timer: \002off\001";

	case 8: // Keyboard LED
		if(GetKernel()->GetLed())
			return L"keyboard LED: \002on\001";
		else
			return L"keyboard LED: \002off\001";

	case 9: // Eq Falloff
		if(GetKernel()->GetAudio()->GetEqFalloff() == -1)
			return L"eq falloff: \002slow\001";
		else if(GetKernel()->GetAudio()->GetEqFalloff() == 0)
			return L"eq falloff: \002normal\001";
		else if(GetKernel()->GetAudio()->GetEqFalloff() == 1)
			return L"eq falloff: \002fast\001";
		else
			return L"eq falloff: \002unknown\001";

	case 10: // Peak fallof
		if(GetKernel()->GetAudio()->GetPeakFalloff() == -1)
			return L"peak falloff: \002slow\001";
		else if(GetKernel()->GetAudio()->GetPeakFalloff() == 0)
			return L"peak falloff: \002normal\001";
		else if(GetKernel()->GetAudio()->GetPeakFalloff() == 1)
			return L"peak falloff: \002fast\001";
		else
			return L"peak falloff: \002unknown\001";

	case 11: // Peak delay
		if(GetKernel()->GetAudio()->GetPeakDelay() == -1)
			return L"peak delay: \002low\001";
		else if(GetKernel()->GetAudio()->GetPeakDelay() == 0)
			return L"peak delay: \002normal\001";
		else if(GetKernel()->GetAudio()->GetPeakDelay() == 1)
			return L"peak delay: \002high\001";
		else
			return L"peak delay: \002unknown\001";
	}

	return NULL;
}