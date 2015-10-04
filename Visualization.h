// Copyright 2004/2006 Marko Mihovilic

#pragma once

class Visualization
{
public:
	Visualization(void) {}
	virtual ~Visualization(void) {}

	virtual bool RenderBackground();
	virtual bool Render() = NULL;	// Render the actual vis
	virtual bool RenderOverlay() { return true; }

	virtual void SetName(const wchar_t* name) { mName = name; }
	virtual const String& GetName() const { return mName; }

protected:
	String		mName;
};