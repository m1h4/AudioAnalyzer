// Copyright 2004/2006 Marko Mihovilic

#pragma once

class nAABB2
{
public:
	nAABB2(void) { /*SetEmpty();*/ }
	nAABB2(const Vector2& center,float width,float height) { Set(center,width,height); }
	nAABB2(const Vector2& v1,const Vector2& v2) { Set(v1,v2); }
	nAABB2(const nAABB2& other) { Set(other); }

	void Set(const Vector2& center,float width,float height) { x = center.x; y = center.y; cx = width / 2.0f; cy = height / 2.0f; }
	void Set(const Vector2& v1,const Vector2& v2) { x = (v1.x + v2.x) / 2.0f; y = (v1.y + v2.y) / 2.0f; cx = fabs(v1.x - v2.x) / 2.0f; cy = fabs(v1.y - v2.y) / 2.0f; }
	void Set(const nAABB2& other) { Set(other); }
	void SetEmpty(void) { x = y = cx = cy = 0.0f; }
	bool IsEmpty(void) const { return x == 0.0f && y == 0.0f && cx == 0.0f && cy == 0.0f; }

	bool PointIn(const Vector2& point) const { return fabs(x - point.x) <= cx && fabs(y - point.y) <= cy; }

	// Operators
	bool operator == (const nAABB2& other) const { return x == other.x && y == other.y && cx == other.cx && cy == other.cx; }
	bool operator != (const nAABB2& other) const { return x != other.x && y != other.y && cx != other.cx && cy != other.cx; }
	bool operator < (const nAABB2& other) const { return x < other.x && y < other.y && cx < other.cx && cy < other.cx; }
	bool operator > (const nAABB2& other) const { return x > other.x && y > other.y && cx > other.cx && cy > other.cx; }
	bool operator <= (const nAABB2& other) const { return x <= other.x && y <= other.y && cx <= other.cx && cy <= other.cx; }
	bool operator >= (const nAABB2& other) const { return x >= other.x && y >= other.y && cx >= other.cx && cy >= other.cx; }

	// Float array conversion
	operator float*(void) { return m; }
	operator const float*(void) const { return m; }

public:
	union 
	{
        struct 
		{
        	float x;
			float y;
			float cx;
			float cy;
        };

        float m[4];
    };
};

inline bool AABB2Overlap(const nAABB2& box1,const nAABB2& box2)
{
	return fabs(box1.x - box2.x) <= box1.cx + box2.cx && fabs(box1.y - box2.y) <= box1.cy + box2.cy;
}