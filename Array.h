// Copyright 2004/2006 Marko Mihovilic

#pragma once

#define ARRAY_ALLOCATION_SIZE 16

template<class Element,unsigned int AllocatioSize = ARRAY_ALLOCATION_SIZE>
class Array
{
public:
	Array(void);
	~Array(void);

	void Insert(unsigned long at,const Element& element);
	void InsertBack(const Element& element) { Insert(mSize,element); }
	void InsertFront(const Element& element) { Insert(0,element); }

	void Erase(unsigned long at,unsigned long count = 1);
	void Erase(void) { Erase(0,mSize); }
	void EraseBack(void) { Erase(mSize - 1); }
	void EraseFront(void) { Erase(0); }

	void Delete(unsigned long at,unsigned long count = 1);
	void Delete(void) { Delete(0,mSize); }
	void DeleteBack(void) { Delete(mSize - 1); }
	void DeleteFront(void) { Delete(0); }

	unsigned long GetBufferSize(void) const { return mBufferSize; }
	unsigned long GetSize(void) const { return mSize; }

	Element& Front(void) { return mBuffer[0]; }
	const Element& Front(void) const { return mBuffer[0]; }

	Element& Back(void) { return mBuffer[mSize-1]; }
	const Element& Back(void) const { return mBuffer[mSize-1]; }

	operator Element*(void) { return mBuffer; }
	operator const Element*(void) const { return mBuffer; }

protected:
	bool PrepareBuffer(unsigned long newSize,bool preserve);
	bool ResizeBuffer(unsigned long newSize,bool preserve);

	Element*		mBuffer;
	unsigned long	mBufferSize;
	unsigned long	mSize;
};

template<class Element,unsigned int AllocatioSize>
Array<Element,AllocatioSize>::Array(void) :
mBuffer(NULL),
mBufferSize(NULL),
mSize(NULL)
{
}

template<class Element,unsigned int AllocatioSize>
Array<Element,AllocatioSize>::~Array(void)
{
	ResizeBuffer(NULL,false);
}

template<class Element,unsigned int AllocatioSize>
bool Array<Element,AllocatioSize>::PrepareBuffer(unsigned long newSize,bool preserve)
{
	if(mBufferSize == newSize)
		return true;

	if(newSize < mBufferSize)
		if(mBufferSize - newSize < AllocatioSize)
			return true;

	newSize = newSize - newSize % AllocatioSize + AllocatioSize;

	return ResizeBuffer(newSize,preserve);
}

template<class Element,unsigned int AllocatioSize>
bool Array<Element,AllocatioSize>::ResizeBuffer(unsigned long newSize,bool preserve)
{
	if(mBufferSize == newSize)
		return true;

	if(!newSize)
	{
		for(unsigned long i = 0; i < mSize; ++i)
			mBuffer[i].~Element();

		::operator delete(mBuffer);

		mBufferSize = NULL;
		mSize = NULL;
		return true;
	}

	if(!mBuffer)
	{
		mBuffer = (Element*)::operator new(sizeof(Element) * newSize);
		if(!mBuffer)
			return false;

		mBufferSize = newSize;
		return true;
	}

	Element* newBuffer = (Element*)::operator new(sizeof(Element) * newSize);
	if(!newBuffer)
		return false;

	if(preserve)
		CopyMemory(newBuffer,mBuffer,sizeof(Element) * mSize);
	else
	{
		for(unsigned long i = 0; i < mSize; ++i)
			mBuffer[i].~Element();

		mSize = NULL;
	}

	::operator delete(mBuffer);

	mBuffer = newBuffer;
	mBufferSize = newSize;

	return true;
}

template<class Element,unsigned int AllocatioSize>
void Array<Element,AllocatioSize>::Insert(unsigned long at,const Element& element)
{
	ASSERT(at <= mSize);

	PrepareBuffer(mSize + 1,true);

	MoveMemory(mBuffer + at + 1,mBuffer + at,sizeof(Element) * (mSize - at));

	::new(&mBuffer[at]) Element(element);
	
	mSize++;
}

template<class Element,unsigned int AllocatioSize>
void Array<Element,AllocatioSize>::Erase(unsigned long at,unsigned long count)
{
	ASSERT(mBuffer);
	ASSERT(at + count <= mSize);

	for(unsigned long i = at; i < at + count; ++i)
		mBuffer[i].~Element();

	CopyMemory(mBuffer + at,mBuffer + at + count,sizeof(Element) * (mSize - at - count));
	
	mSize -= count;

	PrepareBuffer(mSize,true);
}

template<class Element,unsigned int AllocatioSize>
void Array<Element,AllocatioSize>::Delete(unsigned long at,unsigned long count)
{
	ASSERT(mBuffer);
	ASSERT(at + count <= mSize);

	for(unsigned long i = at; i < at + count; ++i)
		delete mBuffer[i];

	Erase(at,count);
}