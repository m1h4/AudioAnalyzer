// Copyright 2004/2006 Marko Mihovilic

#pragma once

#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NON_CONFORMING_SWPRINTFS

#pragma warning(disable : 4244)	// Conversions (double to float etc.)
#pragma warning(disable : 4125) // Decimal digit terminates octal escape sequence

#define STRICT
#define DIRECTINPUT_VERSION 0x0800

#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <basetsd.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <tchar.h>
#include <xmmintrin.h>

//#include <winable.h>	// Keyboard LED control
#include <time.h>		// clock

#include <shlwapi.h>

//#include <dbghelp.h>

#include <d3d9.h>
#include <dsound.h>

#include <new.h>

#ifdef _DEBUG
#include <crtdbg.h>
#include <assert.h>
#define DEBUG_NEW new(_CLIENT_BLOCK,__FILE__, __LINE__)
#define ASSERT assert
#else
#define ASSERT __noop
#endif

#define ALIGN_DATA

#define SAFEDELETE(ptr) { if(ptr) { delete ptr; ptr = NULL; } }
#define SAFEDELETEARRAY(ptr) { if(ptr) { delete[] ptr; ptr = NULL; } }
#define SAFERELEASE(ptr) { if(ptr) { ptr->Release(); ptr = NULL; } }

#define PresentParameters D3DPRESENT_PARAMETERS
#define DeviceCaps D3DCAPS9
#define DeviceIdentifier D3DADAPTER_IDENTIFIER9
#define DisplayMode D3DDISPLAYMODE

#include "Analyzer.h"

#include "String.h"
#include "Rect.h"
#include "Size.h"
#include "Point.h"
#include "File.h"
#include "CriticalSection.h"
#include "Math.h"
#include "Line.h"
#include "Plane.h"
#include "AABB.h"
#include "Vector.h"
#include "Matrix.h"
#include "Color.h"
#include "Array.h"

// Libraries
#pragma comment (lib, "shlwapi.lib")	/* link with Windows Shell API lib */
#pragma comment (lib, "winmm.lib")		/* link with Windows MultiMedia lib */
#pragma comment (lib, "dxguid.lib")		/* link with DirectX GUID lib */
//#pragma comment (lib, "version.lib")	/* link with Version lib */
//#pragma comment (lib, "dinput8.lib")	/* link with DirectInput 8 lib */
//#pragma comment (lib, "dsound.lib")		/* link with DirectSound 8 lib */
//#pragma comment (lib, "ddraw.lib")		/* link with DirectDraw lib */
//#pragma comment (lib, "dxerr9.lib")		/* link with DirectX Helper lib */
//#pragma comment (lib, "d3d9.lib")		/* link with Direct3D 9 lib */

//#pragma comment (lib, "comctl32.lib")		/* link with Common Controls lib */

// Use debug libraries where possible
//#ifdef _DEBUG
//	#pragma comment (lib, "d3dx9d.lib")		/* link with DirectX Extension lib */
//#else
//	#pragma comment (lib, "d3dx9.lib")		/* link with DirectX Extension lib */
//#endif

#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif