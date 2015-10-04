// Copyright 2004/2006 Marko Mihovilic

#include "Globals.h"
#include "Log.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Log::Log(void)
{
}

Log::~Log(void)
{
	if(mFile.GetOpen())
		CloseLog();
}

bool Log::OpeLog(const wchar_t* path)
{
	if(!mFile.Open(path,File::AccessModeWrite,File::ShareModeRead,File::OpenModeCreate))
		return false;

	return true;
}

void Log::CloseLog()
{
	if(mFile.GetOpen())
		mFile.Close();
}

bool Log::Write(const wchar_t* text)
{
	if(!mFile.GetOpen())
		return false;

	char buffer[4096];
	WideCharToMultiByte(CP_UTF8,NULL,text,-1,buffer,sizeof(buffer),NULL,NULL);

	mFile.Write(buffer,(unsigned long)strlen(buffer));

//#ifdef _DEBUG
//	mFile.Flush();
//#endif

	return true;
}