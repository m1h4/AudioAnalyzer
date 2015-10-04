#pragma once

#include "Texture.h"

class Transition
{
public:
	Transition(void) {}
	virtual ~Transition(void) {}

	virtual void CalcTransition(const Texture& texture,Rect* rect,Color* color,float time) = NULL;

	void SetName(const wchar_t* name) { mName = name; }
	const String& GetName(void) const { return mName; }

protected:
	String mName;
};