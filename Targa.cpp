#include "Globals.h"
#include "Targa.h"

bool Targa::Get32(const unsigned char* data,unsigned long /*size*/)
{
	mData = new unsigned char[mHeader.width * mHeader.height * mHeader.bitsperpixel / 8];
	if(!mData)
		return false;

	if(mHeader.imagedescriptor == 8)	// Stored top - bottom
	{
		for(unsigned long y = 0; y < mHeader.height; ++y)
			for(unsigned long x = 0; x < mHeader.width * 4U; ++x)
				mData[y * 4 * mHeader.width + x] = data[(mHeader.height - 1 - y) * 4 * mHeader.width + x];
	}
	else	// Stored left - right
		CopyMemory(mData,data,mHeader.width * mHeader.height * mHeader.bitsperpixel / 8);

	// TGA is stored in BGR, make it RGB
	//for(unsigned long i = 0; i < mHeader.width * mHeader.height * mHeader.bitsperpixel / 8U; i += 4)
	//	mData[i] ^= mData[i + 2] ^= mData[i] ^= mData[i + 2];

	return true;
}

bool Targa::Get24(const unsigned char* data,unsigned long /*size*/)
{
	mData = new unsigned char[mHeader.width * mHeader.height * mHeader.bitsperpixel / 8];
	if(!mData)
		return false;

	if(mHeader.imagedescriptor == 8)	// Stored top - bottom
	{
		for(unsigned long y = 0; y < mHeader.height; ++y)
			for(unsigned long x = 0; x < mHeader.width; ++x)
				mData[y * 3 * mHeader.width + x] = data[x * 3 * mHeader.height + y];
	}
	else	// Stored left - right
		CopyMemory(mData,data,mHeader.width * mHeader.height * mHeader.bitsperpixel / 8);

	// TGA is stored in BGR, make it RGB
	//for(unsigned long i = 0; i < mHeader.width * mHeader.height * mHeader.bitsperpixel / 8U; i += 3)
	//	mData[i] ^= mData[i + 2] ^= mData[i] ^= mData[i + 2];

	return true;
}

bool Targa::Get8(const unsigned char* data,unsigned long /*size*/)
{
	mData = new unsigned char[mHeader.width * mHeader.height * mHeader.bitsperpixel / 8];
	if(!mData)
		return false;

	if(mHeader.imagedescriptor == 8)	// Stored top - bottom
	{
		for(unsigned long y = 0; y < mHeader.height; ++y)
			for(unsigned long x = 0; x < mHeader.width; ++x)
				mData[y * mHeader.width + x] = data[x * mHeader.height + y];
	}
	else	// Stored left - right
		CopyMemory(mData,data,mHeader.width * mHeader.height * mHeader.bitsperpixel / 8);

	return true;
}

bool Targa::LoadFromFile(LPCTSTR path)
{
	HANDLE hFile = CreateFile(path,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,NULL,NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return false;

	unsigned char* buffer = new unsigned char[GetFileSize(hFile,NULL)];
	if(!buffer)
		return false;

	unsigned long read;

	ReadFile(hFile,buffer,GetFileSize(hFile,NULL),&read,NULL);

	if(!LoadFromMemory(buffer,read))
	{
		delete buffer;
		return false;
	}

	delete buffer;
	return true;
}

bool Targa::LoadFromMemory(const unsigned char* data,unsigned long size)
{
	delete[] mData;
	mData = NULL;

	if(size < sizeof(TargaHeader))
		return false;

	CopyMemory(&mHeader,data,sizeof(TargaHeader));

	size -= sizeof(TargaHeader);
	data += sizeof(TargaHeader);

// 0  -  No image data included.
// 1  -  Uncompressed, color-mapped images.
// 2  -  Uncompressed, RGB images.
// 3  -  Uncompressed, black and white images.
// 9  -  Runlength encoded color-mapped images.
// 10  -  Runlength encoded RGB images.
// 11  -  Compressed, black and white images.
// 32  -  Compressed color-mapped data, using Huffman, Delta, and runlength encoding.
// 33  -  Compressed color-mapped data, using Huffman, Delta, and runlength encoding.  4-pass quadtree-type process.

	// We only support uncompressed RGB images written in top to bottom order
	if(mHeader.datatypecode != 2 || mHeader.imagedescriptor != 8)
		return false;

	if(size < (unsigned long)mHeader.idlength + mHeader.colormaptype * mHeader.colormaplength)
		return false;

	data += mHeader.idlength + mHeader.colormaptype * mHeader.colormaplength;
	size -= mHeader.idlength + mHeader.colormaptype * mHeader.colormaplength;

	if(mHeader.bitsperpixel == 32)
		return Get32(data, size);
	else if(mHeader.bitsperpixel == 24)
		return Get24(data, size);	
	else if(mHeader.bitsperpixel == 8)
		return Get8(data, size);

    return false;
}