// Copyright 2004/2006 Marko Mihovilic

#include "Globals.h"
#include "String.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

String::String(void) :
m_Buffer(NULL),
m_BufferSize(NULL),
m_Length(NULL)
{
	SetBuffer(L"");
}

String::String(const wchar_t* buffer) :
m_Buffer(NULL),
m_BufferSize(NULL),
m_Length(NULL)
{
	SetBuffer(buffer);
}

String::String(const String& other) :
m_Buffer(NULL),
m_BufferSize(NULL),
m_Length(NULL)
{
	SetBuffer(other.GetBuffer());
}

String::~String(void)
{
	ResizeBuffer(NULL,false);
}

bool String::PrepareBuffer(unsigned long newSize,bool preserve)
{
	if(m_BufferSize == newSize)
		return true;

	if(newSize < m_BufferSize)
		if(m_BufferSize - newSize < STRING_ALLOCATION_SIZE)
			return true;

	newSize = newSize - newSize % STRING_ALLOCATION_SIZE + STRING_ALLOCATION_SIZE;

	return ResizeBuffer(newSize,preserve);
}

bool String::ResizeBuffer(unsigned long newSize,bool preserve)
{
	if(m_BufferSize == newSize)
		return true;

	if(!newSize)
	{
		delete[] m_Buffer;

		m_Length = NULL;
		m_Buffer = NULL;
		m_BufferSize = NULL;
		return true;
	}

	if(!m_Buffer)
	{
		m_Buffer = new wchar_t[newSize];
		if(!m_Buffer)
			return false;

		m_BufferSize = newSize;
		return true;
	}

	wchar_t* newBuffer = new wchar_t[newSize];
	if(!newBuffer)
		return false;

	if(preserve)
		CopyMemory(newBuffer,m_Buffer,(m_Length + 1) * sizeof(wchar_t));
	else
		m_Length = NULL;

	delete[] m_Buffer;

	m_Buffer = newBuffer;
	m_BufferSize = newSize;

	return true;
}

bool String::SetBuffer(const wchar_t* buffer)
{
	ASSERT(buffer);
	ASSERT(buffer != m_Buffer);

	unsigned long length = wcslen(buffer);

	if(!PrepareBuffer(length + 1,false))
		return false;

	CopyMemory(m_Buffer,buffer,(length + 1) * sizeof(wchar_t));

	m_Length = length;

	return true;
}

void String::Erase(unsigned long at,unsigned long count)
{
	ASSERT(m_Buffer);
	ASSERT(at + count <= m_Length);

	CopyMemory(m_Buffer + at,m_Buffer + at + count,(m_Length - at - count + 1) * sizeof(wchar_t));
	
	m_Length -= count;

	PrepareBuffer(m_Length + 1,true);
}

void String::Insert(unsigned long at,const wchar_t* buffer)
{
	ASSERT(at <= m_Length);
	ASSERT(buffer);
	ASSERT(buffer != m_Buffer);

	unsigned long length = wcslen(buffer);

	PrepareBuffer(m_Length + 1 + length,true);

	MoveMemory(m_Buffer + at + length,m_Buffer + at,(m_Length + 1 - at) * sizeof(wchar_t));
	CopyMemory(m_Buffer + at,buffer,length * sizeof(wchar_t));

	m_Length += length;
}

unsigned long String::FindLeft(unsigned long from,unsigned long to,const wchar_t* buffer) const
{
	ASSERT(from <= m_Length);
	ASSERT(to <= m_Length);
	ASSERT(from <= to);
	ASSERT(buffer);

	unsigned long length = wcslen(buffer);

	for(unsigned long i = from; i < to; ++i)
		if(wcsncmp(m_Buffer + i,buffer,min(m_Length - i,length)) == 0)
			return i;

	return 0xFFFFFFFF;
}

unsigned long String::FindRight(unsigned long from,unsigned long to,const wchar_t* buffer) const
{
	ASSERT(from <= m_Length);
	ASSERT(to <= m_Length);
	ASSERT(from >= to);
	ASSERT(buffer);

	unsigned long length = wcslen(buffer);

	for(unsigned long i = from; i > to; --i)
		if(wcsncmp(m_Buffer + i,buffer,min(m_Length - i,length)) == 0)
			return i;

	return 0xFFFFFFFF;
}

