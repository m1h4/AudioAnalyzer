// Copyright 2004/2006 Marko Mihovilic

#include "Globals.h"
#include "Kernel.h"
#include "StateBlock.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

StateBlock::StateBlock(void) :
mCreated(false),
mStateBlock(NULL)
{
}

StateBlock::~StateBlock(void)
{
}

bool StateBlock::CreateStateBlock(State states)
{
	ASSERT(!mCreated);

	if(states == StateAll)
		mType = D3DSBT_ALL;

	if(states == StatePixel)
		mType = D3DSBT_PIXELSTATE;

	if(states == StateVertex)
		mType = D3DSBT_VERTEXSTATE;

	if(FAILED(GetKernel()->GetGraphics()->GetDevice()->CreateStateBlock(mType,&mStateBlock)))
		return false;

	mCreated = true;

	return true;
}

void StateBlock::Destroy(void)
{
	mCreated = false;

	SAFERELEASE(mStateBlock);
}

bool StateBlock::Capture(void)
{
	return SUCCEEDED(mStateBlock->Capture());
}

bool StateBlock::Apply(void)
{
	return SUCCEEDED(mStateBlock->Apply());
}

bool StateBlock::OnLostDevice(void)
{
	SAFERELEASE(mStateBlock);

	return true;
}

bool StateBlock::OnResetDevice(void)
{
	if(FAILED(GetKernel()->GetGraphics()->GetDevice()->CreateStateBlock(mType,&mStateBlock)))
		return false;

	return true;
}