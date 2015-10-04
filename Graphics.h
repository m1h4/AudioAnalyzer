// Copyright 2004/2006 Marko Mihovilic

#pragma once

#include "Camera.h"
#include "Visualization.h"
#include "Font.h"
#include "Texture.h"
#include "StateBlock.h"
#include "Transition.h"

#define SPRITE_CACHE 32768
#define SPRITE_LINE_CACHE 4096
#define MODEL_CACHE 32768

struct SpriteVertex
{
	Vector4 position;
	unsigned long color;
	Vector2 texture;
};

static const unsigned long SpriteVertexFormat = D3DFVF_XYZRHW|D3DFVF_TEX1|D3DFVF_DIFFUSE;

// Our structure for the color presets
struct ColorPreset
{
	Color		foreground;		// Foreground color
	Color		background;		// Background color
	Color		text[3];		// Text color
	Color		peak;			// Peak color

	Color		backgroundtl;	// Top left corner of background color
	Color		backgroundtr;	// Top right corner of background color
	Color		backgroundbr;	// Bottom right corner of background color
	Color		backgroundbl;	// Bottom left corner of background color

	String		name;			// Name of preset
};

struct OutputString
{
	String	text;
	double time;
};

class Graphics
{
public:
	Graphics();
	~Graphics();

	// DrawRect flags
	enum RectFlag
	{
		RectFlagTop = 1,
		RectFlagLeft = 2,
		RectFlagRight = 4,
		RectFlagBottom = 8,
		RectFlagAll = 15,
	};

	// DrawTextureTransition flags
	enum TransitionEffect
	{
		TransitionEffectTVHorizontal,
		TransitionEffectTVVertical,
		TransitionEffectTVCenter,
		TransitionEffectStretchLeft,
		TransitionEffectStretchTop,
		TransitionEffectStretchRight,
		TransitionEffectStretchBottom,
	};

	bool CreateGraphics(void);
	bool GetCreated(void) const { return mCreated; }
	bool GetCreatedFonts(void) const { return mCreatedFonts; }
	bool GetCreatedRenderTargets(void) const { return mCreatedRenderTargets; }
	bool GetCreatedTextures(void) const { return mCreatedTextures; }
	bool GetCreatedSprites(void) const { return mCreatedSprites; }
	bool CreateTextures(void);
	bool CreateRenderTargets(void);
	bool CreateFonts(void);
	bool CreateSprites(void);
	void DestroyGraphics(void);
	void DestroyTextures(void);
	void DestroyRenderTargets(void);
	void DestroyFonts(void);
	void DestroySprites(void);

	bool ValidateDevice(void);

	void Render(void);

	bool SpriteBegin(void);
	void SpriteEnd(void);
	bool SpriteFlush(void);

	bool SpriteWrite(const Vector2& topleft,const Vector2& topright,const Vector2& bottomright,const Vector2& bottomleft,const Vector2& ttopleft,const Vector2& tbottomright,const Color& ctopleft,const Color& ctopright,const Color& cbottomright,const Color& cbottomleft,IDirect3DTexture9* texture);
	bool SpriteWrite(const Vector2& topleft,const Vector2& topright,const Vector2& bottomright,const Vector2& bottomleft,const Vector2& ttopleft,const Vector2& tbottomright,const Color& ctopleft,const Color& ctopright,const Color& cbottomright,const Color& cbottomleft,const Vector2& center,float angle,IDirect3DTexture9* texture);

	bool InvalidateDeviceObjects(void);
	bool RestoreDeviceObjects(void);
	bool ResetDevice(const PresentParameters& params);

	bool GetClipper(Rect* rect);
	void SetClipper(const Rect* rect);

	bool SetGamma(float contrast,float brightness,float gamma);
	bool RestoreGamma(void);

	void DrawShadowText(const Font& font,const Point& point,const wchar_t* text,const Color& color);
	void DrawText(const Font& font,const Point& point,const wchar_t* text,const Color& color);
	
