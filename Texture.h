#pragma once

class Texture
{
public:
	Texture(void);
	~Texture(void);

	enum Usage
	{
		UsageNone,
		UsageDynamic,
		UsageRenderTarget,
	};

	enum Format
	{
		FormatUnknown,
		FormatA8R8G8B8,
		FormatV8U8,
		FormatDXT5,
	};

	enum Pool
	{
		PoolDefault,
		PoolManaged,
	};

	bool Create(const wchar_t* path,unsigned long width,unsigned long height,unsigned long mipmaps,Usage usage,Format format,Pool pool);
	bool Create(const unsigned char* data,unsigned long size,unsigned long width,unsigned long height,unsigned long mipmaps,Usage usage,Format format,Pool pool);
	bool Create(unsigned long width,unsigned long height,unsigned long levels,Usage usage,Format format,Pool pool);
	bool Create(const wchar_t* id,const wchar_t* group,unsigned long width,unsigned long height,unsigned long mipmaps,Usage usage,Format format,Pool pool);
	bool GetCreated(void) const { return mCreated; }
	void Destroy(void);

	bool OnLostDevice(void);
	bool OnResetDevice(void);

	IDirect3DTexture9* GetTexture(void) const { return mTexture; }

protected:
	bool					mCreated;

	IDirect3DTexture9*		mTexture;

	unsigned char*			mBuffer;
	unsigned long			mSize;

	unsigned long			mWidth;
	unsigned long			mHeight;
	unsigned long			mMipmaps;
	unsigned long			mLevels;
	unsigned long			mUsage;

	D3DFORMAT				mFormat;
	D3DPOOL					mPool;
};