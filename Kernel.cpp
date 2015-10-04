// Copyright 2004/2006 Marko Mihovilic

#include "Globals.h"
#include "PowerSpectrum.h"
#include "AnalyzerRedesigned.h"
#include "LazySpectrum.h"
#include "Scope.h"
#include "DoubleScope.h"
#include "AmoebaScope.h"
#include "VuMeter.h"
#include "Kernel.h"
#include "TransitionEffects.h"
#include "Configuration.h"

#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Our main entry point
INT WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, INT /*nCmdShow*/)
{
	// Create a system wide mutex
	HANDLE mutex = CreateMutex(NULL,FALSE,ANALYZER_MUTEXSTRING);
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		//MessageBox(GetDesktopWindow(),L"A instance of " ANALYZER_WINDOWTITLE L" is already running.\n\nPlease close it before running again.",L"Already running",MB_OK|MB_ICONEXCLAMATION);
		
		HWND hWnd = FindWindow(ANALYZER_CLASSNAME,ANALYZER_WINDOWTITLE);
		ShowWindow(hWnd,SW_RESTORE);
		SetForegroundWindow(hWnd);
		
		return 0;
	}

	// Initialize com
	CoInitialize(NULL);

	// Randomize the random seed
	srand((unsigned long)GetCurrentTime());

	// Increase system timer resolution
	timeBeginPeriod(1);

	//InitCommoControls();

	// Call our main frame's main function that will block until the app exits
	int ret = GetKernel()->OnRun();

	// Restore system timer resolution
	timeEndPeriod(1);

	// Destroy the dynamic objects
	GetKernel(true);
	GetTimer(true);
	GetCommandLineData(true);

	// Uninit COM
	CoUninitialize();

	// Close mutex
	CloseHandle(mutex);

#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif

	// Return the value previously returned by our app's main function
    return ret;
}

Kernel::Kernel() :
mCreated(false),
mWindowHandle(NULL),
mGraphics(NULL),
mAudio(NULL),
mMenus(NULL),
mUserInterface(NULL),
mProcessor(NULL),
mElapsedTime(NULL),
mAbsoluteTime(NULL),
mCursorPositionTime(NULL),
mLed(false)	// Default
{
}

Kernel::~Kernel()
{
	DestroyKernel();
}

void Kernel::CreateModuleInfo(void)
{
	// Get the current module directory
	wchar_t buffer[MAX_PATH];

	GetModuleFileName(NULL,buffer,MAX_PATH);

	// We only need the directory path
	PathRemoveFileSpec(buffer);
	PathAddBackslash(buffer);

	mModulePath = buffer;

	GetModuleFileName(NULL,buffer,MAX_PATH);

	PathStripPath(buffer);

	mModuleName = buffer;

	TRACE(L"Module path is: %s\n",GetModulePath().GetBuffer());
	TRACE(L"Module name is: %s\n",GetModuleName().GetBuffer());
}

