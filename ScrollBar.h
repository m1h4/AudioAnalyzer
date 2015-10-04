#pragma once

#include "Control.h"

class ScrollBar;

typedef ScrollBar* ScrollBarHandle;

class ScrollBar : public Control
{
public:
	ScrollBar(void);
	~ScrollBar(void);

	virtual bool OnRender(void);
	virtual bool OnEventMouse(Event event,WPARAM wparam = NULL,LPARAM lparam = NULL);

	virtual void SetTrackPosition(unsigned long position) { m_TrackPosition = position; }
	virtual unsigned long GetTrackPosition(void) const { return m_TrackPosition; }

	virtual void SetTrackRange(unsigned long range) { m_TrackRange = range; }
	virtual unsigned long GetTrackRange(void) const { return m_TrackRange; }

	virtual void SetTrackCount(unsigned long count) { m_TrackCount = count; }
	virtual unsigned long GetTrackCount(void) const { return m_TrackCount; }

	virtual void SetUpPressed(bool pressed) { m_UpPressed = pressed; }
	virtual bool GetUpPressed(void) const { return m_UpPressed; }

	virtual void SetUpEnabled(bool enabled) { m_UpEnabled = enabled; }
	virtual bool GetUpEnabled(void) const { return m_UpEnabled && GetEnabled(); }

	virtual void SetUpVisible(bool visible) { m_UpVisible = visible; }
	virtual bool GetUpVisible(void) const { return m_UpVisible && GetVisible(); }

	virtual void SetDownPressed(bool pressed) { m_DownPressed = pressed; }
	virtual bool GetDownPressed(void) const { return m_DownPressed; }

	virtual void SetDownEnabled(bool enabled) { m_DownEnabled = enabled; }
	virtual bool GetDownEnabled(void) const { return m_DownEnabled && GetEnabled(); }

	virtual void SetDownVisible(bool visible) { m_DownVisible = visible; }
	virtual bool GetDownVisible(void) const { return m_DownVisible && GetVisible(); }

	virtual void SetTrackPressed(bool pressed) { m_TrackPressed = pressed; }
	virtual bool GetTrackPressed(void) const { return m_TrackPressed; }

	virtual void SetTrackEnabled(bool enabled) { m_TrackEnabled = enabled; }
	virtual bool GetTrackEnabled(void) const { return m_TrackEnabled && GetEnabled(); }

	virtual void SetTrackVisible(bool visible) { m_TrackVisible = visible; }
	virtual bool GetTrackVisible(void) const { return m_TrackVisible && GetVisible(); }

protected:
	unsigned long	m_TrackPosition;
	unsigned long	m_TrackRange;
	unsigned long	m_TrackCount;

	bool			m_UpPressed;
	bool			m_UpEnabled;
	bool			m_UpVisible;

	bool			m_DownPressed;
	bool			m_DownEnabled;
	bool			m_DownVisible;

	bool			m_TrackPressed;
	bool			m_TrackEnabled;
	bool			m_TrackVisible;
};
