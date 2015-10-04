// Copyright 2004/2006 Marko Mihovilic

#pragma once

#define STRING_ALLOCATION_SIZE 16

class String
{
public:
	String(void);
	String(const wchar_t* buffer);
	String(const String& other);
	~String(void);

	float AsFloat() const;
	double AsDouble() const;
	long AsInt() const;
	unsigned long AsUInt() const;
	bool AsBool(void) const;

	//void FromFloat(float value);
	//void FromDouble(double value);
	void FromInt(long value);
	void FromUInt(unsigned long value);
	void FromBool(bool value);

	void ToLower(unsigned long from,unsigned long to);
	void ToLower(void) { ToLower(0,m_Length); }
	void ToUpper(unsigned long from,unsigned long to);
	void ToUpper(void) { ToUpper(0,m_Length); }

	void TrimLeft(const wchar_t* str);
	void TrimLeft(void);
	void TrimRight(const wchar_t* str);
	void TrimRight(void);
	void TrimAll(const wchar_t* str) { TrimLeft(str); TrimRight(str); }
	void TrimAll(void) { TrimLeft(); TrimRight(); }
	
	unsigned long FindInstances(unsigned long from,unsigned long to,const wchar_t* buffer) const;
	unsigned long FindInstances(unsigned long from,unsigned long to,wchar_t chr) const { wchar_t str[] = {chr,0}; return FindInstances(from,to,str); }
	unsigned long FindLeft(unsigned long from,unsigned long to,const wchar_t* buffer) const;
	unsigned long FindLeft(const wchar_t* buffer) const { return FindLeft(0,m_Length,buffer); }
	unsigned long FindRight(unsigned long from,unsigned long to,const wchar_t* buffer) const;
	unsigned long FindRight(const wchar_t* buffer) const { return FindRight(m_Length,0,buffer); }
	unsigned long FindLeftLast(unsigned long from,unsigned long to,const wchar_t* buffer) const;
	unsigned long FindLeftLast(const wchar_t* buffer) const { return FindLeftLast(0,m_Length,buffer); }
	unsigned long FindRightLast(unsigned long from,unsigned long to,const wchar_t* buffer) const;
	unsigned long FindRightLast(const wchar_t* buffer) const { return FindRightLast(m_Length,0,buffer); }
	
	void Insert(unsigned long at,const wchar_t* buffer);
	void Insert(unsigned long at,wchar_t chr) { wchar_t str[] = {chr,0}; Insert(at,str); }
	
	void Erase(unsigned long at,unsigned long count = 1);
	void Erase(void) { Erase(0,m_Length); }

	bool SetBuffer(const wchar_t* data);
	wchar_t* GetBuffer(void) const { return m_Buffer; }
	unsigned long GetBufferSize(void) const { return m_BufferSize; }
	unsigned long GetLength(void) const { return m_Length; }

	int Compare(const wchar_t* other) const { return wcscmp(m_Buffer,other); }
	int ComparePart(const wchar_t* part) const { return wcsncmp(m_Buffer,part,wcslen(part)); }
	int CompareNoCase(const wchar_t* other) const { return _wcsicmp(m_Buffer,other); }
	int CompareNoCasePart(const wchar_t* part) const { return _wcsnicmp(m_Buffer,part,wcslen(part)); }

	String& operator=(const wchar_t* other) { SetBuffer(other); return *this; }
	String& operator=(const String& other) { SetBuffer(other.GetBuffer()); return *this; }

	bool operator==(const wchar_t* other) const { return Compare(other) == 0; }
	bool operator!=(const wchar_t* other) const { return Compare(other) != 0; }
	bool operator>(const wchar_t* other) const { return Compare(other) == 1; }
	bool operator<(const wchar_t* other) const { return Compare(other) == -1; }
	bool operator>=(const wchar_t* other) const { return Compare(other) != -1; }
	bool operator<=(const wchar_t* other) const { return Compare(other) != 1; }

	bool operator==(const String& other) const { return Compare(other) == 0; }
	bool operator!=(const String& other) const { return Compare(other) != 0; }
	bool operator>(const String& other) const { return Compare(other) == 1; }
	bool operator<(const String& other) const { return Compare(other) == -1; }
	bool operator>=(const String& other) const { return Compare(other) != -1; }
	bool operator<=(const String& other) const { return Compare(other) != 1; }

	String operator+(const wchar_t* other) const { String string(m_Buffer); string += other; return string; }
	String& operator+=(const wchar_t* other) { Insert(GetLength(),other); return *this; }
	
	String operator+(const String& other) const { String string(m_Buffer); string += other; return string; }
	String& operator+=(const String& other) { Insert(GetLength(),other.GetBuffer()); return *this; }

	String operator+(wchar_t other) const { String string(m_Buffer); string += other; return string; }
	String& operator+=(wchar_t other) { Insert(GetLength(),other); return *this; }

	operator wchar_t*(void) { return m_Buffer; }
	operator const wchar_t*(void) const { return m_Buffer; }

protected:
	bool PrepareBuffer(unsigned long newSize,bool preserve);
	bool ResizeBuffer(unsigned long newSize,bool preserve);

	wchar_t*		m_Buffer;
	unsigned long	m_BufferSize;
	unsigned long	m_Length;
};