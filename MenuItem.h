// Copyright 2004/2006 Marko Mihovilic

#pragma once

class MenuItem
{
public:
	// The type is used only for rendering
	enum Type
	{
		Label,	// Can have text
		Title,	// Can have text
		Seperator,	// |
		Margin,	// Empty
	};

	MenuItem(void) : mId(0), mStatic(false),mType(MenuItem::Label) {}
	virtual ~MenuItem(void) {}

	virtual void SetText(const wchar_t* text) = NULL;
	virtual const wchar_t* GetText() = NULL;

	void SetId(unsigned long id) { mId = id; }
	unsigned long GetId() const { return mId; }

	// Statics can't be selected
	void SetStatic(bool stati) { mStatic = stati; }
	bool GetStatic() const { return mStatic; }

	void SetType(Type type) { mType = type; }
	Type GetType() const { return mType; }

protected:
	unsigned long				mId;
	bool						mStatic;
	Type						mType;
};
