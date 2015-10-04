// Copyright 2004/2006 Marko Mihovilic

#pragma once

class Size
{
public:
	Size() { /*this->SetEmpty();*/ }
	Size(long cx,long cy) { this->Set(cx,cy); }
	Size(long c) { this->Set(c); }
	Size(const Size& size) { this->Set(size); }

	void Set(long cx,long cy) { this->cx = cx; this->cy = cy; }
	void Set(long c) { this->Set(c,c); }
	void Set(const Size& size) { *this = size; }
	void SetEmpty() { cx = cy = 0; }
	bool IsEmpty() const { if(cx <= 0) return true; if(cy <= 0) return true; return false; }

	void Expand(long dx,long dy) { cx += dx; cy += dy; }
	void Expand(long d) { this->Expand(d,d); }

	bool operator==(const Size& size) const { if(cx != size.cx) return false; if(cy != size.cy) return false; return true; }
	bool operator!=(const Size& size) const { if(cx == size.cx) return false; if(cy == size.cy) return false; return true; }

	Size operator+(const Size& size) const { return Size(cx+size.cx,cy+size.cy); }
	Size operator-(const Size& size) const { return Size(cx-size.cx,cy-size.cy); }

	Size& operator+=(const Size& size) { cx += size.cx; cy += size.cy; return *this; }
	Size& operator-=(const Size& size) { cx -= size.cx; cy -= size.cy; return *this; }

	// Long array conversion
	operator long*(void) { return m; }
	operator const long*(void) const { return m; }

public:
	union 
	{
        struct 
		{
        	long cx;
			long cy;
        };

        long m[2];
    };
};