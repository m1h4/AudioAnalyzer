#include "Globals.h"
#include "Kernel.h"
#include "UserInterface.h"

UserInterface::UserInterface(void)
{
	SetMovable(false);
	SetSizable(false);
	SetCloseVisible(false);
	SetMinimizeVisible(false);
}

UserInterface::~UserInterface(void)
{
}

bool UserInterface::OnRender(void)
{
	return Control::OnRender();
}

bool UserInterface::OnUpdate(void)
{
	Size display(GetKernel()->GetGraphics()->GetDisplayWidth(),GetKernel()->GetGraphics()->GetDisplayHeight());

	if(GetSize() != display)
		SetSize(display);

	return Window::OnUpdate();
}

Color UserInterface::GetTitleColor(void)
{
	return ColorDarken(GetKernel()->GetGraphics()->GetColorPreset()->foreground,0.8f);
}

Color UserInterface::GetInactiveTitleColor(void)
{
	return ColorDarken(GetKernel()->GetGraphics()->GetColorPreset()->foreground,0.4f);
}

Color UserInterface::GetTextColor(void)
{
	return GetKernel()->GetGraphics()->GetColorPreset()->text[0];
}

Color UserInterface::GetInactiveTextColor(void)
{
	return ColorDarken(GetKernel()->GetGraphics()->GetColorPreset()->text[0],0.5f);
}

Color UserInterface::GetSelectionColor(void)
{
	return GetKernel()->GetGraphics()->GetColorPreset()->foreground;
}

Color UserInterface::GetClientColor(void)
{
	return Color(0.2f,ColorLighten(GetKernel()->GetGraphics()->GetColorPreset()->foreground,0.5f));
}

Color UserInterface::GetBorderColor(void)
{
	return GetKernel()->GetGraphics()->GetColorPreset()->foreground;
}

Color UserInterface::GetFocusedColor(void)
{
	return Color(0.2f,ColorLighten(GetKernel()->GetGraphics()->GetColorPreset()->foreground,0.7f));
}

Color UserInterface::GetCursorColor(void)
{
	return GetKernel()->GetGraphics()->GetColorPreset()->peak;
}

void UserInterface::DrawBorder(const Rect& rect)
{
	GetKernel()->GetGraphics()->DrawRect(rect,GetKernel()->GetUserInterface()->GetBorderColor());
}

void UserInterface::DrawCloseSymbol(const Rect& rect,bool pressed,bool enabled,bool focused)
{
	Rect box(rect);

	if(pressed)
		box.Offset(1,1);

	Color color = enabled ? GetKernel()->GetUserInterface()->GetTextColor() : color = GetKernel()->GetUserInterface()->GetInactiveTextColor();

	GetKernel()->GetGraphics()->DrawFillRect(Rect(box.left + 4,box.top + 3,box.left + 4 + 2,box.top + 3 + 1),color);
	GetKernel()->GetGraphics()->DrawFillRect(Rect(box.left + 4 + 6,box.top + 3,box.left + 4 + 2 + 6,box.top + 3 + 1),color);
	GetKernel()->GetGraphics()->DrawFillRect(Rect(box.left + 5,box.top + 4,box.left + 5 + 2,box.top + 4 + 1),color);
	GetKernel()->GetGraphics()->DrawFillRect(Rect(box.left + 5 + 4,box.top + 4,box.left + 5 + 2 + 4,box.top + 4 + 1),color);
	GetKernel()->GetGraphics()->DrawFillRect(Rect(box.left + 6,box.top + 5,box.left + 6 + 4,box.top + 5 + 1),color);
	GetKernel()->GetGraphics()->DrawFillRect(Rect(box.left + 7,box.top + 6,box.left + 7 + 2,box.top + 6 + 1),color);
	GetKernel()->GetGraphics()->DrawFillRect(Rect(box.left + 6,box.top + 7,box.left + 6 + 4,box.top + 7 + 1),color);
	GetKernel()->GetGraphics()->DrawFillRect(Rect(box.left + 5,box.top + 8,box.left + 5 + 2,box.top + 8 + 1),color);
	GetKernel()->GetGraphics()->DrawFillRect(Rect(box.left + 5 + 4,box.top + 8,box.left + 5 + 2 + 4,box.top + 8 + 1),color);
	GetKernel()->GetGraphics()->DrawFillRect(Rect(box.left + 4,box.top + 9,box.left + 4 + 2,box.top + 9 + 1),color);
	GetKernel()->GetGraphics()->DrawFillRect(Rect(box.left + 4 + 6,box.top + 9,box.left + 4 + 2 + 6,box.top + 9 + 1),color);
}