bool Kernel::CreateKernel(void)
{
	// Enable loging
	if(GetCommandLineData()->GetVar(L"/log"))
		if(!mLog.OpeLog(GetCombinedPath(GetCommandLineData()->GetVar(L"/log",GetModulePath()),L"Kernel.log")))
			TRACE(L"Failed to open log file \"%s\"\n",GetCombinedPath(GetCommandLineData()->GetVar(L"/log",GetModulePath()).GetBuffer(),L"Kernel.log"));

	// Increase our priority
	if(GetCommandLineData()->GetVar(L"/highpriority",ANALYZER_HIGHPRIORITY))
	{
		TRACE(L"Setting process priority class to: Above Normal.\n");
		SetPriorityClass(GetCurrentProcess(),ABOVE_NORMAL_PRIORITY_CLASS);
	}

	// Start our app timer, start stop it just to get the current time
	GetTimer()->Start();
	GetTimer()->Stop();

	// Update the absolute time so any outputed strings douring initialization are visible
	mAbsoluteTime = GetTimer()->GetAbsoluteTime();

	// Create the processor
	mProcessor = new Processor;
	if(!mProcessor)
	{
		MessageErrorLast(L"Failed to allocate processor.",L"Error");
		TRACE(L"Failed to allocate processor.\n");
		return false;
	}

	// Set the processor type
#ifdef _X86_
	if(IsProcessorFeaturePresent(PF_XMMI_INSTRUCTIONS_AVAILABLE))
	{
		CreateSSEProcessor(mProcessor);
		TRACE(L"Math: Using SSE Instructions\n");
	}
	else
#endif
	{
		CreateGenericProcessor(mProcessor);
		TRACE(L"Math: Using Generic Instructions\n");
	}

	TRACE(L"Allocating graphics...\n");

	// Create the graphics object
	mGraphics = new Graphics;
	if(!mGraphics)
	{
		MessageErrorLast(L"Failed to allocate graphics.",L"Error");
		TRACE(L"Failed to allocate graphics.\n");
		return false;
	}

	TRACE(L"Allocating audio...\n");

	// Create the audio object
	mAudio = new Audio;
	if(!mAudio)
	{
		MessageErrorLast(L"Failed to allocate audio.",L"Error");
		TRACE(L"Failed to allocate audio.\n");
		return false;
	}

	TRACE(L"Allocating menus...\n");

	// Create the menus object
	mMenus = new Menus;
	if(!mMenus)
	{
		MessageErrorLast(L"Failed to allocate menus.",L"Error");
		TRACE(L"Failed to allocate menus.\n");
		return false;
	}

	TRACE(L"Allocating user interface...\n");

	// Create the user interface
	mUserInterface = new UserInterface;
	if(!mUserInterface)
	{
		MessageErrorLast(L"Failed to allocate user interface.",L"Error");
		TRACE(L"Failed to allocate user interface.\n");
		return false;
	}

	TRACE(L"Creating window...\n");

	DWORD style;

	if(GetCommandLineData()->GetVar(L"/border",true))
		style = WS_POPUP|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX;
	else
		style = WS_POPUP|WS_SYSMENU;

	// Create the window
	if(!CreateWindow(WINDOW_WIDTH,WINDOW_HEIGHT,style))
	{
		MessageErrorLast(L"Failed to create window.",L"Error");
		TRACE(L"Failed to create window.\n");
		return false;
	}

	// Create a fiew DEBUG windows
	if(GetCommandLineData()->GetVar(L"/ui",false))
	{
		WindowHandle window1 = mUserInterface->NewWindow(Point(40,80),Size(300,280),Size(300,280),Size(500,280),L"Color Editor");

		window1->SetMinimizeVisible(false);
		window1->SetMinimizeEnabled(false);
		window1->SetCallbackClass(this);

		window1->AttachChild(mUserInterface->NewButton(10,Point(216,248),Size(75,23),L"Copy"));
		window1->AttachChild(mUserInterface->NewComboBox(20,Point(10,30),Size(280,19)));
		window1->AttachChild(mUserInterface->NewStaticText(Point(10,60),Size(150,20),L"Red: 0"));
		window1->AttachChild(mUserInterface->NewTrackBar(30,Point(10,75),Size(280,18),255,0));
		window1->AttachChild(mUserInterface->NewStaticText(Point(10,105),Size(150,20),L"Green: 0"));
		window1->AttachChild(mUserInterface->NewTrackBar(31,Point(10,120),Size(280,18),255,0));
		window1->AttachChild(mUserInterface->NewStaticText(Point(10,150),Size(150,20),L"Blue: 0"));
		window1->AttachChild(mUserInterface->NewTrackBar(32,Point(10,165),Size(280,18),255,0));
		window1->AttachChild(mUserInterface->NewStaticText(Point(10,195),Size(150,20),L"Alpha: 0"));
		window1->AttachChild(mUserInterface->NewTrackBar(33,Point(10,210),Size(280,18),255,0));
		window1->AttachChild(mUserInterface->NewButton(11,Point(10,248),Size(75,23),L"Sync Controls"));

		window1->GetChild(0)->SetSnap(Control::SnapRight | Control::SnapBottom);
		window1->GetChild(1)->SetSnap(Control::SnapLeft | Control::SnapRight | Control::SnapTop);
		window1->GetChild(2)->SetSnap(Control::SnapLeft | Control::SnapRight | Control::SnapTop);
		window1->GetChild(3)->SetSnap(Control::SnapLeft | Control::SnapRight | Control::SnapTop);
		window1->GetChild(4)->SetSnap(Control::SnapLeft | Control::SnapRight | Control::SnapTop);
		window1->GetChild(5)->SetSnap(Control::SnapLeft | Control::SnapRight | Control::SnapTop);
		window1->GetChild(6)->SetSnap(Control::SnapLeft | Control::SnapRight | Control::SnapTop);
		window1->GetChild(7)->SetSnap(Control::SnapLeft | Control::SnapRight | Control::SnapTop);
		window1->GetChild(8)->SetSnap(Control::SnapLeft | Control::SnapRight | Control::SnapTop);
		window1->GetChild(9)->SetSnap(Control::SnapLeft | Control::SnapRight | Control::SnapTop);

		((ComboBoxHandle)window1->GetChild(1))->AddItem(L"Foreground");
		((ComboBoxHandle)window1->GetChild(1))->AddItem(L"Background");
		((ComboBoxHandle)window1->GetChild(1))->AddItem(L"Peaks");
		((ComboBoxHandle)window1->GetChild(1))->AddItem(L"Text1");
		((ComboBoxHandle)window1->GetChild(1))->AddItem(L"Text2");
		((ComboBoxHandle)window1->GetChild(1))->AddItem(L"Text3");
	}

	// Init the direct3d device in the main application thread
	if(!mGraphics->CreateGraphics())
	{
		MessageErrorLast(L"Failed to initialize graphics.",L"Error");
		TRACE(L"Failed to initialize graphics.\n");
		return false;
	}

	// Create the fonts
	if(!mGraphics->CreateFonts())
	{
		MessageErrorLast(L"Failed to initialize resources.",L"Error");
		TRACE(L"Failed to initialize resources.\n");
		return false;
	}

	// Create the render targets
	if(!mGraphics->CreateRenderTargets())
	{
		MessageErrorLast(L"Failed to initialize resources.",L"Error");
		TRACE(L"Failed to initialize resources.\n");
		return false;
	}

	// Init textures
	if(!mGraphics->CreateTextures())
	{
		MessageErrorLast(L"Failed to initialize resources.",L"Error");
		TRACE(L"Failed to initialize resources.\n");
		return false;
	}

	if(!mGraphics->CreateSprites())
	{
		MessageErrorLast(L"Failed to initialize resource.",L"Error");
		TRACE(L"Failed to initialize resource.\n");
		return false;
	}

	// Init the sound capture objects
	if(!mAudio->CreateAudio())
	{
		MessageErrorLast(L"Failed to initialize audio system.",L"Error");
		TRACE(L"Failed to initialize resources.\n");
		return false;
	}

	// Start the capture of audio
	if(!mAudio->StartCapture())
	{
		MessageErrorLast(L"Failed to start capturing audio.",L"Error");
		TRACE(L"Failed to initialize resources.\n");
		return false;
	}

	// Load the default color presets file
	mGraphics->ReadColorPresets();

	// Add the visualizations
//#ifdef _DEBUG
	mGraphics->AddVisualization(new PowerSpectrum());
	mGraphics->AddVisualization(new LazySpectrumA());
	mGraphics->AddVisualization(new LazySpectrumB());
//#endif
	mGraphics->AddVisualization(new AnalyzerRedesigned());
	mGraphics->AddVisualization(new Scope());
	mGraphics->AddVisualization(new DoubleScope());
	mGraphics->AddVisualization(new VuMeter());
//#ifdef _DEBUG
	mGraphics->AddVisualization(new AmoebaScope());
//#endif

	// Add the transitions
	mGraphics->AddTransition(new TransitionTVHorizontal());
	mGraphics->AddTransition(new TransitionTVVertical());
	mGraphics->AddTransition(new TransitionTVCenter());
	mGraphics->AddTransition(new TransitionStretchLeft());
	mGraphics->AddTransition(new TransitionStretchTop());
	mGraphics->AddTransition(new TransitionStretchRight());
	mGraphics->AddTransition(new TransitionStretchBottom());
	mGraphics->AddTransition(new TransitionStretchInLeft());
	mGraphics->AddTransition(new TransitionStretchInTop());
	mGraphics->AddTransition(new TransitionSlideInTop());
	mGraphics->AddTransition(new TransitionSlideInBottom());
	mGraphics->AddTransition(new TransitionSlideInLeft());
	mGraphics->AddTransition(new TransitionSlideInRight());

	// Load the config
	ReadConfig();

	// Init the menus
	if(!mMenus->CreateMenus())
	{
		MessageErrorLast(L"Failed to initialize menu system.",L"Error");
		TRACE(L"Failed to initialize menu system.\n");
		return false;
	}

	mGraphics->PrintOutputString(L"press \002F1\001 for help");

	// Actualy start the timer
	GetTimer()->Start();

	return true;
}

void Kernel::DestroyKernel(void)
{
	// Save the config
	//WriteConfig(NULL);

	SAFEDELETE(mGraphics);
	SAFEDELETE(mAudio);
	SAFEDELETE(mMenus);
	SAFEDELETE(mUserInterface);
	SAFEDELETE(mProcessor);

	DestroyWindow();
}

#pragma region Command Event Handlers

