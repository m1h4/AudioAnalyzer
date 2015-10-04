#pragma once

#include <pshpack1.h>

struct TargaHeader
{
	unsigned char	idlength;
	unsigned char	colormaptype;
	unsigned char	datatypecode;
	unsigned short	colormaporigin;
	unsigned short	colormaplength;
	unsigned char	colormapdepth;
	unsigned short	originx;
	unsigned short	originy;
	unsigned short	width;
	unsigned short	height;
	unsigned char	bitsperpixel;
	unsigned char	imagedescriptor;
};

#include <poppack.h>

class Targa
{
public:
	Targa(void) : mData(NULL) {}
	~Targa(void) { delete[] mData; }

public:
	bool LoadFromFile(LPCTSTR path);
	bool LoadFromMemory(const unsigned char* data,unsigned long size);

	unsigned long GetWidth(void) const { return mHeader.width; }
	unsigned long GetHeight(void) const { return mHeader.height; }
	unsigned long GetBitsPerPixel(void) const { return mHeader.bitsperpixel; }
	const unsigned char* GetData(void) const { return mData; }

protected:
	bool Get32(const unsigned char* data,unsigned long size);
	bool Get24(const unsigned char* data,unsigned long size);
	bool Get8(const unsigned char* data,unsigned long size);

protected:
	TargaHeader		mHeader;
	unsigned char*	mData;
};