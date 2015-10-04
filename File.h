#pragma once

class File
{
public:
	File(void);
	~File(void);

	enum AccessMode
	{
		AccessModeRead,
		AccessModeWrite,
		AccessModeReadWrite,
	};

	enum ShareMode
	{
		ShareModeRead,
		ShareModeWrite,
		ShareModeReadWrite,
	};

	enum OpenMode
	{
		OpenModeExisting,
		OpenModeTruncate,
		OpenModeNew,
		OpenModeCreate,
	};

	bool Open(const wchar_t* path,AccessMode access,ShareMode share,OpenMode open);
	void Flush(void) { FlushFileBuffers(mHandle); }
	void Close(void);
	
	unsigned long Read(void* buffer,unsigned long size);
	unsigned long Write(const void* buffer,unsigned long size);

	bool GetOpen(void) const { return mHandle != INVALID_HANDLE_VALUE; }
	unsigned long GetSize(unsigned long* high = NULL) { return GetFileSize(mHandle,high); }

protected:
	HANDLE	mHandle;
};