void UserInterface::DrawMinimizeSymbol(const Rect& rect,bool pressed,bool enabled,bool focused)
{
	Rect box(rect);

	if(pressed)
		box.Offset(1,1);

	Color color = enabled ? GetKernel()->GetUserInterface()->GetTextColor() : color = GetKernel()->GetUserInterface()->GetInactiveTextColor();

	GetKernel()->GetGraphics()->DrawFillRect(Rect(box.left + 4,box.top + 9,box.left + 4 + 6,box.top + 9 + 2),color);
}

void UserInterface::DrawCheckSymbol(const Rect& rect,bool pressed,bool enabled,bool focused)
{
	Rect box(rect);

	if(pressed)
		box.Offset(1,1);

	box.Offset(2,2);

	Color color = enabled ? GetKernel()->GetUserInterface()->GetTextColor() : color = GetKernel()->GetUserInterface()->GetInactiveTextColor();
	
	GetKernel()->GetGraphics()->DrawFillRect(Rect(box.left + 1,box.top + 3,box.left + 2,box.top + 6),color);
	GetKernel()->GetGraphics()->DrawFillRect(Rect(box.left + 2,box.top + 4,box.left + 3,box.top + 7),color);
	GetKernel()->GetGraphics()->DrawFillRect(Rect(box.left + 3,box.top + 5,box.left + 4,box.top + 8),color);
	GetKernel()->GetGraphics()->DrawFillRect(Rect(box.left + 4,box.top + 4,box.left + 5,box.top + 7),color);
	GetKernel()->GetGraphics()->DrawFillRect(Rect(box.left + 5,box.top + 3,box.left + 6,box.top + 6),color);
	GetKernel()->GetGraphics()->DrawFillRect(Rect(box.left + 6,box.top + 2,box.left + 7,box.top + 5),color);
	GetKernel()->GetGraphics()->DrawFillRect(Rect(box.left + 7,box.top + 1,box.left + 8,box.top + 4),color);
}

void UserInterface::DrawArrowDownSymbol(const Rect& rect,bool pressed,bool enabled,bool focused)
{
	Rect box(rect);

	if(pressed)
		box.Offset(1,1);

	box.Inflate(-2,-2);

	Color color = enabled ? GetKernel()->GetUserInterface()->GetTextColor() : color = GetKernel()->GetUserInterface()->GetInactiveTextColor();

	GetKernel()->GetGraphics()->DrawFillRect(Rect(box.left + 2,box.top + 4,box.left + 2 + 7,box.top + 5),color);
	GetKernel()->GetGraphics()->DrawFillRect(Rect(box.left + 3,box.top + 5,box.left + 3 + 5,box.top + 6),color);
	GetKernel()->GetGraphics()->DrawFillRect(Rect(box.left + 4,box.top + 6,box.left + 4 + 3,box.top + 7),color);
	GetKernel()->GetGraphics()->DrawFillRect(Rect(box.left + 5,box.top + 7,box.left + 5 + 1,box.top + 8),color);
}

