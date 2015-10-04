#pragma once

#include "Transition.h"

#include "LinearInterpolator.h"
#include "CubicInterpolator.h"
#include "CatmullRomInterpolator.h"
#include "HermiteInterpolator.h"

class TransitionTVHorizontal : public Transition
{
public:
	TransitionTVHorizontal(void) { SetName(L"TV Horizontal"); 
	
		m_Interpolator1.AddState(0.0f,0.0f);
		m_Interpolator1.AddState(1.0f,1.0f);

		m_Interpolator2.AddState(0.0f,0.01f);
		m_Interpolator2.AddState(1.0f,0.01f);
		m_Interpolator2.AddState(1.5f,1.0f);
	}

	void CalcTransition(const Texture& texture,Rect* rect,Color* color,float time);

protected:
	CatmullRomInterpolator		m_Interpolator1;
	CatmullRomInterpolator		m_Interpolator2;
};

class TransitionTVVertical : public Transition
{
public:
	TransitionTVVertical(void) { SetName(L"TV Vertical"); 
	
		m_Interpolator1.AddState(0.0f,0.01f);
		m_Interpolator1.AddState(1.0f,0.01f);
		m_Interpolator1.AddState(1.5f,1.0f);

		m_Interpolator2.AddState(0.0f,0.0f);
		m_Interpolator2.AddState(1.0f,1.0f);
	}

	void CalcTransition(const Texture& texture,Rect* rect,Color* color,float time);

protected:
	CatmullRomInterpolator		m_Interpolator1;
	CatmullRomInterpolator		m_Interpolator2;
};

class TransitionTVCenter : public Transition
{
public:
	TransitionTVCenter(void) { SetName(L"TV Center"); 
	
		m_Interpolator1.AddState(0.0f,0.0f);
		m_Interpolator1.AddState(1.0f,1.0f);
	}

	void CalcTransition(const Texture& texture,Rect* rect,Color* color,float time);

protected:
	CatmullRomInterpolator		m_Interpolator1;
};

class TransitionStretchLeft : public Transition
{
public:
	TransitionStretchLeft(void) { SetName(L"Stretch Left"); 
	
		m_Interpolator1.AddState(0.0f,0.0f);
		m_Interpolator1.AddState(1.0f,1.0f);
	}

	void CalcTransition(const Texture& texture,Rect* rect,Color* color,float time);

protected:
	CatmullRomInterpolator		m_Interpolator1;
};

class TransitionStretchTop : public Transition
{
public:
	TransitionStretchTop(void) { SetName(L"Stretch Top"); 
	
		m_Interpolator1.AddState(0.0f,0.0f);
		m_Interpolator1.AddState(1.0f,1.0f);
	}

	void CalcTransition(const Texture& texture,Rect* rect,Color* color,float time);

protected:
	CatmullRomInterpolator		m_Interpolator1;
};

class TransitionStretchRight : public Transition
{
public:
	TransitionStretchRight(void) { SetName(L"Stretch Right"); 
	
		m_Interpolator1.AddState(0.0f,0.0f);
		m_Interpolator1.AddState(1.0f,1.0f);
	}

	void CalcTransition(const Texture& texture,Rect* rect,Color* color,float time);

protected:
	CatmullRomInterpolator		m_Interpolator1;
};

class TransitionStretchBottom : public Transition
{
public:
	TransitionStretchBottom(void) { SetName(L"Stretch Bottom"); 
	
		m_Interpolator1.AddState(0.0f,0.0f);
		m_Interpolator1.AddState(1.0f,1.0f);
	}

	void CalcTransition(const Texture& texture,Rect* rect,Color* color,float time);

protected:
	CatmullRomInterpolator		m_Interpolator1;
};

class TransitionStretchInLeft : public Transition
{
public:
	TransitionStretchInLeft(void) { SetName(L"Stretch-In Left");

		m_Interpolator1.AddState(0.0f,0.0f);
		m_Interpolator1.AddState(1.0f,1.0f);

		m_Interpolator2.AddState(0.0f,0.01f);
		m_Interpolator2.AddState(1.0f,0.01f);
		m_Interpolator2.AddState(1.5f,1.0f);
	}

	void CalcTransition(const Texture& texture,Rect* rect,Color* color,float time);

protected:
	CatmullRomInterpolator		m_Interpolator1;
	CatmullRomInterpolator		m_Interpolator2;
};

class TransitionStretchInTop : public Transition
{
public:
	TransitionStretchInTop(void) { SetName(L"Stretch-In Top");
		
		m_Interpolator1.AddState(0.0f,0.01f);
		m_Interpolator1.AddState(1.0f,0.01f);
		m_Interpolator1.AddState(1.5f,1.0f);

		m_Interpolator2.AddState(0.0f,0.0f);
		m_Interpolator2.AddState(1.0f,1.0f);
	}

	void CalcTransition(const Texture& texture,Rect* rect,Color* color,float time);

protected:
	CatmullRomInterpolator		m_Interpolator1;
	CatmullRomInterpolator		m_Interpolator2;
};

class TransitionSlideInTop : public Transition
{
public:
	TransitionSlideInTop(void) { SetName(L"Slide-In Top");
		
		m_Interpolator1.AddState(0.0f,1.0f);
		m_Interpolator1.AddState(1.0f,0.0f);
	}

	void CalcTransition(const Texture& texture,Rect* rect,Color* color,float time);

protected:
	CatmullRomInterpolator		m_Interpolator1;
};

class TransitionSlideInBottom : public Transition
{
public:
	TransitionSlideInBottom(void) { SetName(L"Slide-In Bottom");
		
		m_Interpolator1.AddState(0.0f,1.0f);
		m_Interpolator1.AddState(1.0f,0.0f);
	}

	void CalcTransition(const Texture& texture,Rect* rect,Color* color,float time);

protected:
	CatmullRomInterpolator		m_Interpolator1;
};

class TransitionSlideInLeft : public Transition
{
public:
	TransitionSlideInLeft(void) { SetName(L"Slide-In Left");
		
		m_Interpolator1.AddState(0.0f,1.0f);
		m_Interpolator1.AddState(1.0f,0.0f);
	}

	void CalcTransition(const Texture& texture,Rect* rect,Color* color,float time);

protected:
	CatmullRomInterpolator		m_Interpolator1;
};

class TransitionSlideInRight : public Transition
{
public:
	TransitionSlideInRight(void) { SetName(L"Slide-In Right");
		
		m_Interpolator1.AddState(0.0f,1.0f);
		m_Interpolator1.AddState(1.0f,0.0f);
	}

	void CalcTransition(const Texture& texture,Rect* rect,Color* color,float time);

protected:
	CatmullRomInterpolator		m_Interpolator1;
};