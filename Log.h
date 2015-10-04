#pragma once

class Log
{
public:
	Log(void);
	~Log(void);

	bool OpeLog(const wchar_t* path);
	bool GetOpen(void) const { return mFile.GetOpen(); }
	void CloseLog(void);

	bool Write(const wchar_t* text);

private:
	File	mFile;
};