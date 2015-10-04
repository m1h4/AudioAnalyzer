#pragma once

class CommandLineVar
{
public:
	CommandLineVar(void) {}
	~CommandLineVar(void) {}

	String	name;
	String	value;
};

class CommandLineData
{
public:
	CommandLineData(void);
	~CommandLineData(void);

	void Set(const wchar_t* cmdLine);
	void Clear();

	void AddVar(const wchar_t* name,const wchar_t* value);
	const String& GetVar(const wchar_t* var,const String& defaul) const;
	bool GetVar(const wchar_t* var,bool defaul) const;
	bool GetVar(const wchar_t* var) const;

protected:
	Array<CommandLineVar*>		mVars;
};
