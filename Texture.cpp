// Copyright 2004/2006 Marko Mihovilic

#include "Globals.h"
#include "Kernel.h"
#include "Texture.h"
#include "Targa.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Texture::Texture(void) :
mCreated(false),
mTexture(NULL),
mBuffer(NULL),
mSize(NULL),
mWidth(NULL),
mHeight(NULL),
mMipmaps(NULL),
mUsage(NULL),
mLevels(NULL)
{
}

Texture::~Texture(void)
{
	Destroy();
}

bool Texture::Create(const wchar_t* path,unsigned long width,unsigned long height,unsigned long mipmaps,Usage usage,Format format,Pool pool)
{
	ASSERT(!mCreated);

	File file;

	if(!file.Open(path,File::AccessModeRead,File::ShareModeRead,File::OpenModeExisting))
	{
		TRACE(L"Texture failed to open file.\n");
		return false;
	}

	unsigned long size = file.GetSize();
	if(!size)
	{
		TRACE(L"Texture file size mismatch.\n");
		return false;
	}

	unsigned char* buffer = new unsigned char[size];
	if(!buffer)
	{
		TRACE(L"Texture failed to allocate file buffer %u bytes.\n",size);
		return false;
	}

	ZeroMemory(buffer,size);

	if(file.Read(buffer,size) != size)
	{
		delete[] buffer;
		TRACE(L"Texture file read mismatch %u bytes.\n",size);
		return false;
	}

	file.Close();

	if(!Create(buffer,size,width,height,mipmaps,usage,format,pool))
	{
		delete[] buffer;
		return false;
	}

	delete[] buffer;

	return true;
}

bool Texture::Create(const wchar_t* id,const wchar_t* group,unsigned long width,unsigned long height,unsigned long mipmaps,Usage usage,Format format,Pool pool)
{
	ASSERT(!mCreated);

	HRSRC resource = FindResource(NULL,id,group);
	if(!resource)
	{
		TRACE(L"Failed to find texture resource.\n");
		return false;
	}

	HGLOBAL global = LoadResource(NULL,resource);
	if(!global)
	{
		TRACE(L"Failed to load texture resource.\n");
		return false;
	}

	LPBYTE data = (LPBYTE)LockResource(global);
	if(!data)
	{
		TRACE(L"Failed to lock texture resource.\n");
		return false;
	}

	if(!Create(data,SizeofResource(NULL,resource),width,height,mipmaps,usage,format,pool))
	{
		UnlockResource(global);
		FreeResource(global);
		return false;
	}

	UnlockResource(global);
	FreeResource(global);

	return true;
}

bool Texture::Create(const unsigned char* data,unsigned long size,unsigned long width,unsigned long height,unsigned long mipmaps,Usage usage,Format format,Pool pool)
{
	ASSERT(!mCreated);

	if(usage == UsageNone)
		mUsage = NULL;

	if(usage == UsageDynamic)
		mUsage = D3DUSAGE_DYNAMIC;

	if(usage == UsageRenderTarget)
		mUsage = D3DUSAGE_RENDERTARGET;

	if(format == FormatUnknown)
		mFormat = D3DFMT_UNKNOWN;

	if(format == FormatA8R8G8B8)
		mFormat = D3DFMT_A8R8G8B8;

	if(format == FormatV8U8)
		mFormat = D3DFMT_V8U8;

	if(format == FormatDXT5)
		mFormat = D3DFMT_DXT5;

	if(pool == PoolDefault)
		mPool = D3DPOOL_DEFAULT;

	if(pool == PoolManaged)
		mPool = D3DPOOL_MANAGED;

	if(mBuffer)
	{
		delete[] mBuffer;
		mBuffer = NULL;
	}

	mSize = size;
	mBuffer = new unsigned char[mSize];
	if(!mBuffer)
	{
		TRACE(L"Failed to allocate local texture data buffer of %u bytes.\n",mSize);
		return false;
	}

	CopyMemory(mBuffer,data,mSize);

	mWidth = width;
	mHeight = height;
	mMipmaps = mipmaps;

	//if(FAILED(GetDynamicD3DX()->D3DXCreateTextureFromFileInMemoryEx(GetKernel()->GetGraphics()->GetDevice(),mBuffer,mSize,mWidth,mHeight,mMipmaps,mUsage,mFormat,mPool,D3DX_DEFAULT,D3DX_DEFAULT,NULL,NULL,NULL,&mTexture)))
	//{
	//	Trace(L"Failed to load texture data.\n");
	//	return false;
	//}

	Targa tga;

	if(!tga.LoadFromMemory(mBuffer,mSize))
	{
		TRACE(L"Failed to load texture.\n");
		return false;
	}

	mWidth =  tga.GetWidth();
	mHeight = tga.GetHeight();

	if(FAILED(GetKernel()->GetGraphics()->GetDevice()->CreateTexture(mWidth,mHeight,mLevels,mUsage|D3DUSAGE_DYNAMIC,mFormat,mPool,&mTexture,NULL)))
	{
		TRACE(L"Failed to create texture.\n");
		return false;
	}
	
	D3DLOCKED_RECT rect;

	if(FAILED(mTexture->LockRect(0,&rect,NULL,D3DLOCK_DISCARD)))
	{
		TRACE(L"Failed to lock texture.\n");
		return false;
	}

	CopyMemory(rect.pBits,tga.GetData(),tga.GetBitsPerPixel()*tga.GetWidth()*tga.GetHeight()/8);

	mTexture->UnlockRect(0);

	if(mPool != D3DPOOL_DEFAULT)
		SAFEDELETE(mBuffer);

	mCreated = true;

	return true;
}

