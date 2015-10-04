// Copyright 2004/2006 Marko Mihovilic

#include "Globals.h"
#include "Kernel.h"
#include "Color.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void Color::Lerp(const Color& dest,float s)
{
	m[0] = m[0] + s * (dest.m[0] - m[0]);
	m[1] = m[1] + s * (dest.m[1] - m[1]);
	m[2] = m[2] + s * (dest.m[2] - m[2]);
	m[3] = m[3] + s * (dest.m[3] - m[3]);
}

void Color::Cubic(const Color& dest,float s)
{
	float s1 = 2.0f * powf(s,3.0f);
	float s2 = 3.0f * powf(s,2.0f);

	float s12 = s1 - s2 + 1.0f;
	float s21 = s2 - s1;

	m[0] = m[0] * s12 + dest.m[0] * s21;
	m[1] = m[1] * s12 + dest.m[1] * s21;
	m[2] = m[2] * s12 + dest.m[2] * s21;
	m[3] = m[3] * s12 + dest.m[3] * s21;
}

void Color::Cosine(const Color& dest,float s)
{
	float f1 = (1.0f - cosf(M_PI * s)) * 0.5f;

	m[0] = m[0] * (1.0f - f1) + dest.m[0] * f1;
	m[1] = m[1] * (1.0f - f1) + dest.m[1] * f1;
	m[2] = m[2] * (1.0f - f1) + dest.m[2] * f1;
	m[3] = m[3] * (1.0f - f1) + dest.m[3] * f1;
}

Color Color::operator+(const Color& other) const
{
	float color[4];
	
	GetKernel()->GetProcessor()->AddArr(color,m,other.m);
	
	return Color(color);
}

Color Color::operator-(const Color& other) const
{
	float color[4];
	
	GetKernel()->GetProcessor()->SubArr(color,m,other.m);
	
	return Color(color);
}

Color& Color::operator+=(const Color& other)
{
	GetKernel()->GetProcessor()->AddArr(m,m,other.m);
	
	return *this;
}

Color& Color::operator-=(const Color& other)
{
	GetKernel()->GetProcessor()->SubArr(m,m,other.m);
	
	return *this;
}

Color Color::operator*(const float scalar) const
{
	float color[4];
	
	GetKernel()->GetProcessor()->Mul(color,m,scalar);
	
	return Color(color);
}

Color Color::operator/(const float scalar) const
{
	float color[4];
	
	GetKernel()->GetProcessor()->Div(color,m,scalar);
	
	return Color(color);
}

Color& Color::operator*=(const float scalar)
{
	GetKernel()->GetProcessor()->Mul(m,m,scalar);
	
	return *this;
}

Color& Color::operator/=(const float scalar)
{
	GetKernel()->GetProcessor()->Div(m,m,scalar);
	
	return *this;
}

Color Color::operator*(const Color& other) const
{
	float color[4];
	
	GetKernel()->GetProcessor()->MulArr(color,m,other.m);
	
	return Color(color);
}

Color& Color::operator*=(const Color& other)
{
	GetKernel()->GetProcessor()->MulArr(m,m,other.m);
	
	return *this;
}