void UserInterface::DrawArrowUpSymbol(const Rect& rect,bool pressed,bool enabled,bool focused)
{
	Rect box(rect);

	if(pressed)
		box.Offset(1,1);

	box.Inflate(-2,-2);

	Color color = enabled ? GetKernel()->GetUserInterface()->GetTextColor() : color = GetKernel()->GetUserInterface()->GetInactiveTextColor();

	GetKernel()->GetGraphics()->DrawFillRect(Rect(box.left + 5,box.top + 4,box.left + 5 + 1,box.top + 5),color);
	GetKernel()->GetGraphics()->DrawFillRect(Rect(box.left + 4,box.top + 5,box.left + 4 + 3,box.top + 6),color);
	GetKernel()->GetGraphics()->DrawFillRect(Rect(box.left + 3,box.top + 6,box.left + 3 + 5,box.top + 7),color);
	GetKernel()->GetGraphics()->DrawFillRect(Rect(box.left + 2,box.top + 7,box.left + 2 + 7,box.top + 8),color);
}

void UserInterface::DrawWindow(const Rect& rect,bool pressed,bool enabled,bool focused)
{
	Rect box(rect);

	GetKernel()->GetGraphics()->DrawRect(box,GetKernel()->GetUserInterface()->GetBorderColor());

	box.Inflate(-1,-1);

	GetKernel()->GetGraphics()->DrawFillRect(box,GetKernel()->GetUserInterface()->GetClientColor());
}

void UserInterface::DrawButton(const Rect& rect,bool pressed,bool enabled,bool focused)
{
	Rect box(rect);

	if(focused)
	{
		GetKernel()->GetGraphics()->DrawFillRect(box,GetKernel()->GetUserInterface()->GetFocusedColor());
		box.Inflate(-1,-1);
	}

	if(pressed)
		box.Inflate(-1,-1);

	GetKernel()->GetGraphics()->DrawRect(box,GetKernel()->GetUserInterface()->GetBorderColor());

	box.Inflate(-1,-1);

	GetKernel()->GetGraphics()->DrawFillRect(box,GetKernel()->GetUserInterface()->GetClientColor());
}

void UserInterface::DrawButtonText(const Rect& rect,const wchar_t* text,bool pressed,bool enabled,bool focused)
{
	Rect box(rect);

	if(pressed)
		box.Offset(1,1);

	Size size;
	GetKernel()->GetGraphics()->GetTextSize(GetKernel()->GetGraphics()->GetUIFont(),text,&size);

	if(enabled)
	{
		GetKernel()->GetGraphics()->DrawText(GetKernel()->GetGraphics()->GetUIFont(),Point(box.left + box.GetWidth() / 2 - size.cx / 2,box.top + box.GetHeight() / 2 - size.cy / 2),text,GetKernel()->GetUserInterface()->GetTextColor());
	}
	else
	{
		GetKernel()->GetGraphics()->DrawText(GetKernel()->GetGraphics()->GetUIFont(),Point(box.left + box.GetWidth() / 2 - size.cx / 2,box.top + box.GetHeight() / 2 - size.cy / 2),text,GetKernel()->GetUserInterface()->GetInactiveTextColor());
	}
}

void UserInterface::DrawCheckBoxText(const Rect& rect,const wchar_t* text,bool pressed,bool enabled,bool focused)
{
	Rect box(rect);

	if(pressed)
		box.Offset(1,1);

	Size size;
	GetKernel()->GetGraphics()->GetTextSize(GetKernel()->GetGraphics()->GetUIFont(),text,&size);

	if(enabled)
	{
		GetKernel()->GetGraphics()->DrawText(GetKernel()->GetGraphics()->GetUIFont(),Point(box.left,box.top + box.GetHeight() / 2 - size.cy / 2),text,GetKernel()->GetUserInterface()->GetTextColor());
	}
	else
	{
		GetKernel()->GetGraphics()->DrawText(GetKernel()->GetGraphics()->GetUIFont(),Point(box.left,box.top + box.GetHeight() / 2 - size.cy / 2),text,GetKernel()->GetUserInterface()->GetInactiveTextColor());
	}
}

