// Copyright 2004/2006 Marko Mihovilic

#include "Globals.h"
#include "File.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

File::File(void) :
mHandle(INVALID_HANDLE_VALUE)
{
}

File::~File(void)
{
	Close();
}

bool File::Open(const wchar_t* path,AccessMode access,ShareMode share,OpenMode open)
{
	unsigned long acc = NULL;
	unsigned long shr = NULL;
	unsigned long opn = NULL;

	if(access == AccessModeRead)
		acc |= GENERIC_READ;

	if(access == AccessModeWrite)
		acc |= GENERIC_WRITE;

	if(access == AccessModeReadWrite)
		acc |= GENERIC_READ | GENERIC_WRITE;

	if(share == ShareModeRead)
		shr |= FILE_SHARE_READ;

	if(share == ShareModeWrite)
		shr |= FILE_SHARE_WRITE;

	if(share == ShareModeReadWrite)
		shr |= FILE_SHARE_READ | FILE_SHARE_WRITE;

	if(open == OpenModeExisting)
		opn |= OPEN_EXISTING;

	if(open == OpenModeCreate)
		opn |= CREATE_ALWAYS;

	if(open == OpenModeNew)
		opn |= CREATE_NEW;

	if(open == OpenModeTruncate)
		opn |= TRUNCATE_EXISTING;

	mHandle = CreateFile(path,acc,shr,NULL,opn,NULL,NULL);
	if(mHandle == INVALID_HANDLE_VALUE)
		return false;

	return true;
}

void File::Close(void)
{
	if(mHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(mHandle);
		mHandle = INVALID_HANDLE_VALUE;
	}
}

unsigned long File::Read(void* buffer,unsigned long size)
{
	unsigned long read = NULL;

	ReadFile(mHandle,buffer,size,&read,NULL);

	return read;
}

unsigned long File::Write(const void* buffer,unsigned long size)
{
	unsigned long written = NULL;

	WriteFile(mHandle,buffer,size,&written,NULL);

	return written;
}