	void DrawShadowColorText(const Font& font,const Point& point,const wchar_t* text,const Color* colors);
	void DrawColorText(const Font& font,const Point& point,const wchar_t* text,const Color* colors);

	void DrawPrintText(const Font& font,const Point& point,const wchar_t* text,float delta,const Color& color);
	void DrawPrintShadowText(const Font& font,const Point& point,const wchar_t* text,float delta,const Color& color);
	void DrawPrintShadowColorText(const Font& font,const Point& point,const wchar_t* text,float delta,const Color* colors);

	void DrawTexture(const Texture& texture,const Point& point,const Color& color);
	void DrawTexture(const Texture& texture,const Rect& rect,const Color& color);
	void DrawTexture(const Texture& texture,const Point& point,float angle,const Color& color);
	void DrawTexture(const Texture& texture,const Rect& rect,float angle,const Color& color);
	void DrawTexture(const Texture& texture,const Point& point,const Point& center,float angle,const Color& color);
	void DrawTexture(const Texture& texture,const Rect& rect,const Point& center,float angle,const Color& color);

	void DrawTextureTransition(const Texture& texture,const Point& point,const Color& color,Transition* effect,double time);
	void DrawTextureTransition(const Texture& texture,const Rect& rect,const Color& color,Transition* effect,double time);

	void DrawFillRect(const Rect& rect,const Color& colorTl,const Color& colorTr,const Color& colorBr,const Color& colorBl);
	void DrawFillRect(const Rect& rect,const Color& color);
	void DrawShadowFillRect(const Rect& rect,const Color& color);
	
	void DrawRect(const Rect& rect,const Color& color,unsigned char flags = RectFlagAll) { this->DrawRect(rect,1.0f,color,flags); };
	void DrawRect(const Rect& rect,float width,const Color& color,unsigned char flags = RectFlagAll);
	void DrawShadowRect(const Rect& rect,const Color& color,unsigned char flags = RectFlagAll) { this->DrawShadowRect(rect,1.0f,color,flags); }
	void DrawShadowRect(const Rect& rect,float width,const Color& color,unsigned char flags = RectFlagAll);

	void DrawCornerRect(const Rect& rect,float size,const Color& color,unsigned char flags = RectFlagAll) { this->DrawCornerRect(rect,size,1.0f,color,flags); }
	void DrawCornerRect(const Rect& rect,float size,float width,const Color& color,unsigned char flags = RectFlagAll);
	void DrawShadowCornerRect(const Rect& rect,float size,const Color& color,unsigned char flags = RectFlagAll) { this->DrawShadowCornerRect(rect,size,1.0f,color,flags); }
	void DrawShadowCornerRect(const Rect& rect,float size,float width,const Color& color,unsigned char flags = RectFlagAll);

	void DrawCenterRect(const Rect& rect,float size,const Color& color,unsigned char flags = RectFlagAll) { this->DrawCenterRect(rect,size,1.0f,color,flags); }
	void DrawCenterRect(const Rect& rect,float size,float width,const Color& color,unsigned char flags = RectFlagAll);
	void DrawShadowCenterRect(const Rect& rect,float size,const Color& color,unsigned char flags = RectFlagAll) { this->DrawShadowCenterRect(rect,size,1.0f,color,flags); }
	void DrawShadowCenterRect(const Rect& rect,float size,float width,const Color& color,unsigned char flags = RectFlagAll);

	void DrawLine(const Vector2& start,const Vector2& end,const Color& color,float width);
	void DrawLines(const Vector2* lines,unsigned long count,const Color& color,float width);
	
	void GetTextRect(const Font& font,const wchar_t* text,Rect* rect);
	void GetTextSize(const Font& font,const wchar_t* text,Size* size);
	void GetTextBlock(const Font& font,const wchar_t* text,Point* block);

	void DrawStatusText(void);
	void DrawOutputStrings(void);
	void DrawHelp(void);
	void DrawMenus(void);
	void DrawAbout(void);
	void DrawLoading(void);
	void DrawCursor(void);