bool Kernel::OnCommandEvent(const ControlHandle& sender,WPARAM wparam,LPARAM /*lparam*/)
{
	static wchar_t text[1024];

	switch(sender->GetID())
	{
	case 10:
		swprintf(text,L"[Untitled]\r\n"
					  L"Foreground=%g %g %g %g\r\n"
					  L"Background=%g %g %g %g\r\n"
					  L"Peak=%g %g %g %g\r\n"
					  L"Text1=%g %g %g %g\r\n"
					  L"Text2=%g %g %g %g\r\n"
					  L"Text3=%g %g %g %g\r\n",
					   mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->foreground.a,
					   mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->foreground.r,
					   mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->foreground.g,
					   mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->foreground.b,
					   mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->background.a,
					   mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->background.r,
					   mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->background.g,
					   mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->background.b,
					   mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->peak.a,
					   mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->peak.r,
					   mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->peak.g,
					   mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->peak.b,
					   mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->text[0].a,
					   mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->text[0].r,
					   mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->text[0].g,
					   mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->text[0].b,
					   mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->text[1].a,
					   mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->text[1].r,
					   mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->text[1].g,
					   mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->text[1].b,
					   mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->text[2].a,
					   mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->text[2].r,
					   mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->text[2].g,
					   mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->text[2].b);

		if(OpenClipboard(mWindowHandle))
		{
			EmptyClipboard();

			HGLOBAL clipboardData = GlobalAlloc(GMEM_DDESHARE,sizeof(wchar_t)*(wcslen(text)+1));
			
			LPVOID data = GlobalLock(clipboardData);

			CopyMemory(data,text,sizeof(wchar_t)*(wcslen(text)+1));

			GlobalUnlock(clipboardData);

			SetClipboardData(CF_UNICODETEXT,clipboardData);

			CloseClipboard();
		}
		return true;

	case 11:
		OnCommandEvent(mUserInterface->GetChild(0)->GetChild(1),Control::EventChildCommand,NULL);
		return true;

	case 30:
		swprintf(text,L"Red: %g",(float)((TrackBarHandle)sender)->GetTrackValue()/255.0f);
		((StaticTextHandle)mUserInterface->GetChild(0)->GetChild(2))->SetText(text);

		switch(((ComboBoxHandle)mUserInterface->GetChild(0)->GetChild(1))->GetSelectedItem())
		{
		case 0:
			mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->foreground.r = (float)((TrackBarHandle)sender)->GetTrackValue()/255.0f;
			break;

		case 1:
			mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->background.r = (float)((TrackBarHandle)sender)->GetTrackValue()/255.0f;
			break;

		case 2:
			mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->peak.r = (float)((TrackBarHandle)sender)->GetTrackValue()/255.0f;
			break;

		case 3:
			mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->text[0].r = (float)((TrackBarHandle)sender)->GetTrackValue()/255.0f;
			break;

		case 4:
			mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->text[1].r = (float)((TrackBarHandle)sender)->GetTrackValue()/255.0f;
			break;

		case 5:
			mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->text[2].r = (float)((TrackBarHandle)sender)->GetTrackValue()/255.0f;
			break;
		}
		return true;

	case 31:
		swprintf(text,L"Green: %g",(float)((TrackBarHandle)sender)->GetTrackValue()/255.0f);
		((StaticTextHandle)mUserInterface->GetChild(0)->GetChild(4))->SetText(text);

		switch(((ComboBoxHandle)mUserInterface->GetChild(0)->GetChild(1))->GetSelectedItem())
		{
		case 0:
			mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->foreground.g = (float)((TrackBarHandle)sender)->GetTrackValue()/255.0f;
			break;

		case 1:
			mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->background.g = (float)((TrackBarHandle)sender)->GetTrackValue()/255.0f;
			break;

		case 2:
			mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->peak.g = (float)((TrackBarHandle)sender)->GetTrackValue()/255.0f;
			break;

		case 3:
			mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->text[0].g = (float)((TrackBarHandle)sender)->GetTrackValue()/255.0f;
			break;

		case 4:
			mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->text[1].g = (float)((TrackBarHandle)sender)->GetTrackValue()/255.0f;
			break;

		case 5:
			mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->text[2].g = (float)((TrackBarHandle)sender)->GetTrackValue()/255.0f;
			break;
		}
		return true;

	case 32:
		swprintf(text,L"Blue: %g",(float)((TrackBarHandle)sender)->GetTrackValue()/255.0f);
		((StaticTextHandle)mUserInterface->GetChild(0)->GetChild(6))->SetText(text);

		switch(((ComboBoxHandle)mUserInterface->GetChild(0)->GetChild(1))->GetSelectedItem())
		{
		case 0:
			mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->foreground.b = (float)((TrackBarHandle)sender)->GetTrackValue()/255.0f;
			break;

		case 1:
			mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->background.b = (float)((TrackBarHandle)sender)->GetTrackValue()/255.0f;
			break;

		case 2:
			mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->peak.b = (float)((TrackBarHandle)sender)->GetTrackValue()/255.0f;
			break;

		case 3:
			mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->text[0].b = (float)((TrackBarHandle)sender)->GetTrackValue()/255.0f;
			break;

		case 4:
			mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->text[1].b = (float)((TrackBarHandle)sender)->GetTrackValue()/255.0f;
			break;

		case 5:
			mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->text[2].b = (float)((TrackBarHandle)sender)->GetTrackValue()/255.0f;
			break;
		}
		return true;

	case 33:
		swprintf(text,L"Alpha: %g",(float)((TrackBarHandle)sender)->GetTrackValue()/255.0f);
		((StaticTextHandle)mUserInterface->GetChild(0)->GetChild(8))->SetText(text);

		switch(((ComboBoxHandle)mUserInterface->GetChild(0)->GetChild(1))->GetSelectedItem())
		{
		case 0:
			mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->foreground.a = (float)((TrackBarHandle)sender)->GetTrackValue()/255.0f;
			break;

		case 1:
			mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->background.a = (float)((TrackBarHandle)sender)->GetTrackValue()/255.0f;
			break;

		case 2:
			mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->peak.a = (float)((TrackBarHandle)sender)->GetTrackValue()/255.0f;
			break;

		case 3:
			mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->text[0].a = (float)((TrackBarHandle)sender)->GetTrackValue()/255.0f;
			break;

		case 4:
			mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->text[1].a = (float)((TrackBarHandle)sender)->GetTrackValue()/255.0f;
			break;

		case 5:
			mGraphics->GetColorPreset(mGraphics->GetColorPresetIndex())->text[2].a = (float)((TrackBarHandle)sender)->GetTrackValue()/255.0f;
			break;
		}
		return true;

	case 20:
		switch(((ComboBoxHandle)sender)->GetSelectedItem())
		{
		case 0:
			((TrackBarHandle)mUserInterface->GetChild(0)->GetChild(3))->SetTrackValue(mGraphics->GetColorPreset()->foreground.r * 255);
			((TrackBarHandle)mUserInterface->GetChild(0)->GetChild(5))->SetTrackValue(mGraphics->GetColorPreset()->foreground.g * 255);
			((TrackBarHandle)mUserInterface->GetChild(0)->GetChild(7))->SetTrackValue(mGraphics->GetColorPreset()->foreground.b * 255);
			((TrackBarHandle)mUserInterface->GetChild(0)->GetChild(9))->SetTrackValue(mGraphics->GetColorPreset()->foreground.a * 255);
			
			OnCommandEvent(mUserInterface->GetChild(0)->GetChild(3),Control::EventChildCommand,NULL);
			OnCommandEvent(mUserInterface->GetChild(0)->GetChild(5),Control::EventChildCommand,NULL);
			OnCommandEvent(mUserInterface->GetChild(0)->GetChild(7),Control::EventChildCommand,NULL);
			OnCommandEvent(mUserInterface->GetChild(0)->GetChild(9),Control::EventChildCommand,NULL);
			break;

		case 1:
			((TrackBarHandle)mUserInterface->GetChild(0)->GetChild(3))->SetTrackValue(mGraphics->GetColorPreset()->background.r * 255);
			((TrackBarHandle)mUserInterface->GetChild(0)->GetChild(5))->SetTrackValue(mGraphics->GetColorPreset()->background.g * 255);
			((TrackBarHandle)mUserInterface->GetChild(0)->GetChild(7))->SetTrackValue(mGraphics->GetColorPreset()->background.b * 255);
			((TrackBarHandle)mUserInterface->GetChild(0)->GetChild(9))->SetTrackValue(mGraphics->GetColorPreset()->background.a * 255);

			OnCommandEvent(mUserInterface->GetChild(0)->GetChild(3),Control::EventChildCommand,NULL);
			OnCommandEvent(mUserInterface->GetChild(0)->GetChild(5),Control::EventChildCommand,NULL);
			OnCommandEvent(mUserInterface->GetChild(0)->GetChild(7),Control::EventChildCommand,NULL);
			OnCommandEvent(mUserInterface->GetChild(0)->GetChild(9),Control::EventChildCommand,NULL);
			break;

		case 2:
			((TrackBarHandle)mUserInterface->GetChild(0)->GetChild(3))->SetTrackValue(mGraphics->GetColorPreset()->peak.r * 255);
			((TrackBarHandle)mUserInterface->GetChild(0)->GetChild(5))->SetTrackValue(mGraphics->GetColorPreset()->peak.g * 255);
			((TrackBarHandle)mUserInterface->GetChild(0)->GetChild(7))->SetTrackValue(mGraphics->GetColorPreset()->peak.b * 255);
			((TrackBarHandle)mUserInterface->GetChild(0)->GetChild(9))->SetTrackValue(mGraphics->GetColorPreset()->peak.a * 255);

			OnCommandEvent(mUserInterface->GetChild(0)->GetChild(3),Control::EventChildCommand,NULL);
			OnCommandEvent(mUserInterface->GetChild(0)->GetChild(5),Control::EventChildCommand,NULL);
			OnCommandEvent(mUserInterface->GetChild(0)->GetChild(7),Control::EventChildCommand,NULL);
			OnCommandEvent(mUserInterface->GetChild(0)->GetChild(9),Control::EventChildCommand,NULL);
			break;

		case 3:
			((TrackBarHandle)mUserInterface->GetChild(0)->GetChild(3))->SetTrackValue(mGraphics->GetColorPreset()->text[0].r * 255);
			((TrackBarHandle)mUserInterface->GetChild(0)->GetChild(5))->SetTrackValue(mGraphics->GetColorPreset()->text[0].g * 255);
			((TrackBarHandle)mUserInterface->GetChild(0)->GetChild(7))->SetTrackValue(mGraphics->GetColorPreset()->text[0].b * 255);
			((TrackBarHandle)mUserInterface->GetChild(0)->GetChild(9))->SetTrackValue(mGraphics->GetColorPreset()->text[0].a * 255);

			OnCommandEvent(mUserInterface->GetChild(0)->GetChild(3),Control::EventChildCommand,NULL);
			OnCommandEvent(mUserInterface->GetChild(0)->GetChild(5),Control::EventChildCommand,NULL);
			OnCommandEvent(mUserInterface->GetChild(0)->GetChild(7),Control::EventChildCommand,NULL);
			OnCommandEvent(mUserInterface->GetChild(0)->GetChild(9),Control::EventChildCommand,NULL);
			break;

		case 4:
			((TrackBarHandle)mUserInterface->GetChild(0)->GetChild(3))->SetTrackValue(mGraphics->GetColorPreset()->text[1].r * 255);
			((TrackBarHandle)mUserInterface->GetChild(0)->GetChild(5))->SetTrackValue(mGraphics->GetColorPreset()->text[1].g * 255);
			((TrackBarHandle)mUserInterface->GetChild(0)->GetChild(7))->SetTrackValue(mGraphics->GetColorPreset()->text[1].b * 255);
			((TrackBarHandle)mUserInterface->GetChild(0)->GetChild(9))->SetTrackValue(mGraphics->GetColorPreset()->text[1].a * 255);

			OnCommandEvent(mUserInterface->GetChild(0)->GetChild(3),Control::EventChildCommand,NULL);
			OnCommandEvent(mUserInterface->GetChild(0)->GetChild(5),Control::EventChildCommand,NULL);
			OnCommandEvent(mUserInterface->GetChild(0)->GetChild(7),Control::EventChildCommand,NULL);
			OnCommandEvent(mUserInterface->GetChild(0)->GetChild(9),Control::EventChildCommand,NULL);
			break;

		case 5:
			((TrackBarHandle)mUserInterface->GetChild(0)->GetChild(3))->SetTrackValue(mGraphics->GetColorPreset()->text[2].r * 255);
			((TrackBarHandle)mUserInterface->GetChild(0)->GetChild(5))->SetTrackValue(mGraphics->GetColorPreset()->text[2].g * 255);
			((TrackBarHandle)mUserInterface->GetChild(0)->GetChild(7))->SetTrackValue(mGraphics->GetColorPreset()->text[2].b * 255);
			((TrackBarHandle)mUserInterface->GetChild(0)->GetChild(9))->SetTrackValue(mGraphics->GetColorPreset()->text[2].a * 255);

			OnCommandEvent(mUserInterface->GetChild(0)->GetChild(3),Control::EventChildCommand,NULL);
			OnCommandEvent(mUserInterface->GetChild(0)->GetChild(5),Control::EventChildCommand,NULL);
			OnCommandEvent(mUserInterface->GetChild(0)->GetChild(7),Control::EventChildCommand,NULL);
			OnCommandEvent(mUserInterface->GetChild(0)->GetChild(9),Control::EventChildCommand,NULL);
			break;
		}
		return true;
	}

	TRACE(L"Got command event %u (control %u).\n",wparam,sender->GetID());

	return false;
}
#pragma endregion

