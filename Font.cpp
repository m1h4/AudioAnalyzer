// Copyright 2004/2006 Marko Mihovilic

#include "Globals.h"
#include "Kernel.h"
#include "Font.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Font::Font(void) :
mCreated(false)
{
	ZeroMemory(&mHeader,sizeof(mHeader));
	ZeroMemory(mElements,sizeof(mElements));
}

Font::~Font(void)
{
	Destroy();
}

bool Font::Create(const unsigned char* data,unsigned long size)
{
	ASSERT(!mCreated);

	if(size < sizeof(mHeader))
	{
		TRACE(L"Font data size smaller than font header.\n");
		return false;
	}

	CopyMemory(&mHeader,data,sizeof(mHeader));

	data += sizeof(mHeader);
	size -= sizeof(mHeader);

	if(mHeader.magic != FontMagic)
	{
		TRACE(L"Font header magic mismatch.\n");
		return false;
	}

	if(mHeader.version != FontVersion)
	{
		TRACE(L"Font header version mismatch.\n");
		return false;
	}

	if(!mHeader.tsize)
	{
		TRACE(L"Font has no texture.\n");
		return false;
	}

	ZeroMemory(mElements,sizeof(mElements));

	if(size < sizeof(mElements[0]) * mHeader.chars)
	{
		TRACE(L"Font header char count mismatch.\n");
		return false;
	}

	for(unsigned long i = 0; i < mHeader.chars; ++i)
	{
		CopyMemory(&mElements[i],data,sizeof(mElements[i]));

		data += sizeof(mElements[i]);
		size -= sizeof(mElements[i]);
	}

	if(size < mHeader.tsize)
	{
		TRACE(L"Font header texture size mismatch.\n");
		return false;
	}

	if(!mTexture.Create(data,mHeader.tsize,NULL,NULL,NULL,Texture::UsageNone,Texture::FormatA8R8G8B8,Texture::PoolDefault))
		return false;

	mCreated = true;

	return true;
}

bool Font::Create(const wchar_t* path)
{
	ASSERT(!mCreated);

	File file;

	if(!file.Open(path,File::AccessModeRead,File::ShareModeRead,File::OpenModeExisting))
	{
		TRACE(L"Font file cannot be opened.\n");
		return false;
	}

	unsigned long size = file.GetSize();
	if(!size)
	{
		TRACE(L"Font file size mismatch.\n");
		return false;
	}

	unsigned char* buffer = new unsigned char[size];
	if(!buffer)
	{
		TRACE(L"Font file buffer failed to allocate %u bytes.\n",size);
		return false;
	}

	ZeroMemory(buffer,size);

	if(file.Read(buffer,size) != size)
	{
		delete[] buffer;
		TRACE(L"Font read %u bytes mismatch.\n",size);
		return false;
	}

	file.Close();

	if(!Create(buffer,size))
	{
		delete[] buffer;
		return false;
	}

	delete[] buffer;

	return true;
}

bool Font::Create(const wchar_t* id,const wchar_t* group)
{
	ASSERT(!mCreated);

	HRSRC resource = FindResource(NULL,id,group);
	if(!resource)
	{
		TRACE(L"Failed to find font resource.\n");
		return false;
	}

	HGLOBAL global = LoadResource(NULL,resource);
	if(!global)
	{
		TRACE(L"Failed to load font resource.\n");
		return false;
	}

	LPBYTE data = (LPBYTE)LockResource(global);
	if(!data)
	{
		TRACE(L"Failed to lock font resource.\n");
		return false;
	}

	if(!Create(data,SizeofResource(NULL,resource)))
	{
		UnlockResource(global);
		FreeResource(global);
		return false;
	}

	UnlockResource(global);
	FreeResource(global);

	return true;
}

void Font::Destroy(void)
{
	mCreated = false;

	mTexture.Destroy();

	ZeroMemory(&mHeader,sizeof(mHeader));
	ZeroMemory(mElements,sizeof(mElements));
}

void Font::DrawColorText(const Point& point,const wchar_t* string,unsigned long length,const Color* colors) const
{
	if(!length)
		length = (unsigned int)wcslen(string);

	long x = point.x;
	long y = point.y + mHeader.ascent;

	unsigned int colorIndex = 0;
	unsigned int ch;

	for(unsigned int i = 0; i < length; ++i)
	{
		ch = *(unsigned char*)&string[i];

		if(ch < L'\n')
			colorIndex = ch - 1;

		else if(ch == L'\n')
		{
			x = point.x;
			y += mHeader.height;
		}

		if(ch < 32)
			continue;

		ch -= 32;

		DrawElement(Point(x,y),ch,colors[colorIndex]);

		x += mElements[ch].advancex;
	}
}

