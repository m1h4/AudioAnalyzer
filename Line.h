// Copyright 2004/2006 Marko Mihovilic

#pragma once

#include "Vector.h"

class Line
{
public:
	Line(void) { /*SetEmpty();*/ }
	Line(float a,float b,float c) { Set(a,b,c); }
	Line(const Line& other) { Set(other); }
	Line(const Vector2& start,const Vector2& end) { Set(start,end); }
	//Line(const Vector2& normal,const Vector2& v1) { Set(normal,v1); }
	Line(const Vector2& normal,float distance) { Set(normal,distance); }

	void Set(float a,float b,float c) { this->a = a; this->b = b; this->c = c; }
	void Set(const Line& other) { a = other.a; b = other.b; c = other.c; }
	void Set(const Vector2& start,const Vector2& end) { SetFromNormalPoint(Vector2Normalize(Vector2Perpendicular(start - end)),start); }
	void SetFromNormalPoint(const Vector2& normal,const Vector2& v1) { Set(normal,Vector2Dot(normal,v1)); }
	void Set(const Vector2& normal,float distance) { a = normal.x; b = normal.y; c = -distance; }
	void SetEmpty(void) { a = b = c = 0.0f; }
	bool IsEmpty(void) const { return a == 0.0f && b == 0.0f && c == 0.0f; }

	float Dot(const Vector2& vector) const { return a * vector.x + b * vector.y + c; }

	// Operators
	bool operator == (const Line& other) const { return a == other.a && b == other.b && c == other.c; }
	bool operator != (const Line& other) const { return a != other.a && b != other.b && c != other.c; }
	bool operator < (const Line& other) const { return a < other.a && b < other.b && c < other.c; }
	bool operator > (const Line& other) const { return a > other.a && b > other.b && c > other.c; }
	bool operator <= (const Line& other) const { return a <= other.a && b <= other.b && c <= other.c; }
	bool operator >= (const Line& other) const { return a >= other.a && b >= other.b && c >= other.c; }

	// Float array conversion
	operator float*(void) { return m; }
	operator const float*(void) const { return m; }

public:
	union 
	{
        struct 
		{
        	float a;
			float b;
			float c;
        };

        float m[3];
    };
};

inline bool LineIntersect(const Line& line1,const Line& line2,const Vector2& line1Start,const Vector2& line1End,const Vector2& line2Start,const Vector2& line2End)
{
	return line1.Dot(line2Start) >= 0 != line1.Dot(line2End) >= 0 && line2.Dot(line1Start) >= 0 != line2.Dot(line1End) >= 0;
}

inline bool LineIntersection(const Line& line1,const Line& line2,Vector2* intersection)
{
	//return Vector2(-line1.c * line2.b + line2.c * line1.b,-line1.a * line2.c + line2.a * line1.c) / (line1.a * line2.b - line2.a * line1.b);

	float determinant = line1.a * line2.b - line2.a * line1.b;

	if(determinant == 0.0f)	// TODO Maybe put a epsilon value here
		return false;

	*intersection = Vector2(-line1.c * line2.b + line2.c * line1.b,-line1.a * line2.c + line2.a * line1.c) / determinant;

	return true;
}