	bool EnableAnisotropic(void);

	bool ReadColorPresets(const wchar_t* filePath = NULL);

	bool AddColorPreset(ColorPreset* preset);
	bool SetColorPreset(const wchar_t* name) { for(unsigned long i = 0; i < mColorPresets.GetSize(); ++i) { if(mColorPresets[i]->name == name) return SetColorPreset(i); } return false; }
	bool SetColorPreset(unsigned long index);
	unsigned long GetColorPresetIndex(void) const { return mColorIndex; }
	const wchar_t* GetColorPresetName(unsigned long index) { if(index >= mColorPresets.GetSize()) return NULL; return mColorPresets[index]->name; }
	ColorPreset* GetColorPreset(void) { return &mColor; }
	ColorPreset* GetColorPreset(unsigned long index) { if(index >= mColorPresets.GetSize()) return NULL; return mColorPresets[index]; }
	unsigned long GetColorPresetCount(void) const { return mColorPresets.GetSize(); }

	bool AddVisualization(Visualization* vis);
	unsigned long GetVisualizationIndex(void) const { return mVisualizationIndex; }
	const wchar_t* GetVisualizationName(unsigned long index) { if(index >= mVisualizations.GetSize()) return NULL; return mVisualizations[index]->GetName(); }
	bool SetVisualization(const wchar_t* name) { for(unsigned long i = 0; i < mVisualizations.GetSize(); ++i) { if(mVisualizations[i]->GetName() == name) return SetVisualization(i); } return false; }
	bool SetVisualization(unsigned long index);
	unsigned long GetVisualizationCount(void) const { return mVisualizations.GetSize(); }

	bool AddTransition(Transition* trans);
	unsigned long GetTransitionIndex(void) const { return mTransitionIndex; }
	const wchar_t* GetTransitionName(unsigned long index) { if(index >= mTransitions.GetSize()) return NULL; return mTransitions[index]->GetName(); }
	bool SetTransition(unsigned long index);
	bool SetTransition(const wchar_t* name) { for(unsigned long i = 0; i < mTransitions.GetSize(); ++i) { if(mTransitions[i]->GetName() == name) return SetTransition(i); } return false; }
	unsigned long GetTransitionCount(void) const { return mTransitions.GetSize(); }

	void PrintOutputString(const wchar_t* fmt,...);
	unsigned long GetOutputStringCount(void) const { return mOutputStrings.GetSize(); }
	OutputString* GetOutputString(unsigned long index) { if(index >= mOutputStrings.GetSize()) return NULL; return mOutputStrings[index]; }
	void ClearOutputStrings(void) { if(mOutputStrings.GetSize()) mOutputStrings.Delete(); }
	void SuspendOutputStrings(void);

	bool GetFullscreen(void) const { return !mPresentParameters.Windowed; }

	const Texture& GetBandTexture(unsigned long index) { return mBandTextures[index]; }	// TODO Out-of-bounds check
	const Texture& GetDailTexture(unsigned long index) const { return mDailTextures[index]; }
	const Texture& GetBeatTexture(unsigned long index) const { return mBeatTextures[index]; }

	const Font& GetLargeBoldFont(void) const { return mFontLargeBold; }
	const Font& GetBoldFont(void) const { return mFontBold; }
	const Font& GetUIFont(void) const { return mFontUI; }

	long GetFontHeight(const Font& font) { return font.GetHeader().height; }

	void SetDrawHelp(bool draw);
	bool GetDrawHelp(void) const { return mDrawHelp; }

	void SetDrawOutputStrings(bool draw);
	bool GetDrawOutputStrings(void) const { return mDrawOutputStrings; }

	void SetDrawVisualizationName(bool draw);
	bool GetDrawVisualizationName(void) const { return mDrawVisualizationName; }

	void SetDrawTimer(bool draw);
	bool GetDrawTimer(void) const { return mDrawTimer; }

