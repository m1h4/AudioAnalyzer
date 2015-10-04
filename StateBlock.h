#pragma once

class StateBlock
{
public:
	StateBlock(void);
	~StateBlock(void);

	enum State
	{
		StateAll,
		StatePixel,
		StateVertex,
	};

	bool CreateStateBlock(State states);
	bool GetCreated(void) const { return mCreated; }
	void Destroy(void);

	bool Capture(void);
	bool Apply(void);

	bool OnLostDevice(void);
	bool OnResetDevice(void);

	IDirect3DStateBlock9* GetStateBlock(void) const { return mStateBlock; }

protected:
	bool						mCreated;

	IDirect3DStateBlock9*		mStateBlock;

	D3DSTATEBLOCKTYPE			mType;
};
