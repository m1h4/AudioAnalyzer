#include "Globals.h"
#include "Kernel.h"
#include "Control.h"

Control::Control(void) :
mParent(NULL),
mID(NULL),
mSnap(NULL),
mEnabled(true),
mVisible(true),
mFocused(true),
mMouseOver(false),
mEditorMode(false),
mSize(0,0),
mPosition(0,0)
{
}

Control::~Control(void)
{
	while(mChilds.GetSize())
	{
		SAFEDELETE(mChilds.Back());
		mChilds.EraseBack();
	}
}

void Control::AttachChild(const ControlHandle& child)
{
	mChilds.InsertBack(child);

	mChilds.Back()->SetParent(this);
	mChilds.Back()->SetFocused(false);

	OnEvent(EventGotChild,(WPARAM)child);
}

void Control::DetachChild(unsigned long index)
{
	if(index >= mChilds.GetSize())
		return;

	mChilds[index]->SetParent(NULL);
	mChilds[index]->SetFocused(true);

	mChilds.Erase(index);

	OnEvent(EventLostChild,index);
}

ControlHandle Control::GetChild(unsigned long index)
{
	if(index >= mChilds.GetSize())
		return NULL;

	return mChilds[index];
}

void Control::FocusChild(unsigned long index)
{
	if(index >= mChilds.GetSize())
		return;

	Focus();

	UnfocusChilds();

	mChilds[index]->SetFocused(true);
}

void Control::UnfocusChilds(void)
{
	for(unsigned long i = 0; i < mChilds.GetSize(); ++i)
		if(mChilds[i]->GetFocused())
			mChilds[i]->SetFocused(false);
}

unsigned long Control::GetFocusedChild(void)
{
	for(unsigned long i = 0; i < mChilds.GetSize(); ++i)
		if(mChilds[i]->GetFocused())
			return i;

	return 0xFFFFFFFF;
}

unsigned long Control::GetChildIndex(const ControlHandle& child)
{
	for(unsigned long i = 0; i < mChilds.GetSize(); ++i)
		if(mChilds[i] == child)
			return i;

	return 0xFFFFFFFF;
}

unsigned long Control::GetChildIndex(unsigned long id)
{
	for(unsigned long i = 0; i < mChilds.GetSize(); ++i)
		if(mChilds[i]->GetID() == id)
			return i;

	return 0xFFFFFFFF;
}

ControlHandle Control::GetChildAtPoint(const Point& point)
{
	for(unsigned long i = 0; i < mChilds.GetSize(); ++i)
		if(mChilds[i]->PointIn(point) && mChilds[i]->GetEnabled() && mChilds[i]->GetVisible())
			return mChilds[i];

	if(PointIn(point) && GetEnabled() && GetVisible())
		return this;

	return NULL;
}

bool Control::ContainsChild(const ControlHandle& child)
{
	return GetChildIndex(child) != 0xFFFFFFFF;
}

bool Control::SetFocusNext(void)
{
	if(!GetChildCount())
		return false;

	unsigned long focused = GetFocusedChild();

	if(focused == 0xFFFFFFFF)
		FocusChild(0);
	else
		FocusChild((focused + 1) % mChilds.GetSize());

	return true;
}

bool Control::SetFocusPrevious(void)
{
	if(!GetChildCount())
		return false;

	unsigned long focused = GetFocusedChild();

	if(!focused || focused == 0xFFFFFFFF)
		FocusChild(mChilds.GetSize() - 1);
	else
		FocusChild(focused - 1);

	return true;
}

Point Control::GetScreenPosition(void) const
{
	Point position = GetPosition();
	ToScreen(&position);
	return position;
}

void Control::ToScreen(Point* point) const
{
	if(GetParent())
	{
		Point position = GetParent()->GetPosition();
		GetParent()->ToScreen(&position);

		*point += position;
	}
}

void Control::ToClient(Point* point) const
{
	if(GetParent())
	{
		Point position = GetParent()->GetPosition();
		GetParent()->ToClient(&position);

		*point -= position;
	}
}

bool Control::PointIn(const Point& point) const
{
	if(!GetVisible())
		return false;

	Point position = GetScreenPosition();

	if(point.x < position.x || point.x > position.x + mSize.cx)
		return false;

	if(point.y < position.y || point.y > position.y + mSize.cy)
		return false;

	return true;
}

bool Control::Close(void)
{
	if(GetParent())
		GetParent()->DetachChild(GetParent()->GetChildIndex(this));

	OnEvent(EventClose);

	delete this;

	return true;
}

void Control::SetSize(const Size& size)
{
	if(GetSize() == size)
		return;
	
	Size delta = size - mSize;

	mSize = size;

	for(unsigned long i = 0; i < mChilds.GetSize(); ++i)
	{
		if(mChilds[i]->GetSnap() & SnapRight && !(mChilds[i]->GetSnap() & SnapLeft))
			mChilds[i]->SetPosition(Point(mChilds[i]->GetPosition().x+delta.cx,mChilds[i]->GetPosition().y));
		else if(mChilds[i]->GetSnap() & SnapRight)
			mChilds[i]->SetSize(Size(mChilds[i]->GetSize().cx+delta.cx,mChilds[i]->GetSize().cy));

		if(mChilds[i]->GetSnap() & SnapBottom && !(mChilds[i]->GetSnap() & SnapTop))
			mChilds[i]->SetPosition(Point(mChilds[i]->GetPosition().x,mChilds[i]->GetPosition().y+delta.cy));
		else if(mChilds[i]->GetSnap() & SnapBottom)
			mChilds[i]->SetSize(Size(mChilds[i]->GetSize().cx,mChilds[i]->GetSize().cy+delta.cy));
	}
	
	OnEvent(EventSizeChange,(WPARAM)&GetSize());
}

bool Control::OnRender(void)
{
	if(GetEditorMode())
		OnRenderEditorMode();

	unsigned long focused = GetFocusedChild();

	for(unsigned long i = 0; i < mChilds.GetSize(); ++i)
		if(i != focused)
			mChilds[i]->OnRender();

	if(focused != 0xFFFFFFFF)
		mChilds[focused]->OnRender();

	return true;
}

bool Control::OnRenderEditorMode(void)
{
	GetKernel()->GetUserInterface()->DrawResizeHandles(GetBoundingBox());
	return true;
}

bool Control::OnUpdate(void)
{
	unsigned long focused = GetFocusedChild();

	for(unsigned long i = 0; i < mChilds.GetSize(); ++i)
		if(i != focused)
			mChilds[i]->OnUpdate();

	if(focused != 0xFFFFFFFF)
		mChilds[focused]->OnUpdate();

	return true;
}

bool Control::OnEventMouseEditor(Event event,WPARAM wparam,LPARAM lparam)
{
	if(!GetEditorMode())
		return false;

	//
	// TODO Implement control resizing/repositioning code
	//

	return false;
}