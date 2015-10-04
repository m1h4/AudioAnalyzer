// Copyright 2004/2006 Marko Mihovilic

#include "Globals.h"
#include "TransitionEffects.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning(disable : 4100)	// Unreferenced Parameter

void TransitionTVHorizontal::CalcTransition(const Texture& texture,Rect* rect,Color* color,float time)
{
	m_Interpolator1.SetTime(time);
	m_Interpolator2.SetTime(time);

	float width = m_Interpolator1.GetValue();
	float height = m_Interpolator2.GetValue();

	Rect rectOut;
	rectOut.left = rect->left + rect->GetWidth()/2-width*rect->GetWidth()/2;
	rectOut.top = rect->top + rect->GetHeight()/2-height*rect->GetHeight()/2;
	rectOut.right = rectOut.left + width*rect->GetWidth();
	rectOut.bottom = rectOut.top + height*rect->GetHeight();

	*rect = rectOut;
}

void TransitionTVVertical::CalcTransition(const Texture& texture,Rect* rect,Color* color,float time)
{
	m_Interpolator1.SetTime(time);
	m_Interpolator2.SetTime(time);

	float width = m_Interpolator1.GetValue();
	float height = m_Interpolator2.GetValue();

	Rect rectOut;
	rectOut.left = rect->left + rect->GetWidth()/2-width*rect->GetWidth()/2;
	rectOut.top = rect->top + rect->GetHeight()/2-height*rect->GetHeight()/2;
	rectOut.right = rectOut.left + width*rect->GetWidth();
	rectOut.bottom = rectOut.top + height*rect->GetHeight();

	*rect = rectOut;
}

void TransitionTVCenter::CalcTransition(const Texture& texture,Rect* rect,Color* color,float time)
{
	m_Interpolator1.SetTime(time);

	float width = m_Interpolator1.GetValue();
	float height = width;

	Rect rectOut;
	rectOut.left = rect->left + rect->GetWidth()/2-width*rect->GetWidth()/2;
	rectOut.top = rect->top + rect->GetHeight()/2-height*rect->GetHeight()/2;
	rectOut.right = rectOut.left + width*rect->GetWidth();
	rectOut.bottom = rectOut.top + height*rect->GetHeight();

	*rect = rectOut;
}

void TransitionStretchLeft::CalcTransition(const Texture& texture,Rect* rect,Color* color,float time)
{
	m_Interpolator1.SetTime(time);

	float width = m_Interpolator1.GetValue();

	Rect rectOut;
	rectOut.left = rect->left;
	rectOut.top = rect->top;
	rectOut.right = rect->left + width*rect->GetWidth();
	rectOut.bottom = rect->bottom;

	*rect = rectOut;
}

void TransitionStretchTop::CalcTransition(const Texture& texture,Rect* rect,Color* color,float time)
{
	m_Interpolator1.SetTime(time);

	float hieght = m_Interpolator1.GetValue();

	Rect rectOut;
	rectOut.left = rect->left;
	rectOut.top = rect->top;
	rectOut.right = rect->right;
	rectOut.bottom = rect->top + hieght*rect->GetHeight();

	*rect = rectOut;
}

void TransitionStretchRight::CalcTransition(const Texture& texture,Rect* rect,Color* color,float time)
{
	m_Interpolator1.SetTime(time);

	float width = m_Interpolator1.GetValue();

	Rect rectOut;
	rectOut.left = rect->right - width*rect->GetWidth();
	rectOut.top = rect->top;
	rectOut.right = rect->right;
	rectOut.bottom = rect->bottom;

	*rect = rectOut;
}

void TransitionStretchBottom::CalcTransition(const Texture& texture,Rect* rect,Color* color,float time)
{
	m_Interpolator1.SetTime(time);

	float height = m_Interpolator1.GetValue();

	Rect rectOut;
	rectOut.left = rect->left;
	rectOut.top = rect->bottom - height*rect->GetHeight();
	rectOut.right = rect->right;
	rectOut.bottom = rect->bottom;

	*rect = rectOut;
}

void TransitionStretchInLeft::CalcTransition(const Texture& texture,Rect* rect,Color* color,float time)
{
	m_Interpolator1.SetTime(time);
	m_Interpolator2.SetTime(time);

	float width = m_Interpolator1.GetValue();
	float height = m_Interpolator2.GetValue();

	Rect rectOut;
	rectOut.left = rect->left;
	rectOut.top = rect->top + rect->GetHeight() / 2 - height * rect->GetHeight() / 2;
	rectOut.right = rectOut.left + width * rect->GetWidth();
	rectOut.bottom = rectOut.top + height * rect->GetHeight();

	*rect = rectOut;
}

void TransitionStretchInTop::CalcTransition(const Texture& texture,Rect* rect,Color* color,float time)
{
	m_Interpolator1.SetTime(time);
	m_Interpolator2.SetTime(time);

	float width = m_Interpolator1.GetValue();
	float height = m_Interpolator2.GetValue();

	Rect rectOut;
	rectOut.left = rect->left + rect->GetWidth() / 2 - width * rect->GetWidth() / 2;
	rectOut.top = rect->top;
	rectOut.right = rectOut.left + width * rect->GetWidth();
	rectOut.bottom = rectOut.top  + height * rect->GetHeight();

	*rect = rectOut;
}

void TransitionSlideInTop::CalcTransition(const Texture& texture,Rect* rect,Color* color,float time)
{
	m_Interpolator1.SetTime(time);

	float height = m_Interpolator1.GetValue();

	Rect rectOut;
	rectOut.left = rect->left;
	rectOut.top = rect->top - rect->GetHeight() * height;
	rectOut.right = rect->right;
	rectOut.bottom = rectOut.top + rect->GetHeight();

	*rect = rectOut;
}

void TransitionSlideInBottom::CalcTransition(const Texture& texture,Rect* rect,Color* color,float time)
{
	m_Interpolator1.SetTime(time);

	float height = m_Interpolator1.GetValue();

	Rect rectOut;
	rectOut.left = rect->left;
	rectOut.bottom = rect->bottom + rect->GetHeight() * height;
	rectOut.top = rectOut.bottom - rect->GetHeight();
	rectOut.right = rect->right;

	*rect = rectOut;
}

void TransitionSlideInLeft::CalcTransition(const Texture& texture,Rect* rect,Color* color,float time)
{
	m_Interpolator1.SetTime(time);

	float width = m_Interpolator1.GetValue();

	Rect rectOut;
	rectOut.left = rect->left + rect->GetWidth() * width;
	rectOut.top = rect->top;
	rectOut.right = rectOut.left + rect->GetWidth();
	rectOut.bottom = rect->bottom;

	*rect = rectOut;
}

void TransitionSlideInRight::CalcTransition(const Texture& texture,Rect* rect,Color* color,float time)
{
	m_Interpolator1.SetTime(time);

	float width = m_Interpolator1.GetValue();

	Rect rectOut;
	rectOut.right = rect->right - rect->GetWidth() * width;
	rectOut.left = rectOut.right - rect->GetWidth();
	rectOut.top = rect->top;
	rectOut.bottom = rect->bottom;

	*rect = rectOut;
}

#pragma warning(default : 4100)