void UserInterface::DrawComboBoxText(const Rect& rect,const wchar_t* text,bool pressed,bool enabled,bool focused)
{
	Rect box(rect);

	if(pressed)
		box.Offset(1,1);

	Size size;
	GetKernel()->GetGraphics()->GetTextSize(GetKernel()->GetGraphics()->GetUIFont(),text,&size);

	if(enabled)
	{
		GetKernel()->GetGraphics()->DrawText(GetKernel()->GetGraphics()->GetUIFont(),Point(box.left,box.top + box.GetHeight() / 2 - size.cy / 2),text,GetKernel()->GetUserInterface()->GetTextColor());
	}
	else
	{
		GetKernel()->GetGraphics()->DrawText(GetKernel()->GetGraphics()->GetUIFont(),Point(box.left,box.top + box.GetHeight() / 2 - size.cy / 2),text,GetKernel()->GetUserInterface()->GetInactiveTextColor());
	}
}

void UserInterface::DrawComboBox(const Rect& rect,bool pressed,bool enabled,bool focused)
{
	Rect box(rect);

	GetKernel()->GetGraphics()->DrawRect(box,GetKernel()->GetUserInterface()->GetBorderColor());

	box.Inflate(-1,-1);

	GetKernel()->GetGraphics()->DrawFillRect(box,GetKernel()->GetUserInterface()->GetClientColor());
}

void UserInterface::DrawStaticText(const Rect& rect,const wchar_t* text,bool pressed,bool enabled,bool focused)
{
	Rect box(rect);

	if(pressed)
		box.Offset(1,1);

	if(enabled)
	{

		GetKernel()->GetGraphics()->DrawText(GetKernel()->GetGraphics()->GetUIFont(),Point(box.left,box.top),text,GetKernel()->GetUserInterface()->GetTextColor());
	}
	else
	{
		GetKernel()->GetGraphics()->DrawText(GetKernel()->GetGraphics()->GetUIFont(),Point(box.left,box.top),text,GetKernel()->GetUserInterface()->GetInactiveTextColor());
	}
}

void UserInterface::DrawResizeHandles(const Rect& rect)
{
	static const unsigned long handleSize = 2;

	GetKernel()->GetGraphics()->DrawFillRect(Rect(0,rect.top - 1,GetKernel()->GetGraphics()->GetDisplayWidth(),rect.top),Color(0.0f,1.0f,0.0f));
	GetKernel()->GetGraphics()->DrawFillRect(Rect(0,rect.bottom,GetKernel()->GetGraphics()->GetDisplayWidth(),rect.bottom + 1),Color(0.0f,1.0f,0.0f));

	GetKernel()->GetGraphics()->DrawFillRect(Rect(rect.left - 1,0,rect.left,GetKernel()->GetGraphics()->GetDisplayHeight()),Color(0.0f,1.0f,0.0f));
	GetKernel()->GetGraphics()->DrawFillRect(Rect(rect.right,0,rect.right + 1,GetKernel()->GetGraphics()->GetDisplayHeight()),Color(0.0f,1.0f,0.0f));

	GetKernel()->GetGraphics()->DrawFillRect(Rect(rect.left - handleSize,rect.top - handleSize,rect.left + handleSize,rect.top + handleSize),Color(0.5f,0.5f,0.5f));
	GetKernel()->GetGraphics()->DrawFillRect(Rect(rect.right - handleSize,rect.top - handleSize,rect.right + handleSize,rect.top + handleSize),Color(0.5f,0.5f,0.5f));
	GetKernel()->GetGraphics()->DrawFillRect(Rect(rect.right - handleSize,rect.bottom - handleSize,rect.right + handleSize,rect.bottom + handleSize),Color(0.5f,0.5f,0.5f));
	GetKernel()->GetGraphics()->DrawFillRect(Rect(rect.left - handleSize,rect.bottom - handleSize,rect.left + handleSize,rect.bottom + handleSize),Color(0.5f,0.5f,0.5f));

	GetKernel()->GetGraphics()->DrawRect(Rect(rect.left - handleSize,rect.top - handleSize,rect.left + handleSize,rect.top + handleSize),Color(0.0f,0.0f,0.0f));
	GetKernel()->GetGraphics()->DrawRect(Rect(rect.right - handleSize,rect.top - handleSize,rect.right + handleSize,rect.top + handleSize),Color(0.0f,0.0f,0.0f));
	GetKernel()->GetGraphics()->DrawRect(Rect(rect.right - handleSize,rect.bottom - handleSize,rect.right + handleSize,rect.bottom + handleSize),Color(0.0f,0.0f,0.0f));
	GetKernel()->GetGraphics()->DrawRect(Rect(rect.left - handleSize,rect.bottom - handleSize,rect.left + handleSize,rect.bottom + handleSize),Color(0.0f,0.0f,0.0f));
}

