// Copyright 2004/2006 Marko Mihovilic

#pragma once

#include "Point.h"
#include "Size.h"

class Rect
{
public:
	Rect() { /*this->SetEmpty();*/ }
	Rect(long left,long top,long right,long bottom) { this->Set(left,top,right,bottom); }
	Rect(long width,long height) { this->Set(width,height); }
	Rect(const Point& topLeft,const Size& size) { this->Set(topLeft,size); }
	Rect(const Point& topLeft,const Point& bottomRight) { this->Set(topLeft,bottomRight); }
	Rect(const Rect& rect) { this->Set(rect); }

	long GetWidth() const { return right - left; }
	long GetHeight() const { return bottom - top; }

	void SetWidth(long width) { this->right = this->left + width; }
	void SetHeight(long height) { this->bottom = this->top + height; }
	void SetWidthRight(long width) { this->left = this->right - width; }
	void SetHeightRight(long height) { this->top = this->bottom - height; }

	void Set(long left,long top,long right,long bottom) { this->left = left; this->top = top; this->right = right; this->bottom = bottom; }
	void Set(long width,long height) { left = 0; top = 0; right = width; bottom = height; }
	void Set(const Point& topLeft,const Size& size) { left = topLeft.x; top = topLeft.y; right = left + size.cx; bottom = top + size.cy; }
	void Set(const Point& topLeft,const Point& bottomRight) { left = topLeft.x; top = topLeft.y; right = bottomRight.x; bottom = bottomRight.y; }
	void Set(const Rect& rect) { *this = rect; }
	void SetEmpty() { left = top = right = bottom = 0; }
	bool IsEmpty() const { if(right <= left) return true; if(bottom <= top) return true; return false; }

	void AdjustWidth(void) { if(right < left) { long tmp = left; left = right; right = tmp; } }
	void AdjustHeight(void) { if(bottom < top) { long tmp = top; top = bottom; bottom = tmp; } }
	void Adjust(void) { AdjustWidth(); AdjustHeight(); }

	void Inflate(long dx,long dy) { left -= dx; right += dx; top -= dy; bottom += dy; }
	void Inflate(long d) { this->Inflate(d,d); }

	void Offset(long dx,long dy) { left += dx; right += dx; top += dy; bottom += dy; }
	void Offset(long d) { this->Offset(d,d); }

	void Scale(float dx,float dy) { long cx = (right + left) / 2; long cy = (bottom + top) / 2; long width = (right - left) / 2; long height = (bottom - top) / 2; left = cx - width * dx; right = cx + width * dx; top = cy - height * dy; bottom = cy + height * dy; }
	void Scale(float d) { Scale(d,d); }

	bool PointIn(const Point& point) const { if(point.x < left || point.x > right) return false; if(point.y < top || point.y > bottom) return false; return true; }

	Point GetCenter() const { return Point((right+left)/2,(bottom+top)/2); }
	Size GetSize() const { return Size(right-left,bottom-top); }

	// TODO You should check if the result rect is valid if there is a chance that the two rects don't intersect!
	void Intersect(const Rect& rect) { left = max(left,rect.left); right = min(right,rect.right); top = max(top,rect.top); bottom = min(bottom,rect.bottom); /*return RectIsEmpty(*rect);*/ }

	Rect operator+(const Point& point) const { return Rect(left+point.x,top+point.y,right+point.x,bottom+point.y); }
	Rect operator-(const Point& point) const { return Rect(left-point.x,top-point.y,right-point.x,bottom-point.y); }

	Rect& operator+=(const Point& point) { left += point.x; top += point.y; right += point.x; bottom += point.y; return *this; }
	Rect& operator-=(const Point& point) { left -= point.x; top -= point.y; right -= point.x; bottom -= point.y; return *this; }

	bool operator==(const Rect& other) const { if(bottom != other.bottom) return false; if(left != other.left) return false; if(right != other.right) return false; if(top != other.top) return false; return true; }
	bool operator!=(const Rect& other) const { if(bottom == other.bottom) return false; if(left == other.left) return false; if(right == other.right) return false; if(top == other.top) return false; return true; }

	// Long array conversion
	operator long*(void) { return m; }
	operator const long*(void) const { return m; }

public:
	union 
	{
        struct 
		{
        	long left;
			long top;
			long right;
			long bottom;
        };

        long m[4];
    };
};