void Font::DrawText(const Point& point,const wchar_t* string,unsigned long length,const Color& color) const
{
	if(!length)
		length = (unsigned int)wcslen(string);

	long x = point.x;
	long y = point.y + mHeader.ascent;

	unsigned int ch;

	for(unsigned int i = 0; i < length; ++i)
	{
		ch = *(unsigned char*)&string[i];

		if(ch == L'\n')
		{
			x = point.x;
			y += mHeader.height;
		}

		if(ch < 32)
			continue;

		ch -= 32;

		DrawElement(Point(x,y),ch,color);

		x += mElements[ch].advancex;
	}
}

void Font::DrawElement(const Point& point,unsigned long element,const Color& color) const
{
	// Don't draw if fully transparent
	if(color.a <= 0.0f)
		return;

	Rect rect(mElements[element].x - (long)mHeader.outmargl,mElements[element].y - (long)mHeader.outmargt,mElements[element].x + mElements[element].width + mHeader.outmargr,mElements[element].y + mElements[element].height + mHeader.outmargb);

	D3DSURFACE_DESC desc;
	mTexture.GetTexture()->GetLevelDesc(0,&desc);

	Vector2 topleft,topright,bottomright,bottomleft;

	topleft.x = bottomleft.x = (float)point.x - (float)mHeader.outmargl*mHeader.uscale + (float)mElements[element].left*mHeader.uscale;
	topleft.y = topright.y = (float)point.y - (float)mHeader.outmargt*mHeader.uscale + (float)mElements[element].top*mHeader.uscale;
	topright.x = bottomright.x = topleft.x + (rect.right - rect.left) * mHeader.uscale;
	bottomleft.y = bottomright.y = topleft.y + (rect.bottom - rect.top) * mHeader.uscale;

	GetKernel()->GetGraphics()->SpriteWrite(topleft,topright,bottomright,bottomleft,Vector2(rect.left / (float)desc.Width,rect.top / (float)desc.Height),Vector2(rect.right / (float)desc.Width,rect.bottom / (float)desc.Height),color,color,color,color,mTexture.GetTexture());
}

void Font::GetTextRect(const wchar_t* string,unsigned long length,Rect* rect) const
{
	ASSERT(rect);

	Size size;
	GetTextSize(string,length,&size);

	rect->right = rect->left + size.cx;
	rect->bottom = rect->top + size.cy;
}

void Font::GetTextSize(const wchar_t* string,unsigned long length,Size* size) const
{
	ASSERT(size);

	if(!length)
		length = (unsigned int)wcslen(string);

	long x = 0;
	long y = 0;

	unsigned int ch;

	size->cx = x;
	size->cy = y;

	for(unsigned int i = 0; i < length; ++i)
	{
		ch = *(unsigned char*)&string[i];

		if(ch == L'\n')
		{
			x = 0;
			y += mHeader.height;
		}

		if(ch < 32)
			continue;

		ch -= 32;

		if(i == length-1)
			x += mElements[ch].left + mElements[ch].width;
		else
			x += mElements[ch].advancex;
		
		size->cx = max(size->cx,x);
	}

	size->cy = y + mHeader.height;
}

void Font::GetTextBlock(const wchar_t* string,unsigned long length,Point* block) const
{
	ASSERT(block);

	if(!length)
		length = (unsigned int)wcslen(string);

	block->x = 0;
	block->y = 0;

	unsigned int ch;

	for(unsigned int i = 0; i < length; ++i)
	{
		ch = *(unsigned char*)&string[i];

		if(ch == L'\n')
		{
			block->x = 0;
			block->y += mHeader.height;
		}

		if(ch < 32)
			continue;

		ch -= 32;

		if(i == length-1)
			block->x += mElements[ch].left + mElements[ch].width;
		else
			block->x += mElements[ch].advancex;
	}
}

bool Font::OnLostDevice(void)
{
	mTexture.OnLostDevice();

	return true;
}

bool Font::OnResetDevice(void)
{
	mTexture.OnResetDevice();

	return true;
}