unsigned long String::FindLeftLast(unsigned long from,unsigned long to,const wchar_t* buffer) const
{
	ASSERT(from <= m_Length);
	ASSERT(to <= m_Length);
	ASSERT(from <= to);
	ASSERT(buffer);

	unsigned long length = wcslen(buffer);

	if(length > m_Length)
		return 0xFFFFFFFF;

	unsigned long i;
	for(i = from; i < to; i += length)
		if(wcsncmp(m_Buffer + i,buffer,min(m_Length - i,length)) != 0)
			return i == from ? 0xFFFFFFFF : i;

	return i >= to ? 0xFFFFFFFF : i;
}

unsigned long String::FindRightLast(unsigned long from,unsigned long to,const wchar_t* buffer) const
{
	ASSERT(from <= m_Length);
	ASSERT(to <= m_Length);
	ASSERT(from >= to);
	ASSERT(buffer);

	unsigned long length = wcslen(buffer);

	if(from < length)
		return 0xFFFFFFFF;

	unsigned long i;
	for(i = from - length; i > to; i -= length > i ? i : length)
		if(wcsncmp(m_Buffer + i,buffer,min(m_Length - i,length)) != 0)
			return i == from ? 0xFFFFFFFF : i;

	return i <= to ? 0xFFFFFFFF : i;
}

unsigned long String::FindInstances(unsigned long from,unsigned long to,const wchar_t* buffer) const
{
	ASSERT(from <= m_Length);
	ASSERT(to <= m_Length);
	ASSERT(from <= to);
	ASSERT(buffer);

	unsigned long length = wcslen(buffer);
	unsigned long count = 0;

	for(unsigned long i = from; i < to; ++i)
	{
		if(wcsncmp(m_Buffer + i,buffer,min(m_Length - i,length)) == 0)
		{
			count++;
			i += length;
		}
	}

	return count;
}

void String::TrimLeft(const wchar_t* str)
{
	ASSERT(str);

	unsigned long last = FindLeftLast(str);
	if(last == 0xFFFFFFFF)
		return;

	Erase(0,last);
}

void String::TrimLeft(void)
{
	unsigned long last;

	for(last = 0; last < m_Length; ++last)
		if(!iswspace(m_Buffer[last]))
			break;

	Erase(0,last);
}

void String::TrimRight(const wchar_t* str)
{
	ASSERT(str);

	unsigned long last = FindRightLast(str);
	if(last == 0xFFFFFFFF)
		return;

	last++;

	Erase(last,m_Length - last);
}

void String::TrimRight(void)
{
	ASSERT(m_Length);

	long last;

	for(last = m_Length-1; last >= 0; --last)
		if(!iswspace(m_Buffer[last]))
			break;

	last++;

	Erase(last,m_Length - last);
}

void String::ToLower(unsigned long from,unsigned long to)
{
	ASSERT(from <= m_Length);
	ASSERT(to <= m_Length);
	ASSERT(from <= to);

	for(unsigned long i = from; i < to; ++i)
		m_Buffer[i] = towlower(m_Buffer[i]);
}

void String::ToUpper(unsigned long from,unsigned long to)
{
	ASSERT(from <= m_Length);
	ASSERT(to <= m_Length);
	ASSERT(from <= to);

	for(unsigned long i = from; i < to; ++i)
		m_Buffer[i] = towupper(m_Buffer[i]);
}

float String::AsFloat(void) const
{
	return (float)wcstod(m_Buffer,NULL);
}

double String::AsDouble(void) const
{
	return wcstod(m_Buffer,NULL);
}

long String::AsInt(void) const
{
	return wcstol(m_Buffer,NULL,0);
}

unsigned long String::AsUInt(void) const
{
	return wcstoul(m_Buffer,NULL,0);
}

bool String::AsBool(void) const
{
	if(_wcsicmp(m_Buffer,L"true") == 0)
		return true;
	else if(AsInt())
		return true;
	else
		return false;
}
/*
void String::FromFloat(float value)
{
	wchar_t buffer[64];
	_ftow(value,buffer,10);
	SetBuffer(buffer);
}

void String::FromDouble(double value)
{
	wchar_t buffer[64];
	_dtow(value,buffer,10);
	SetBuffer(buffer);
}
*/
void String::FromInt(long value)
{
	wchar_t buffer[64];
	_ltow(value,buffer,10);
	SetBuffer(buffer);
}

void String::FromUInt(unsigned long value)
{
	wchar_t buffer[64];
	_ultow(value,buffer,10);
	SetBuffer(buffer);
}

void String::FromBool(bool value)
{
	SetBuffer(value ? L"True" : L"False");
}