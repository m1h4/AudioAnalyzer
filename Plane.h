// Copyright 2004/2006 Marko Mihovilic

#pragma once

#include "Vector.h"

class Plane
{
public:
	Plane(void) { /*SetEmpty();*/ }
	Plane(float a,float b,float c,float d) { Set(a,b,c,d); }
	Plane(const Plane& other) { Set(other); }
	Plane(const Vector3& v1,const Vector3& v2,const Vector3& v3) { Set(v1,v2,v3); }
	Plane(const Vector3& normal,const Vector3& v1) { Set(normal,v1); }
	Plane(const Vector3& normal,float distance) { Set(normal,distance); }

	void Set(float a,float b,float c,float d) { this->a = a; this->b = b; this->c = c; this->d = d; }
	void Set(const Plane& other) { a = other.a; b = other.b; c = other.c; d = other.d; }
	void Set(const Vector3& v1,const Vector3& v2,const Vector3& v3) { Set(Vector3Normalize(Vector3Cross(v2 - v1,v3 - v1)),v1); }
	void Set(const Vector3& normal,const Vector3& v1) { Set(normal,Vector3Dot(normal,v1)); }
	void Set(const Vector3& normal,float distance) { a = normal.x; b = normal.y; c = normal.z; d = -distance; }
	void SetEmpty(void) { a = b = c = d = 0.0f; }
	bool IsEmpty(void) const { return a == 0.0f && b == 0.0f && c == 0.0f && d == 0.0f; }

	float Dot(const Vector3& vector) const { return a * vector.x + b * vector.y + c * vector.z + d; }

	// Operators
	bool operator == (const Plane& other) const { return a == other.a && b == other.b && c == other.c && d == other.d; }
	bool operator != (const Plane& other) const { return a != other.a && b != other.b && c != other.c && d != other.d; }
	bool operator < (const Plane& other) const { return a < other.a && b < other.b && c < other.c && d < other.d; }
	bool operator > (const Plane& other) const { return a > other.a && b > other.b && c > other.c && d > other.d; }
	bool operator <= (const Plane& other) const { return a <= other.a && b <= other.b && c <= other.c && d <= other.d; }
	bool operator >= (const Plane& other) const { return a >= other.a && b >= other.b && c >= other.c && d >= other.d; }

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
			float d;
        };

        float m[4];
    };
};