bool Kernel::CreateWindow(unsigned long width,unsigned long height,unsigned long style)
{
	// Register the window class
	ZeroMemory(&mWindowClass,sizeof(mWindowClass));
	mWindowClass.cbSize = sizeof(WNDCLASSEX);
	if(GetCommandLineData()->GetVar(L"/shadow",false))
		mWindowClass.style = CS_CLASSDC | CS_DBLCLKS | 0x00020000 /*CS_DROPSHADOW*/;
	else
		mWindowClass.style = CS_CLASSDC | CS_DBLCLKS;
	mWindowClass.lpfnWndProc = Kernel::WindowProcedure;
	mWindowClass.hInstance = GetModuleHandle(NULL);
	mWindowClass.hIcon = LoadIcon(GetModuleHandle(NULL),MAKEINTRESOURCE(IDI_MAIN_ICON));
	mWindowClass.hCursor = LoadCursor(NULL,IDC_ARROW);
	mWindowClass.lpszClassName = ANALYZER_CLASSNAME;
    
	if(!RegisterClassEx(&mWindowClass))
	{
		GetKernel()->SetLastError(L"Failed to register window class.");
		TRACE(L"Failed to register window class.\n");
		return false;
	}

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Create the application's window
	mWindowHandle = CreateWindowEx(NULL,ANALYZER_CLASSNAME, ANALYZER_WINDOWTITLE, style, screenWidth / 2 - width / 2, screenHeight / 2 - height / 2, width, height, GetDesktopWindow(), NULL, mWindowClass.hInstance, NULL);
	if(!mWindowHandle)
	{
		GetKernel()->SetLastError(L"Failed to create window.");
		TRACE(L"Failed to create window.\n");
		return false;
	}

	// Set the correct client size
	Rect rect(0,0,width,height);
	AdjustWindowRect((LPRECT)&rect,style,FALSE);
	SetWindowPos(mWindowHandle,NULL,0,0,rect.GetWidth(),rect.GetHeight(),SWP_NOMOVE);

	mWindowStyle = style;

	TRACE(L"Window %dx%d, style %#X\n",width,height,style);

	return true;
}

