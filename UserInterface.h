#pragma once

#include "Window.h"
#include "Button.h"
#include "CheckBox.h"
#include "ComboBox.h"
#include "StaticText.h"
#include "ScrollBar.h"
#include "TrackBar.h"

//static const Color OuterHighlight1(212.0f / 255.0f,208.0f / 255.0f,200.0f / 255.0f);
//static const Color OuterHighlight2(255.0f / 255.0f,255.0f / 255.0f,255.0f / 255.0f);
//
//static const Color OuterShadow1(64.0f / 255.0f,64.0f / 255.0f,64.0f / 255.0f);
//static const Color OuterShadow2(128.0f / 255.0f,128.0f / 255.0f,128.0f / 255.0f);
//
//static const Color Client1(212.0f / 255.0f,208.0f / 255.0f,200.0f / 255.0f);
//
//static const Color Title1(10.0f / 255.0f,36.0f / 255.0f,106.0f / 255.0f);
//static const Color Title2(166.0f / 255.0f,202.0f / 255.0f,240.0f / 255.0f);
//static const Color InactiveTitle1(128.0f / 255.0f,128.0f / 255.0f,128.0f / 255.0f);
//static const Color InactiveTitle2(192.0f / 255.0f,192.0f / 255.0f,192.0f / 255.0f);
//
//static const Color Text1(255.0f / 255.0f,255.0f / 255.0f,255.0f / 255.0f);
//static const Color Text2(0.0f / 255.0f,0.0f / 255.0f,0.0f / 255.0f);
//static const Color InactiveText1(212.0f / 255.0f,208.0f / 255.0f,200.0f / 255.0f);
//static const Color Hot1(34.0f / 255.0f,101.0f / 255.0f,101.0f / 255.0f);
//static const Color Focused1(0.0f / 255.0f,0.0f / 255.0f,0.0f / 255.0f);
//static const Color Selection1(10.0f / 255.0f,36.0f / 255.0f,106.0f / 255.0f);

class UserInterface : public Window
{
public:
	UserInterface(void);
	~UserInterface(void);

	bool OnRender(void);
	bool OnUpdate(void);

	void DrawBorder(const Rect& rect);
	void DrawCloseSymbol(const Rect& rect,bool pressed,bool enabled,bool focused);
	void DrawMinimizeSymbol(const Rect& rect,bool pressed,bool enabled,bool focused);
	void DrawCheckSymbol(const Rect& rect,bool pressed,bool enabled,bool focused);
	void DrawArrowDownSymbol(const Rect& rect,bool pressed,bool enabled,bool focused);
	void DrawArrowUpSymbol(const Rect& rect,bool pressed,bool enabled,bool focused);
	void DrawWindow(const Rect& rect,bool pressed,bool enabled,bool focused);
	void DrawButton(const Rect& rect,bool pressed,bool enabled,bool focused);
	void DrawComboBox(const Rect& rect,bool pressed,bool enabled,bool focused);
	void DrawButtonText(const Rect& rect,const wchar_t* text,bool pressed,bool enabled,bool focused);
	void DrawCheckBoxText(const Rect& rect,const wchar_t* text,bool pressed,bool enabled,bool focused);
	void DrawComboBoxText(const Rect& rect,const wchar_t* text,bool pressed,bool enabled,bool focused);
	void DrawStaticText(const Rect& rect,const wchar_t* text,bool pressed,bool enabled,bool focused);
	void DrawResizeHandles(const Rect& rect);
	void DrawTrackBar(const Rect& rect,const Rect& track,unsigned long range,unsigned long value,bool pressed,bool enabled,bool focused);

	Color GetTitleColor(void);
	Color GetInactiveTitleColor(void);
	Color GetTextColor(void);
	Color GetInactiveTextColor(void);
	Color GetSelectionColor(void);
	Color GetClientColor(void);
	Color GetBorderColor(void);
	Color GetFocusedColor(void);
	Color GetCursorColor(void);

	WindowHandle NewWindow(const Point& position,const Size& size,const String& title);
	WindowHandle NewWindow(const Point& position,const Size& size,const Size& miSize,const Size& maxSize,const String& title);
	ButtonHandle NewButton(unsigned long id,const Point& position,const Size& size,const String& text);
	CheckBoxHandle NewCheckBox(unsigned long id,const Point& position,const Size& size,const String& text);
	ComboBoxHandle NewComboBox(unsigned long id,const Point& position,const Size& size);
	StaticTextHandle NewStaticText(const Point& position,const Size& size,const String& text);
	ScrollBarHandle NewScrollBar(unsigned long id,const Point& position,const Size& size,unsigned long range,unsigned long count);
	TrackBarHandle NewTrackBar(unsigned long id,const Point& position,const Size& size,unsigned long range,unsigned long value);
};