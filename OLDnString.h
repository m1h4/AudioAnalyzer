// Copyright 2004/2006 Miha Software

#pragma once

#include <string>
#include <locale>
#include <algorithm>

class nString : public std::string
{
public:
	nString() {}
	~nString() {}

	// Various constructor flavours
	nString(const nString& arg)			{ assign(arg.c_str()); }
	nString(const std::string& arg)	{ assign(arg); }
	nString(const char* pArg)			{ assign(pArg); }

	// Various assignment operator flavours
	nString& operator=(const nString& arg)		{ assign(arg.c_str()); return *this; }
	nString& operator=(const std::string& arg)	{ assign(arg); return *this; }
	nString& operator=(const char* pArg)		{ assign(pArg); return *this; }

	// The printf function
	nString& format(const char* fmt,...)
	{
		static TCHAR strBuffer[8096];
    
		va_list args;
		va_start(args,fmt);
		_vsntprintf(strBuffer,8096,fmt,args);
		va_end(args);

		assign(strBuffer);

		return *this;
	}

	// The trimmer function
	nString& trimRight(const char* pszDelimiters = " \t\r\n")
	{
		size_type idx = this->find_last_not_of(pszDelimiters);
		if(idx != nString::npos)
			this->erase(++idx);

		return *this;
	}

	// The trimmer function
	nString& trimLeft(const char* pszDelimiters = " \t\r\n")
    {
        size_type idx = this->find_first_not_of(pszDelimiters);
        if(idx != nString::npos)
            this->erase(0,idx);
        else
			this->erase();

        return *this;
    }

	// Trim leading and trailing whitespace
    nString& trimAll(const char* pszDelimiters = " \t\r\n")
    {
		return trimLeft(pszDelimiters).trimRight(pszDelimiters);
    }

	// Compare functions
	bool compareNoCase(const char* pString) const
	{
		return strcmpi(c_str(),pString) == 0;
	}

	// Conversion functions
	float asFloat() const
	{
		return (float)strtod(c_str(),NULL);
	}

	double asDouble() const
	{
		return strtod(c_str(),NULL);
	}

	long asInt() const
	{
		return strtol(c_str(),NULL,0);	// When base is set to 0 the conversion fuction automaticty detects the base from the string
	}

	long asUInt() const
	{
		return strtoul(c_str(),NULL,0);	// When base is set to 0 the conversion fuction automaticty detects the base from the string
	}

	bool asBool() const
	{
		if(this->compareNoCase("true"))
			return true;
		else if(this->compareNoCase("false"))
			return false;
		else if(this->asInt())
			return true;
		else
			return false;
	}

	unsigned long findTimes(const char* pszDelimiter = " ")
	{
		unsigned long found = 0;
		unsigned long at = 0;
		while((at = (unsigned long)this->find(pszDelimiter,at)) != nString::npos)
		{
			found++;
			at++;
		}

		return found;
	}

	nString& toUpper() { std::transform(this->begin(), this->end(), this->begin(), toupper); return *this;  }
	nString& toLower() { std::transform(this->begin(), this->end(), this->begin(), tolower); return *this;  }
};
