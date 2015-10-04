#pragma once

#include "Control.h"

class TrackBar;

typedef TrackBar* TrackBarHandle;

class TrackBar : public Control
{
public:
	TrackBar(void);
	~TrackBar(void);

	enum ControlEvent
	{
		ControlEventTrackChange = 40,
	};

	virtual bool OnRender(void);
	virtual bool OnEventMouse(Event event,WPARAM wparam,LPARAM lparam);
	virtual bool OnEventKeyboard(Event event,WPARAM wparam,LPARAM lparam);

	virtual void SetTrackRange(unsigned long range) { mRange = range; }
	virtual unsigned long GetTrackRange(void) const { return mRange; }

	virtual void SetTrackValue(unsigned long value) { mValue = value; }
	virtual unsigned long GetTrackValue(void) const { return mValue; }

	virtual Rect GetTrackBox(void) const;
	virtual bool PointInTrack(const Point& point) const { return GetTrackBox().PointIn(point); }

protected:
	bool mTracking;
	long mTrackingPosition;
	unsigned long mTrackingValue;

	unsigned long mRange;
	unsigned long mValue;
};
