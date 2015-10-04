// Copyright 2004/2006 Marko Mihovilic

#pragma once

class Point
{
public:
	Point() { /*this->SetEmpty();*/ }
	Point(long x,long y) { this->Set(x,y); }
	Point(long at) { this->Set(at); }
	Point(const Point& point) { this->Set(point); }

	void Set(long x,long y) { this->x = x; this->y = y; }
	void Set(long at) { x = at; y = at; }
	void Set(const Point& point) { *this = point; }
	void SetEmpty() { x = y = 0; }
	bool IsEmpty() const { if(x != 0) return false; if(y != 0) return false; return true; }

	void Offset(long dx,long dy) { x += dx; y += dy; }
	void Offset(long d) { this->Offset(d,d); }

	bool operator==(const Point& point) const { if(x != point.x) return false; if(y != point.y) return false; return true; }
	bool operator!=(const Point& point) const { if(x == point.x) return false; if(y == point.y) return false; return true; }

	Point operator+(const Point& point) const { return Point(x+point.x,y+point.y); }
	Point operator-(const Point& point) const { return Point(x-point.x,y-point.y); }

	Point& operator+=(const Point& point) { x += point.x; y += point.y; return *this; }
	Point& operator-=(const Point& point) { x -= point.x; y -= point.y; return *this; }

	// Long array conversion
	operator long*(void) { return m; }
	operator const long*(void) const { return m; }

public:
	union 
	{
        struct 
		{
        	long x;
			long y;
        };

        long m[2];
    };
};