bool Texture::Create(unsigned long width,unsigned long height,unsigned long levels,Usage usage,Format format,Pool pool)
{
	ASSERT(!mCreated);

	if(usage == UsageNone)
		mUsage = NULL;

	if(usage == UsageDynamic)
		mUsage = D3DUSAGE_DYNAMIC;

	if(usage == UsageRenderTarget)
		mUsage = D3DUSAGE_RENDERTARGET;

	if(format == FormatUnknown)
		mFormat = D3DFMT_UNKNOWN;

	if(format == FormatA8R8G8B8)
		mFormat = D3DFMT_A8R8G8B8;

	if(format == FormatV8U8)
		mFormat = D3DFMT_V8U8;

	if(format == FormatDXT5)
		mFormat = D3DFMT_DXT5;

	if(pool == PoolDefault)
		mPool = D3DPOOL_DEFAULT;

	if(pool == PoolManaged)
		mPool = D3DPOOL_MANAGED;

	mWidth = width;
	mHeight = height;
	mLevels = levels;

	if(FAILED(GetKernel()->GetGraphics()->GetDevice()->CreateTexture(mWidth,mHeight,mLevels,mUsage,mFormat,mPool,&mTexture,NULL)))
	{
		TRACE(L"Failed to create texture.\n");
		return false;
	}

	mCreated = true;

	return true;
}

void Texture::Destroy(void)
{
	mCreated = false;

	SAFERELEASE(mTexture);
	SAFEDELETEARRAY(mBuffer);

	mSize = NULL;
	mWidth = NULL;
	mHeight = NULL;
	mMipmaps = NULL;
	mUsage = NULL;
	mLevels = NULL;
}

bool Texture::OnLostDevice(void)
{
	if(mPool == D3DPOOL_DEFAULT)
		SAFERELEASE(mTexture);

	return true;
}

bool Texture::OnResetDevice(void)
{
	if(mPool == D3DPOOL_DEFAULT && mBuffer)
	{
		//if(FAILED(GetDynamicD3DX()->D3DXCreateTextureFromFileInMemoryEx(GetKernel()->GetGraphics()->GetDevice(),mBuffer,mSize,mWidth,mHeight,mMipmaps,mUsage,mFormat,mPool,D3DX_DEFAULT,D3DX_DEFAULT,NULL,NULL,NULL,&mTexture)))
		//{
		//	Trace(L"Failed to recreate texture.\n");
		//	return false;
		//}

		Targa tga;

		if(!tga.LoadFromMemory(mBuffer,mSize))
		{
			TRACE(L"Failed to load texture.\n");
			return false;
		}

		mWidth =  tga.GetWidth();
		mHeight = tga.GetHeight();

		if(FAILED(GetKernel()->GetGraphics()->GetDevice()->CreateTexture(mWidth,mHeight,mLevels,mUsage|D3DUSAGE_DYNAMIC,mFormat,mPool,&mTexture,NULL)))
		{
			TRACE(L"Failed to create texture.\n");
			return false;
		}
		
		D3DLOCKED_RECT rect;

		if(FAILED(mTexture->LockRect(0,&rect,NULL,D3DLOCK_DISCARD)))
		{
			TRACE(L"Failed to lock texture.\n");
			return false;
		}

		CopyMemory(rect.pBits,tga.GetData(),tga.GetBitsPerPixel()*tga.GetWidth()*tga.GetHeight()/8);

		mTexture->UnlockRect(0);
	}

	return true;
}