	void SetDrawFps(bool draw);
	bool GetDrawFps(void) const { return mDrawFps; }

	void SetDrawAbout(bool draw);
	bool GetDrawAbout(void) const { return mDrawAbout; }

	void SetDrawCursor(bool draw);
	bool GetDrawCursor(void) const { return mDrawCursor; }

	void SetDrawPeaks(bool draw);
	bool GetDrawPeaks(void) const { return mDrawPeaks; }
	double GetDrawPeaksTime(void) const { return mDrawPeaksTime; }

	void SetAutoHideCursorTime(double time) { mAutoHideCursorTime = time; }
	double GetAutoHideCursorTime(void) const { return mAutoHideCursorTime; }

	double GetFps(void) const { return mFps; }

	unsigned long GetDisplayWidth(void) { return mPresentParameters.BackBufferWidth; }
	unsigned long GetDisplayHeight(void) { return mPresentParameters.BackBufferHeight; }

	const DeviceCaps& GetDeviceCaps(void) const { return mCaps; }
	const DeviceIdentifier& GetDeviceIdentifier(void) const { return mIdentifier; }
	const PresentParameters& GetDevicePresentParameters(void) const { return mPresentParameters; }
	const DisplayMode& GetDesktopDisplayMode(void) const { return mDisplayMode; }

	IDirect3D9* GetDirect3D(void) const { return mDirect3D; }
	IDirect3DDevice9* GetDevice(void) const { return mDevice; }

protected:
	bool mCreated;
	bool mCreatedTextures;
	bool mCreatedFonts;
	bool mCreatedRenderTargets;
	bool mCreatedSprites;

	// Our interfaces
	IDirect3D9*					mDirect3D;			// Our main direct3d9 object
	IDirect3DDevice9*			mDevice;			// Our primary direct3d9 device

	IDirect3DVertexBuffer9*		mSpriteBuffer;
	IDirect3DTexture9*			mSpriteTexture;
	IDirect3DTexture9*			mLineTexture;
	unsigned long				mSpriteCount;
	SpriteVertex*				mSpriteData;
	
	Texture						mBandTextures[4];	// Our spectrum analyzer's bar textures
	Texture						mCursor[1];			// The cursor texture
	Texture						mScenes[2];			// The scene render targets
	Texture						mDailTextures[2];	// The dail texture
	Texture						mBeatTextures[2];

	Font						mFontBold;
	Font						mFontUI;
	Font						mFontLargeBold;

	float						mFps;					// Frames per second

	PresentParameters			mPresentParameters;		// The current display mode
	DeviceCaps					mCaps;
	DeviceIdentifier			mIdentifier;
	DisplayMode					mDisplayMode;

	Array<ColorPreset*>			mColorPresets;			// Array of loaded color presets
	unsigned long				mColorIndex;			// Current color preset index in color preset array
	double						mColorTime;				// Absolute time when color was last changed
	ColorPreset					mColor;					// Current color preset

	bool						mDrawVisualizationName;
	double						mDrawVisualizationNameTime;
	bool						mDrawTimer;
	double						mDrawTimerTime;
	bool						mDrawFps;
	double						mDrawFpsTime;
	bool						mDrawPeaks;			// Should we draw the peaks
	double						mDrawPeaksTime;
	bool						mDrawOutputStrings;	// Should we draw the outputed strings
	double						mDrawOutputStringsTime;
	bool						mDrawHelp;
	double						mDrawHelpTime;
	bool						mDrawAbout;
	double						mDrawAboutTime;

	bool						mDrawCursor;
	double						mDrawCursorTime;
	double						mAutoHideCursorTime;

	Array<Transition*>			mTransitions;
	unsigned long				mTransitionIndex;
	unsigned long				mTransitionScene;
	double						mTransitionTime;

	Array<Visualization*>		mVisualizations;
	unsigned long				mVisualizationIndex;	// The currently selected visualization ID

	Array<OutputString*>		mOutputStrings;
};
