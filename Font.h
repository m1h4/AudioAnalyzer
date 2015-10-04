#pragma once

#include "Texture.h"

static const unsigned long FontMagic = MAKEFOURCC('f','d','t',' ');
static const unsigned long FontVersion = 0x00010004;
static const unsigned long FontChars = 127 - 32;

#include <pshpack1.h>

struct FontElement
{
	CHAR code;

	LONG left;
	LONG top;

	LONG advancex;
	LONG advancey;

	USHORT x;
	USHORT y;
	USHORT width;
	USHORT height;
};

struct FontHeader
{
	ULONG	magic;
	ULONG	version;
	ULONG	chars;
	ULONG	height;
	FLOAT			uscale;
	ULONG	sizex;
	ULONG	sizey;
	ULONG	tsize;
	ULONG	ascent;
	ULONG	descent;
	ULONG	outmargt;
	ULONG	outmargb;
	ULONG	outmargl;
	ULONG	outmargr;
	ULONG	inmargt;
	ULONG	inmargb;
	ULONG	inmargl;
	ULONG	inmargr;
};

#include <poppack.h>

class Font
{
public:
	Font(void);
	~Font(void);

	bool Create(const wchar_t* path);
	bool Create(const unsigned char* data,unsigned long size);
	bool Create(const wchar_t* id,const wchar_t* group);
	bool GetCreated(void) const { return mCreated; }
	void Destroy(void);

	void DrawColorText(const Point& point,const wchar_t* string,unsigned long length,const Color* colors) const;
	void DrawText(const Point& point,const wchar_t* string,unsigned long length,const Color& color) const;
	void DrawElement(const Point& point,unsigned long element,const Color& color) const;

	void GetTextRect(const wchar_t* string,unsigned long length,Rect* rect) const;
	void GetTextSize(const wchar_t* string,unsigned long length,Size* size) const;
	void GetTextBlock(const wchar_t* string,unsigned long length,Point* block) const;

	const FontHeader& GetHeader(void) const { return mHeader; }
	const FontElement& GetElement(unsigned long index) const { return mElements[index]; }	// TODO Possible out-of-bounds

	bool OnLostDevice(void);
	bool OnResetDevice(void);

protected:
	bool				mCreated;

	Texture				mTexture;
	FontHeader			mHeader;
	FontElement			mElements[FontChars];
};