void Kernel::DestroyWindow(void)
{
	// Destroy the window object
	::DestroyWindow(mWindowHandle);

	// Unregister the window class registrated at the begining of this function
	UnregisterClass(ANALYZER_CLASSNAME, mWindowClass.hInstance);
}

bool Kernel::ShowWindow(void)
{
	if(!mWindowHandle)
		return false;

	return ::ShowWindow(mWindowHandle,SW_SHOW) ? true : false;
}

bool Kernel::HideWindow(void)
{
	if(!mWindowHandle)
		return false;

	return ::ShowWindow(mWindowHandle,SW_HIDE) ? true : false;
}

bool Kernel::GetWindowVisible(void)
{
	if(!mWindowHandle)
		return false;

	return ::IsWindowVisible(mWindowHandle) ? true : false;
}

int Kernel::OnRun(void)
{
	// Get the module info
	CreateModuleInfo();

	// Set the command line string
	GetCommandLineData()->Set(::GetCommandLine());

	// Create the kernel
	if(!CreateKernel())
		return -1;

	// This will hold our messages
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));  // Must do this otherwise the app would hang

	// Enter the message loop
	while(msg.message != WM_QUIT)
	{
		if(PeekMessage(&msg,NULL,NULL,NULL,PM_REMOVE))
		{
			// We got a message so translate and dispatch it
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if(GetTimer()->GetStopped())	// Don't idle if we are paused
			Sleep(10);
		else
			OnIdle();
	}

	// Destroy the kernel objects
	DestroyKernel();

	// Return the wParam cause it contains our exit code
	return (int)msg.wParam;
}

void Kernel::OnIdle(void)
{
	// Update the speed factor
	double elapsed = GetTimer()->GetElapsedTime();
	mAbsoluteTime += elapsed;
	mElapsedTime = elapsed * ANALYZER_FRAMERATE;
	if(mElapsedTime > 5.0f)
	{
		TRACE(L"Overhead by %g, reset to 5.0\n",mElapsedTime);
		mElapsedTime = 5.0f;
	}

	// Update the cursor position over our window
	POINT pos;
	GetCursorPos(&pos);
	RECT rect;
	GetClientRect(mWindowHandle,&rect);
	ScreenToClient(mWindowHandle,&pos);
	if(!PtInRect(&rect,pos))
		SetCursorPositionTime(0);

	// Process the capture and preparation of audio
	if(mAudio && mAudio->GetCreated())
		mAudio->Process();

	// Update windows
	if(mUserInterface)
		mUserInterface->OnUpdate();

	// Render the captured data
	if(mGraphics && mGraphics->GetCreated())
		mGraphics->Render();
}

void Kernel::MessageFormat(const wchar_t* fmt,...)
{
	static wchar_t buffer[8096];
    
    va_list args;
    va_start(args,fmt);
    _vsnwprintf(buffer,_countof(buffer),fmt,args);
    va_end(args);

	if(mLog.GetOpen())
		mLog.Write(buffer);

	OutputDebugString(buffer);
}

void Kernel::MessageLocation(const char* location,const wchar_t* fmt,...)
{
	static wchar_t buffer[8096];
    
    va_list args;
    va_start(args,fmt);
    _vsnwprintf(buffer,_countof(buffer),fmt,args);
    va_end(args);

	MessageFormat(L"%S: %s",location,buffer);
}

void Kernel::MessageLine(const char* file,unsigned long line,const wchar_t* fmt,...)
{
	static wchar_t buffer[8096];
    
    va_list args;
    va_start(args,fmt);
    _vsnwprintf(buffer,_countof(buffer),fmt,args);
    va_end(args);

	MessageFormat(L"%S(%d): %s",file,line,buffer);
}

void Kernel::MessageModule(const wchar_t* fmt,...)
{
	wchar_t buffer[8096];
    
    va_list args;
    va_start(args,fmt);
    _vsnwprintf(buffer,_countof(buffer),fmt,args);
    va_end(args);

	MessageFormat(L"'%s': %s",GetModuleName().GetBuffer(),buffer);
}

void Kernel::MessageErrorLast(const wchar_t* section,const wchar_t* title)
{
	String messsage;

	messsage += section;
	messsage += L"\n\n";
	messsage += GetKernel()->GetLastError();
	
	MessageError(messsage,title);
}

void Kernel::MessageError(const wchar_t* message,const wchar_t* title)
{
	MessageBox(GetDesktopWindow(),message,title,MB_OK|MB_ICONEXCLAMATION|MB_SETFOREGROUND);
}

void Kernel::SetLastError(const wchar_t* fmt,...)
{
	static wchar_t buffer[8096];
    
    va_list args;
    va_start(args,fmt);
    _vsnwprintf(buffer,_countof(buffer),fmt,args);
    va_end(args);

	mLastError = buffer;
}

bool Kernel::Dump(const wchar_t* /*path*/)
{
	/*
	String pathString;

	if(!path)
		pathString = GetKernel()->GetCombinedModulePath(L"Dump.dmp");
	else
		pathString = path;

	HANDLE file = CreateFile(pathString,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,NULL,NULL);
	if(file == INVALID_HANDLE_VALUE)
		return false;

	MiniDumpWriteDump(GetCurrentProcess(),GetCurrentProcessId(),file,MiniDumpNormal,NULL,NULL,NULL);

	CloseHandle(file);
	*/

	return true;
}

void Kernel::SetLed(float bmax)
{
	// Get the current keyboard state
	BYTE kstate[256];
	GetKeyboardState(kstate);

	if(bmax > 0.2f)	// Numlock should be ON
	{
		if(!(kstate[VK_NUMLOCK] & 1))
		{
			keybd_event(VK_NUMLOCK,0,NULL,0);
			keybd_event(VK_NUMLOCK,0,KEYEVENTF_KEYUP,0);
		}
	}
	else	// Numlock should be OFF
	{
		if(kstate[VK_NUMLOCK] & 1)
		{
			keybd_event(VK_NUMLOCK,0,NULL,0);
			keybd_event(VK_NUMLOCK,0,KEYEVENTF_KEYUP,0);
		}
	}

	if(bmax > 0.5f)	// Capslock should be ON
	{
		if(!(kstate[VK_CAPITAL] & 1))
		{
			keybd_event(VK_CAPITAL,0,NULL,0);
			keybd_event(VK_CAPITAL,0,KEYEVENTF_KEYUP,0);
		}
	}
	else	// Capslock should be OFF
	{
		if(kstate[VK_CAPITAL] & 1)
		{
			keybd_event(VK_CAPITAL,0,NULL,0);
			keybd_event(VK_CAPITAL,0,KEYEVENTF_KEYUP,0);
		}
	}

	if(bmax > 0.9f)	// Scorlllock should be ON
	{
		if(!(kstate[VK_SCROLL] & 1))
		{
			keybd_event(VK_SCROLL,0,NULL,0);
			keybd_event(VK_SCROLL,0,KEYEVENTF_KEYUP,0);
		}
	}
	else	// Scorlllock should be OFF
	{
		if(kstate[VK_SCROLL] & 1)
		{
			keybd_event(VK_SCROLL,0,NULL,0);
			keybd_event(VK_SCROLL,0,KEYEVENTF_KEYUP,0);
		}
	}
}

bool Kernel::ReadConfig(const wchar_t* filePath)
{
	String pathString;

	if(!filePath)
		pathString = GetKernel()->GetCombinedModulePath(L"Config.ini");
	else
		pathString = filePath;

	Configuration config(pathString);

	mGraphics->SetColorPreset(config[L"Main"][L"Preset"](mGraphics->GetColorPreset()->name));
	mGraphics->SetVisualization(config[L"Main"][L"Visualization"](mGraphics->GetVisualizationName(mGraphics->GetVisualizationIndex())));
	mAudio->SetSource(config[L"Main"][L"Mixer"]);
	mGraphics->SetDrawPeaks(config[L"Main"][L"Peaks"](mGraphics->GetDrawPeaks()));
	mGraphics->SetDrawCursor(config[L"Main"][L"Cursor"](mGraphics->GetDrawCursor()));
	mGraphics->SetDrawFps(config[L"Main"][L"Fps"](mGraphics->GetDrawFps()));
	mGraphics->SetDrawOutputStrings(config[L"Main"][L"Output"](mGraphics->GetDrawOutputStrings()));
	mGraphics->SetDrawTimer(config[L"Main"][L"Timer"](mGraphics->GetDrawTimer()));
	mGraphics->SetDrawVisualizationName(config[L"Main"][L"Name"](mGraphics->GetDrawVisualizationName()));
	
	mAudio->SetEqFalloff(config[L"Main"][L"Eq Falloff"](mAudio->GetEqFalloff()));
	mAudio->SetPeakFalloff(config[L"Main"][L"Peak Falloff"](mAudio->GetPeakFalloff()));
	mAudio->SetPeakDelay(config[L"Main"][L"Peak Delay"](mAudio->GetPeakDelay()));

	mGraphics->PrintOutputString(L"configuration loaded: \002%s\001",GetKernel()->GetStripedPath(pathString).GetBuffer());
	return true;
}

bool Kernel::WriteConfig(const wchar_t* filePath)
{
	String pathString;

	if(!filePath)
		pathString = GetKernel()->GetCombinedModulePath(L"Config.ini");
	else
		pathString = filePath;

	Configuration config(pathString);

	config[L"Main"][L"Preset"] = mGraphics->GetColorPreset()->name;
	config[L"Main"][L"Visualization"] = mGraphics->GetVisualizationName(mGraphics->GetVisualizationIndex());
	config[L"Main"][L"Peaks"] = mGraphics->GetDrawPeaks();
	config[L"Main"][L"Fps"] = mGraphics->GetDrawFps();
	config[L"Main"][L"Output"] = mGraphics->GetDrawOutputStrings();
	config[L"Main"][L"Timer"] = mGraphics->GetDrawTimer();
	config[L"Main"][L"Name"] = mGraphics->GetDrawVisualizationName();
	
	config[L"Main"][L"Eq Falloff"] = mAudio->GetEqFalloff();
	config[L"Main"][L"Peak Falloff"] = mAudio->GetPeakFalloff();
	config[L"Main"][L"Peak Delay"] = mAudio->GetPeakDelay();

	mGraphics->PrintOutputString(L"configuration saved: \002%s\001",GetKernel()->GetStripedPath(pathString).GetBuffer());
	return true;
}

void Kernel::SetLed(bool led)
{
	if(mLed == led)
		return;

	TRACE(L"Keyboard LED turned %s.\n",led ? L"on" : L"off");
	if(mGraphics)
		mGraphics->PrintOutputString(L"keyboard LED: \002%s\001",led ? L"on" : L"off");
	
	mLed = led;

	if(!mLed)
		SetLed(0.0f);
}

void Kernel::SetCursorPosition(const Point& position)
{
	if(position.x == mCursorPosition.x && position.y == mCursorPosition.y)
		return;

	mCursorPosition = position;
	mCursorPositionTime = GetKernel()->GetAbsoluteTime();

	if(mCursorPosition.x < 0)
		mCursorPosition.x = 0;
	if(mCursorPosition.y < 0)
		mCursorPosition.y = 0;

	if(!GetKernel()->GetGraphics() || !GetKernel()->GetGraphics()->GetCreated())
		return;

	if(mCursorPosition.y > (LONG)GetKernel()->GetGraphics()->GetDisplayHeight())
		mCursorPosition.y = GetKernel()->GetGraphics()->GetDisplayHeight();
	if(mCursorPosition.x > (LONG)GetKernel()->GetGraphics()->GetDisplayWidth())
		mCursorPosition.x = GetKernel()->GetGraphics()->GetDisplayHeight();
}

String Kernel::GetCombinedPath(const wchar_t* path1,const wchar_t* path2)
{
	wchar_t buffer[MAX_PATH];
	PathCombine(buffer,path1,path2);
	return buffer;
}

String Kernel::GetStripedPath(const wchar_t* path)
{
	wchar_t buffer[MAX_PATH];
	lstrcpyn(buffer,path,MAX_PATH);
	PathStripPath(buffer);
	return buffer;
}

