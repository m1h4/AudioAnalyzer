// Copyright 2004/2006 Marko Mihovilic

#include "Globals.h"
#include "Kernel.h"
#include "CommandLine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CommandLineData::CommandLineData(void)
{
}

CommandLineData::~CommandLineData(void)
{
	Clear();
}

void CommandLineData::Set(const wchar_t* cmdLine)
{
	String cmd = cmdLine;
	
	// Check length
	if(!cmd.GetLength())
		return;

	// Trim spaces
	cmd.TrimAll();

	// Check if entire string was just spaces
	if(!cmd.GetLength())
		return;

	// Quote monitoring
	bool quote = true;

	// Check for unmached quotes
	for(unsigned long i = 0; i < cmd.GetLength(); ++i)
	{
		if(cmd[i] == L'"')
			quote = !quote;
	}

	// Check if ok
	if(!quote)
		return;

	// Clear old vars
	Clear();

	quote = false;

	String varName;
	String varValue;

	bool value = false;

	// Split the string
	while(cmd.GetLength())
	{
		if(cmd[0] == L'"')
		{
			quote = !quote;

			cmd.Erase(0);
		}
		else if(cmd[0] == L'=' && !value)
		{
			value = true;

			cmd.Erase(0);
		}
		else if(iswspace(cmd[0]))
		{
			if(quote)
			{
				if(value)
					varValue += cmd[0];
				else
					varName += cmd[0];
			}
			else if(cmd.GetLength())
			{
				// Add var
				value = false;

				AddVar(varName,varValue);

				varName.Erase();
				varValue.Erase();
			}

			cmd.Erase(0);
		}
		else
		{
			if(value)
				varValue += cmd[0];
			else
				varName += cmd[0];

			cmd.Erase(0);
		}
	}

	if(varName.GetLength())
	{
		// Add var
		value = false;

		AddVar(varName,varValue);

		varName.Erase();
		varValue.Erase();
	}
}

void CommandLineData::AddVar(const wchar_t* name,const wchar_t* value)
{
	CommandLineVar* var = new CommandLineVar;
	ASSERT(var);

	var->name = name;
	var->value = value;

	mVars.InsertBack(var);

	TRACE(L"Var added: %s=%s\n",name,value);
}

void CommandLineData::Clear()
{
	if(mVars.GetSize())
		mVars.Delete();
}

const String& CommandLineData::GetVar(const wchar_t* var,const String& defaul) const
{
	for(unsigned long i = 0; i < mVars.GetSize(); ++i)
		if(!lstrcmpi(mVars[i]->name,var))
			return mVars[i]->value;

	return defaul;
}

bool CommandLineData::GetVar(const wchar_t* var,bool defaul) const
{
	for(unsigned long i = 0; i < mVars.GetSize(); ++i)
		if(!lstrcmpi(mVars[i]->name,var))
			return mVars[i]->value.AsBool();

	return defaul;
}

bool CommandLineData::GetVar(const wchar_t* var) const
{
	for(unsigned long i = 0; i < mVars.GetSize(); ++i)
		if(!lstrcmpi(mVars[i]->name,var))
			return true;

	return false;
}