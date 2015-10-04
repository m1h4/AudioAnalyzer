// Copyright 2004/2006 Marko Mihovilic

#pragma once

#undef CreateWindow

#include "Graphics.h"
#include "Audio.h"
#include "Timer.h"
#include "CommandLine.h"
#include "Menus.h"
#include "UserInterface.h"
#include "Log.h"
#include "Processor.h"
#include "Misc.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 500

#ifdef _DEBUG

//#define TRACE(msg) GetKernel()->MessageLine(__FILE__,__LINE__,msg)
//#define TRACE(msg) GetKernel()->MessageLocation(__FUNCTION__,msg)
#define TRACE GetKernel()->MessageModule

#else

#define TRACE __noop

#endif

class Kernel : public CommandCallbackClass
{
public:
	Kernel(void);
	~Kernel(void);

	bool OnCommandEvent(const ControlHandle& sender,WPARAM wparam,LPARAM lparam);

	bool CreateKernel(void);
	bool GetCreated(void) const { return mCreated; }
	void DestroyKernel(void);

	bool CreateWindow(unsigned long width,unsigned long height,unsigned long style);
	bool ShowWindow(void);
	bool HideWindow(void);
	bool GetWindowVisible(void);
	void DestroyWindow(void);

	void CreateModuleInfo(void);

	int OnRun(void);
	void OnIdle(void);

	bool Dump(const wchar_t* path);

	void SetLed(float bmax);
	void SetLed(bool led);
	bool GetLed(void) const { return mLed; }

	Graphics* GetGraphics(void) const { return mGraphics; }
	Audio* GetAudio(void) const { return mAudio; }
	Menus* GetMenus(void) const { return mMenus; }
	UserInterface* GetUserInterface(void) const { return mUserInterface; }
	Processor* GetProcessor(void) const { return mProcessor; }

	float GetElapsedTime(void) const { return mElapsedTime; }
	double GetAbsoluteTime(void) const { return mAbsoluteTime; }

	bool ReadConfig(const wchar_t* filePath = NULL);
	bool WriteConfig(const wchar_t* filePath = NULL);

	void SetCursorPosition(const Point& position);	// To be only called by the window procedure
	void SetCursorPositionTime(double time) { mCursorPositionTime = time; }	// To be only called by the window procedure
	const Point& GetCursorPosition(void) const { return mCursorPosition; }
	double GetCursorPositionTime(void) const { return mCursorPositionTime; }

	const String& GetModulePath(void) const { return mModulePath; }
	const String& GetModuleName(void) const { return mModuleName; }

	String GetCombinedPath(const wchar_t* path1,const wchar_t* path2);
	String GetStripedPath(const wchar_t* path);
	String GetCombinedModulePath(const wchar_t* path) { return GetCombinedPath(GetModulePath(),path); }

	HWND GetWindowHandle(void) const { return mWindowHandle; }
	WNDCLASSEX GetWindowClass(void) const { return mWindowClass; }
	DWORD GetWindowStyle(void) const { return mWindowStyle; }
	
	void MessageFormat(const wchar_t* fmt,...);
	void MessageLine(const char* file,unsigned long line,const wchar_t* fmt,...);
	void MessageLocation(const char* location,const wchar_t* fmt,...);
	void MessageModule(const wchar_t* fmt,...);
	void MessageErrorLast(const wchar_t* section,const wchar_t* title);
	void MessageError(const wchar_t* message,const wchar_t* title);

	void SetLastError(const wchar_t* fmt,...);
	const String& GetLastError(void) const { return mLastError; }
	
	static LRESULT CALLBACK WindowProcedure(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);

protected:
	bool				mCreated;

	HWND				mWindowHandle;
	WNDCLASSEX			mWindowClass;
	DWORD				mWindowStyle;

	Log					mLog;

	bool				mLed;				// Should we use leds

	Graphics*			mGraphics;
	Audio*				mAudio;
	Menus*				mMenus;
	UserInterface*		mUserInterface;
	Processor*			mProcessor;

	String				mModulePath;		// Path to the exe
	String				mModuleName;		// Name of exe
	String				mLastError;

	Point				mCursorPosition;	// The last cursor position
	double				mCursorPositionTime;

	float				mElapsedTime;		// Our global speed factor for moving objects
	double				mAbsoluteTime;		// Abstolute app time
};

DECLARE_GLOBAL_INSTANCE(Kernel);
DECLARE_GLOBAL_INSTANCE(CommandLineData);
DECLARE_GLOBAL_INSTANCE(Timer);