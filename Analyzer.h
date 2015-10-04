#pragma once

#define EXPANDA1(x) #x
#define TOSTRINGA(x) EXPANDA1(x)
#define EXPANDW2(x) L##x
#define EXPANDW1(x) EXPANDW2(x)
#define TOSTRINGW(x) EXPANDW1(TOSTRINGA(x))

#ifdef _UNICODE
#define TOSTRING TOSTRINGW
#else
#define TOSTRING TOSTRINGA
#endif

#define ANALYZER_VERSION_MAJOR 2
#define ANALYZER_VERSION_MINOR 7

#define ANALYZER_CLASSNAME L"AudioAnalyzer"

#ifdef _DEBUG
#define ANALYZER_MUTEXSTRING L"Audio Analyzer Debug 856379857515467257473294"
#else
#define ANALYZER_MUTEXSTRING L"Audio Analyzer 856379857515467257473294"
#endif

#ifdef _DEBUG
#define ANALYZER_WINDOWTITLE L"Audio Analyzer [Debug]"
#else
#define ANALYZER_WINDOWTITLE L"Audio Analyzer"
#endif

#define ANALYZER_CONFIGVERSION L"1.0"
#define ANALYZER_PRESETSVERSION L"1.0"

#define ANALYZER_CHANNELS 2					// 2 default
#define ANALYZER_FREQUENCY 44100			// 44100 default
#define ANALYZER_BITS 16					// 16 default
#define ANALYZER_BUFFERSIZE 16384			// 4096 default
#define ANALYZER_FFTBUFFERSIZE 1024			// 1024 default

#define ANALYZER_NUMBANDS 20				// 20 default

#define ANALYZER_LINEANTIALIAS TRUE			// TRUE default

#define ANALYZER_FRAMERATE 30.0f		// 30.0f default

// Flags
#define ANALYZER_WINDOWED		true		// Should we display in a window
#define ANALYZER_VSYNC			true		// Should we display with vertical sync
#define ANALYZER_FSAA			false		// Should we display with fullscreen antialiasing
#define ANALYZER_HIGHPRIORITY	false		// Should we set the app's priority to a higher level