LRESULT CALLBACK Kernel::WindowProcedure(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	Point delta;

	// Our message handler
	switch(msg)
	{
		case WM_CLEAR:
			return TRUE;

		case WM_SETCURSOR:
			if(LOWORD(lParam) == HTCLIENT)
			{
				// We draw our own cline-area cursor
				SetCursor(NULL);
				return TRUE;
			}

			break;

		case WM_EXITMENULOOP:
		case WM_EXITSIZEMOVE:
			// Unpause the app
			GetTimer()->Reset();
			GetTimer()->Start();
			break;

		case WM_ENTERMENULOOP:
		case WM_ENTERSIZEMOVE:
			// Pause the app
			GetTimer()->Stop();
			break;

		case WM_ACTIVATE:
			if(HIWORD(wParam))
				GetTimer()->Stop();
			else
			{
				GetTimer()->Reset();
				GetTimer()->Start();
			}
			break;

		case WM_KILLFOCUS:
			if(GetKernel()->GetLed())
				GetKernel()->SetLed(false);
			break;

		case WM_CLOSE:
		case WM_DESTROY:
			if(GetKernel()->GetLed())
				GetKernel()->SetLed(0.0f);	// Turn off leds

			PostQuitMessage(0); // Post the quit message so we exit our message pump
			return TRUE;

		//case WM_LBUTTONDOWN:
		//	// This is a nice trick for making the window move when the user drags it
		//	// pause the app when it's being draged
		//	DXUtil_Timer(TIMER_STOP);
		//	ReleaseCapture(); 
		//	SetCursor(LoadCursor(0,IDC_SIZEALL));
		//	SendMessage(m_hwnd,WM_NCLBUTTONDOWN, HTCAPTION, 0); 
		//	return TRUE;

		//case WM_LBUTTONUP:
		//	DXUtil_Timer(TIMER_START);
		//	return TRUE;

		case WM_LBUTTONDBLCLK:
			SetCapture(hWnd);

			if(GetKernel()->GetUserInterface())
				if(GetKernel()->GetUserInterface()->OnEvent(Control::EventLeftButtonDoubleClick,(WPARAM)&GetKernel()->GetCursorPosition()))
					return true;

			break;

		case WM_LBUTTONDOWN:
			SetCapture(hWnd);

			if(GetKernel()->GetUserInterface())
				if(GetKernel()->GetUserInterface()->OnEvent(Control::EventLeftButtonDown,(WPARAM)&GetKernel()->GetCursorPosition()))
					return true;

			// Send event to menu
			if(GetKernel()->GetGraphics() && GetKernel()->GetMenus()->GetMenuRoot()->GetCurrentMenu() && GetKernel()->GetMenus()->GetMenuRoot()->GetCurrentMenu()->GetOpen())
				return GetKernel()->GetMenus()->GetMenuRoot()->MouseEvent(1,Point(0,0));

			// Trick Windows into thinking that the user is dragging the window
			ReleaseCapture();
			SendMessage(hWnd,WM_NCLBUTTONDOWN,HTCAPTION,lParam);

			break;

		case WM_LBUTTONUP:
			ReleaseCapture();

			if(GetKernel()->GetUserInterface())
				if(GetKernel()->GetUserInterface()->OnEvent(Control::EventLeftButtonUp,(WPARAM)&GetKernel()->GetCursorPosition()))
					return true;

			if(GetKernel()->GetGraphics() && GetKernel()->GetMenus()->GetMenuRoot()->GetCurrentMenu())
				if(GetKernel()->GetMenus()->GetMenuRoot()->GetCurrentMenu()->GetOpen())
					return GetKernel()->GetMenus()->GetMenuRoot()->MouseEvent(2,Point(0,0));
			break;

		case WM_MBUTTONDBLCLK:
			SetCapture(hWnd);

			if(GetKernel()->GetUserInterface())
				if(GetKernel()->GetUserInterface()->OnEvent(Control::EventMiddleButtonDoubleClick,(WPARAM)&GetKernel()->GetCursorPosition()))
					return true;

			break;

		case WM_MBUTTONDOWN:
			SetCapture(hWnd);

			if(GetKernel()->GetUserInterface())
				if(GetKernel()->GetUserInterface()->OnEvent(Control::EventMiddleButtonDown,(WPARAM)&GetKernel()->GetCursorPosition()))
					return true;

			break;

		case WM_MBUTTONUP:
			ReleaseCapture();

			if(GetKernel()->GetUserInterface())
				if(GetKernel()->GetUserInterface()->OnEvent(Control::EventMiddleButtonUp,(WPARAM)&GetKernel()->GetCursorPosition()))
					return true;

			if(GetKernel()->GetGraphics() && GetKernel()->GetMenus()->GetMenuRoot()->GetCurrentMenu())
			{
				if(!GetKernel()->GetMenus()->GetMenuRoot()->GetCurrentMenu()->GetOpen())
					GetKernel()->GetMenus()->GetMenuRoot()->GetCurrentMenu()->SetOpen(true);

				return TRUE;
			}
			break;

		case WM_RBUTTONDOWN:
			SetCapture(hWnd);

			if(GetKernel()->GetUserInterface())
				if(GetKernel()->GetUserInterface()->OnEvent(Control::EventRightButtonDown,(WPARAM)&GetKernel()->GetCursorPosition()))
					return true;

			// Send event to menu
			if(GetKernel()->GetGraphics() && GetKernel()->GetMenus()->GetMenuRoot()->GetCurrentMenu() && GetKernel()->GetMenus()->GetMenuRoot()->GetCurrentMenu()->GetOpen())
				return GetKernel()->GetMenus()->GetMenuRoot()->MouseEvent(3,Point(0,0));

			break;

		case WM_RBUTTONDBLCLK:
			SetCapture(hWnd);

			if(GetKernel()->GetUserInterface())
				if(GetKernel()->GetUserInterface()->OnEvent(Control::EventRightButtonDoubleClick,(WPARAM)&GetKernel()->GetCursorPosition()))
					return true;

			break;

		case WM_RBUTTONUP:
			ReleaseCapture();

			if(GetKernel()->GetUserInterface())
				if(GetKernel()->GetUserInterface()->OnEvent(Control::EventRightButtonUp,(WPARAM)&GetKernel()->GetCursorPosition()))
					return true;

			// If we are in the main menu, close it, else go back to the main menu
			if(GetKernel()->GetGraphics() && GetKernel()->GetMenus()->GetMenuRoot()->GetCurrentMenu())
			{
				if(!GetKernel()->GetMenus()->GetMenuRoot()->GetCurrentMenuIndex())
					GetKernel()->GetMenus()->GetMenuRoot()->GetCurrentMenu()->SetOpen(false);
				else	// TODO Just going back to the main menu is good only if we are in a < 3 level menu
					GetKernel()->GetMenus()->GetMenuRoot()->SetCurrentMenu(0);
				
				return TRUE;
			}

			// Send event to menu
			if(GetKernel()->GetGraphics() && GetKernel()->GetMenus()->GetMenuRoot()->GetCurrentMenu() && GetKernel()->GetMenus()->GetMenuRoot()->GetCurrentMenu()->GetOpen())
				return GetKernel()->GetMenus()->GetMenuRoot()->MouseEvent(4,Point(0,0));
			break;

		case WM_MOUSEMOVE:
			// Update mouse position
			delta = GetKernel()->GetCursorPosition();
			GetKernel()->SetCursorPosition(Point(LOWORD(lParam),HIWORD(lParam)));
			delta = GetKernel()->GetCursorPosition() - delta;

			if(GetKernel()->GetUserInterface())
				if(GetKernel()->GetUserInterface()->OnEvent(Control::EventMouseMove,(WPARAM)&GetKernel()->GetCursorPosition()))
					return true;

			// Send event to menu
			if(GetKernel()->GetGraphics() && GetKernel()->GetMenus()->GetMenuRoot()->GetCurrentMenu() && GetKernel()->GetMenus()->GetMenuRoot()->GetCurrentMenu()->GetOpen())
				return GetKernel()->GetMenus()->GetMenuRoot()->MouseEvent(0,delta);
			break;

		case 0x020A:	// WM_MOUSEWHEEL
			// Send event to menu
			if(GetKernel()->GetGraphics() && GetKernel()->GetMenus()->GetMenuRoot()->GetCurrentMenu() && GetKernel()->GetMenus()->GetMenuRoot()->GetCurrentMenu()->GetOpen())
			{
				if((short)HIWORD(wParam) < 0)
					return GetKernel()->GetMenus()->GetMenuRoot()->MouseEvent(5,Point(0,0));
				else if((short)HIWORD(wParam) > 0)
					return GetKernel()->GetMenus()->GetMenuRoot()->MouseEvent(6,Point(0,0));
			}
			break;

		case WM_KEYUP:
			if(GetKernel()->GetUserInterface())
				if(GetKernel()->GetUserInterface()->OnEvent(Control::EventKeyUp,wParam))
					return true;

			break;

		case WM_SYSCHAR:
			switch(wParam)
			{
			case VK_RETURN:
				if(GetKernel()->GetGraphics()->GetDevicePresentParameters().Windowed)
				{
					PresentParameters parameters = GetKernel()->GetGraphics()->GetDevicePresentParameters();

					parameters.Windowed = FALSE;
					
					DisplayMode mode;

					// Get the desktop's display mode
					if(FAILED(GetKernel()->GetGraphics()->GetDirect3D()->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode)))
						return false;

					parameters.BackBufferWidth = GetSystemMetrics(SM_CXSCREEN);
					parameters.BackBufferHeight = GetSystemMetrics(SM_CYSCREEN);

					GetKernel()->GetGraphics()->ResetDevice(parameters);
				}
				else
				{
					PresentParameters parameters = GetKernel()->GetGraphics()->GetDevicePresentParameters();

					parameters.Windowed = TRUE;

					// Set the correct client size
					Rect rect(0,0,WINDOW_WIDTH,WINDOW_HEIGHT);
					AdjustWindowRect((LPRECT)&rect,GetKernel()->GetWindowStyle(),FALSE);
					SetWindowPos(GetKernel()->GetWindowHandle(),HWND_NOTOPMOST,GetSystemMetrics(SM_CXSCREEN)/2 - rect.GetWidth()/2,GetSystemMetrics(SM_CYSCREEN)/2 - rect.GetHeight()/2,rect.GetWidth(),rect.GetHeight(),NULL);
					
					parameters.BackBufferWidth = WINDOW_WIDTH;
					parameters.BackBufferHeight = WINDOW_HEIGHT;

					GetKernel()->GetGraphics()->ResetDevice(parameters);
				}
				return 0;
			}
			break;

		case WM_KEYDOWN:
			if(GetKernel()->GetUserInterface())
				if(GetKernel()->GetUserInterface()->OnEvent(Control::EventKeyDown,wParam))
					return true;

			switch(wParam)
			{
#ifdef _DEBUG
			case VK_ESCAPE:
				PostQuitMessage(0);
				break;
#endif

			case VK_F9:	// Toggle UI
				if(GetKernel()->GetUserInterface())
				{
					GetKernel()->GetUserInterface()->SetVisible(!GetKernel()->GetUserInterface()->GetVisible());
					return true;
				}

				break;

			case VK_F11:	// Toggle UI Enabled
				if(GetKernel()->GetUserInterface())
				{
					GetKernel()->GetUserInterface()->SetEnabled(!GetKernel()->GetUserInterface()->GetEnabled());
					return true;
				}

				break;

			case VK_F8:	// Toggle UI Edit
				if(GetKernel()->GetUserInterface())
				{
					GetKernel()->GetUserInterface()->SetEditorMode(!GetKernel()->GetUserInterface()->GetEditorMode());
					return true;
				}

				break;

			case VK_F1:	// Toggle help text
				if(GetKernel()->GetGraphics())
				{
					GetKernel()->GetGraphics()->SetDrawHelp(!GetKernel()->GetGraphics()->GetDrawHelp());
					return TRUE;
				}
				break;

			case VK_F12: // Toggle about text
				if(GetKernel()->GetGraphics())
				{
					GetKernel()->GetGraphics()->SetDrawAbout(!GetKernel()->GetGraphics()->GetDrawAbout());
					return TRUE;
				}
				break;

			case VK_DELETE:	// Clear output strings
				if(GetKernel()->GetGraphics())
				{
					GetKernel()->GetGraphics()->SuspendOutputStrings();
					return TRUE;
				}
				break;

			case VK_INSERT:	// Toggle peaks
				if(GetKernel()->GetGraphics())
				{
					GetKernel()->GetGraphics()->SetDrawPeaks(!GetKernel()->GetGraphics()->GetDrawPeaks());
					return TRUE;
				}

			case VK_NEXT:	// Next vis
				if(GetKernel()->GetGraphics() && GetKernel()->GetGraphics()->GetVisualizationIndex() < GetKernel()->GetGraphics()->GetVisualizationCount()-1)
				{
					GetKernel()->GetGraphics()->SetVisualization(GetKernel()->GetGraphics()->GetVisualizationIndex()+1);
					return TRUE;
				}

				break;

			case VK_PRIOR:	// Previous vis
				if(GetKernel()->GetGraphics() && GetKernel()->GetGraphics()->GetVisualizationIndex())
				{
					GetKernel()->GetGraphics()->SetVisualization(GetKernel()->GetGraphics()->GetVisualizationIndex()-1);
					return TRUE;
				}
				break;

			case VK_END:	// Next color preset
				if(GetKernel()->GetGraphics() && GetKernel()->GetGraphics()->GetColorPresetIndex() < GetKernel()->GetGraphics()->GetColorPresetCount()-1)
				{
					GetKernel()->GetGraphics()->SetColorPreset(GetKernel()->GetGraphics()->GetColorPresetIndex()+1);
					return TRUE;
				}
				break;

			case VK_HOME:	// Previous color preset
				if(GetKernel()->GetGraphics() && GetKernel()->GetGraphics()->GetColorPresetIndex())
				{
					GetKernel()->GetGraphics()->SetColorPreset(GetKernel()->GetGraphics()->GetColorPresetIndex()-1);
					return TRUE;
				}
				break;

			case VK_UP:
			case VK_DOWN:
				if(GetKernel()->GetGraphics() && GetKernel()->GetMenus()->GetMenuRoot()->GetCurrentMenu() && GetKernel()->GetMenus()->GetMenuRoot()->GetCurrentMenu()->GetOpen())
					return GetKernel()->GetMenus()->GetMenuRoot()->KeyEvent(wParam);

				break;

			case VK_RETURN:
				// Open/confirm menus
				if(GetKernel()->GetGraphics() && GetKernel()->GetMenus()->GetMenuRoot()->GetCurrentMenu())
				{
					if(!GetKernel()->GetMenus()->GetMenuRoot()->GetCurrentMenu()->GetOpen())
						GetKernel()->GetMenus()->GetMenuRoot()->GetCurrentMenu()->SetOpen(true);
					else
						return GetKernel()->GetMenus()->GetMenuRoot()->KeyEvent(VK_RETURN);
					
					return TRUE;
				}
				break;

			case VK_BACK:
				// If we are in the main menu, close it, else go back to the main menu
				if(GetKernel()->GetGraphics() && GetKernel()->GetMenus()->GetMenuRoot()->GetCurrentMenu())
				{
					if(!GetKernel()->GetMenus()->GetMenuRoot()->GetCurrentMenuIndex())
						GetKernel()->GetMenus()->GetMenuRoot()->GetCurrentMenu()->SetOpen(false);
					else	// TODO Just going back to the main menu is good only if we are in a < 3 level menu
						GetKernel()->GetMenus()->GetMenuRoot()->SetCurrentMenu(0);
					
					return TRUE;
				}
				break;
		}
	}

	// Let the default message handler handle all unhandled messages
    return DefWindowProc(hWnd,msg,wParam,lParam);
}