// Copyright 2004/2006 Marko Mihovilic

#pragma once

#ifdef ALIGN_DATA
__declspec(align(16))
#endif
class Color
{
public:
	Color() { /*SetEmpty();*/ }
	Color(float a,float r,float g,float b) { Set(a,r,g,b); }
	Color(float r,float g,float b) { Set(r,g,b); }
	Color(float a,float value) { Set(a,value); }
	Color(float value) { Set(value); }
	Color(const Color& other) { Set(other); }
	Color(float a,const Color& other) { Set(a,other); }
	Color(const float* m) { Set(m); }

	void Set(float a,float r,float g,float b) { this->a = a; this->r = r; this->g = g; this->b = b; }
	void Set(float r,float g,float b) { this->a = 1.0f; this->r = r; this->g = g; this->b = b; }
	void Set(float a,float value) { this->a = a; r = g = b = value; }
	void Set(float value)  { a = r = g = b = value; }
	void Set(const Color& other) { a = other.a; r = other.r; g = other.g; b = other.b; }
	void Set(float a,const Color& other) { this->a = a; r = other.r; g = other.g; b = other.b; }
	void Set(const float* m) { this->m[0] = m[0]; this->m[1] = m[1]; this->m[2] = m[2]; this->m[3] = m[3]; }
	
	void SetEmpty(void) { a = r = g = b = 0.0f; }
	bool IsEmpty(void) { if(a != 0.0f) return false; if(r != 0.0f) return false; if(g != 0.0f) return false; if(b != 0.0f) return false; return true; }

	bool operator==(const Color& other) const { return a == other.a && r == other.r && g == other.g && b == other.b; }
	bool operator!=(const Color& other) const { return a != other.a && r != other.r && g != other.g && b != other.b; }
	bool operator<(const Color& other) const { return a < other.a && r < other.r && g < other.g && b < other.b; }
	bool operator>(const Color& other) const { return a > other.a && r > other.r && g > other.g && b > other.b; }
	bool operator<=(const Color& other) const { return a <= other.a && r <= other.r && g <= other.g && b <= other.b; }
	bool operator>=(const Color& other) const { return a >= other.a && r >= other.r && g >= other.g && b >= other.b; }

	void Clamp(float mn,float mx) { a = min(max(a,mn),mx); r = min(max(r,mn),mx); g = min(max(g,mn),mx); b = min(max(b,mn),mx); }

	void Lerp(const Color& dest,float factor);
	void Cubic(const Color& dest,float factor);
	void Cosine(const Color& dest,float factor);
	
	void Brighten(float s) { Lerp(Color(a,1.0f,1.0f,1.0f),s); }
	void Darken(float s) { Lerp(Color(a,0.0f,0.0f,0.0f),s); }
	
	void Saturation(float s) { float v = (r+g+b)/3.0f; Lerp(Color(a,v,v,v),s); }
	void Negative(void) { r = 1.0f-r; g = 1.0f-g; b = 1.0f-b; }
	void Grayscale(void) { float v = (r+g+b)/3.0f; r = g = b = v; }
	void Shadow(void) { a *= 0.1f; r = g = b = 0.0f; }
	void Gamma(float factor) { r = powf(r,factor); g = powf(g,factor); b = powf(b,factor); }

	operator unsigned long() const { return (unsigned long)(((((unsigned long)((a)*255.0f))&0xFF)<<24)|((((unsigned long)((r)*255.0f))&0xFF)<<16)|((((unsigned long)((g)*255.0f))&0xFF)<<8)|(((unsigned long)((b)*255.0f))&0xFF)); }

	Color operator+(const Color& other) const;
	Color operator-(const Color& other) const;

	Color& operator+=(const Color& other);
	Color& operator-=(const Color& other);

	Color operator*(const float scalar) const;
	Color operator/(const float scalar) const;

	Color& operator*=(const float scalar);
	Color& operator/=(const float scalar);

	Color operator*(const Color& other) const;
	Color& operator*=(const Color& other);

	// Float array conversion
	operator float*(void) { return m; }
	operator const float*(void) const { return m; }

public:
	union 
	{
		struct 
		{
			float a;
			float r;
			float g;
			float b;
		};

		float m[4];
	};
};

inline Color ColorGamma(const Color& color,float factor)
{
	return Color(color.a,powf(color.r,factor),powf(color.g,factor),powf(color.b,factor));
}

inline Color ColorDarken(const Color& color,float factor)
{
	return Color(color.a,factor * color.r,factor * color.g,factor * color.b);
}

inline Color ColorLighten(const Color& color,float factor)
{
	return Color(color.a,color.r + factor * (1.0f - color.r),color.g + factor * (1.0f - color.g),color.b + factor * (1.0f - color.b));
}

inline Color ColorLerp(const Color& a,const Color& b,float factor)
{
	return Color(a.a + factor * (b.a - a.a),a.r + factor * (b.r - a.r),a.g + factor * (b.g - a.g),a.b + factor * (b.b - a.b));
}