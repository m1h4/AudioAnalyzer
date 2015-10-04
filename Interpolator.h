#pragma once

class Interpolator
{
public:
	Interpolator(void) : mDestination(NULL), mSource(NULL), mTime(NULL), mSpeed(NULL) {}
	virtual ~Interpolator(void) {}

	void AddState(float time,float value) { mStates.InsertBack(time); mValues.InsertBack(value); }
	bool RemoveState(unsigned long index) { if(index >= mStates.GetSize()) return false; mStates.Erase(index); mValues.Erase(index); return true; }
	float GetStateTime(unsigned long index) const { return mStates[index]; }
	float GetStateValue(unsigned long index) const { return mValues[index]; }
	unsigned long GetStateCount(void) const { return mStates.GetSize(); }

	void SetSource(unsigned long source) { if(source >= mStates.GetSize()) return; mSource = source; mTime = mStates[mSource]; }
	unsigned long GetSource(void) const { return mSource; }

	void SetDestination(unsigned long dest) { if(dest >= mStates.GetSize()) return; mDestination = dest; }
	unsigned long GetDestination(void) const { return mDestination; }

	void SetTime(float time) { mTime = time; }
	float GetTime(void) const { return mTime; }

	void SetSpeed(float speed) { mSpeed = speed; }
	float GetSpeed(void) const { return mSpeed; }

	float GetValue(void) const;

	virtual void Update(void);
	virtual float Interpolate(unsigned long left,unsigned long right) const = NULL;

protected:
	Array<float>	mStates;
	Array<float>	mValues;
	
	float			mTime;
	float			mSpeed;

	unsigned long	mSource;
	unsigned long	mDestination;
};