void UserInterface::DrawTrackBar(const Rect& rect,const Rect& track,unsigned long range,unsigned long value,bool pressed,bool enabled,bool focused)
{
	Rect box(rect);

	GetKernel()->GetGraphics()->DrawFillRect(Rect(box.left,box.top + box.GetHeight() / 2,box.right,box.top + box.GetHeight() / 2 + 2),GetKernel()->GetUserInterface()->GetTextColor());

	DrawButton(track,pressed,enabled,focused);
}

WindowHandle UserInterface::NewWindow(const Point& position,const Size& size,const String& title)
{
	WindowHandle window = new Window;
	ASSERT(window);

	window->SetPosition(position);
	window->SetSize(size);
	window->SetTitle(title);

	AttachChild(window);

	return window;
}

WindowHandle UserInterface::NewWindow(const Point& position,const Size& size,const Size& miSize,const Size& maxSize,const String& title)
{
	WindowHandle window = new Window;
	ASSERT(window);

	window->SetPosition(position);
	window->SetSize(size);
	window->SetTitle(title);
	window->SetMinimumSize(miSize);
	window->SetMaximumSize(maxSize);
	window->SetSizable(true);

	AttachChild(window);

	return window;
}

ButtonHandle UserInterface::NewButton(unsigned long id,const Point& position,const Size& size,const String& text)
{
	ButtonHandle button = new Button;
	ASSERT(button);

	button->SetPosition(position);
	button->SetSize(size);
	button->SetText(text);
	button->SetID(id);

	return button;
}

CheckBoxHandle UserInterface::NewCheckBox(unsigned long id,const Point& position,const Size& size,const String& text)
{
	CheckBoxHandle checkbox = new CheckBox;
	ASSERT(checkbox);

	checkbox->SetPosition(position);
	checkbox->SetSize(size);
	checkbox->SetText(text);
	checkbox->SetID(id);

	return checkbox;
}

ComboBoxHandle UserInterface::NewComboBox(unsigned long id,const Point& position,const Size& size)
{
	ComboBoxHandle combobox = new ComboBox;
	ASSERT(combobox);

	combobox->SetPosition(position);
	combobox->SetSize(size);
	combobox->SetID(id);

	return combobox;
}

StaticTextHandle UserInterface::NewStaticText(const Point& position,const Size& size,const String& text)
{
	StaticTextHandle statictext = new StaticText;
	ASSERT(statictext);

	statictext->SetPosition(position);
	statictext->SetSize(size);
	statictext->SetText(text);

	return statictext;
}

ScrollBarHandle UserInterface::NewScrollBar(unsigned long id,const Point& position,const Size& size,unsigned long range,unsigned long count)
{
	ScrollBarHandle scrollbar = new ScrollBar;
	ASSERT(scrollbar);

	scrollbar->SetPosition(position);
	scrollbar->SetSize(size);
	scrollbar->SetTrackCount(count);
	scrollbar->SetTrackRange(range);
	scrollbar->SetID(id);

	return scrollbar;
}

TrackBarHandle UserInterface::NewTrackBar(unsigned long id,const Point& position,const Size& size,unsigned long range,unsigned long value)
{
	TrackBarHandle trackbar = new TrackBar;
	ASSERT(trackbar);

	trackbar->SetPosition(position);
	trackbar->SetSize(size);
	trackbar->SetTrackValue(value);
	trackbar->SetTrackRange(range);
	trackbar->SetID(id);

	return trackbar;
}