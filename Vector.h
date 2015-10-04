// Copyright 2004/2006 Marko Mihovilic

#pragma once

class Vector3
{
public:
	Vector3(void) { /*SetEmpty();*/ }
	Vector3(float x,float y,float z) { Set(x,y,z); }
	Vector3(float xyz) { Set(xyz); }
	Vector3(const float* m) { Set(m); }
	Vector3(const Vector3& other) { Set(other); }

	void Set(float x,float y,float z) { this->x = x; this->y = y; this->z = z; }
	void Set(float xyz) { x = y = z = xyz; }
	void Set(const float* m) { this->m[0] = m[0]; this->m[1] = m[1]; this->m[2] = m[2]; }
	void Set(const Vector3& other) { x = other.x; y = other.y; z = other.z; }
	void SetEmpty(void) { x = y = z = 0.0f; }
	bool IsEmpty(void) const { return x == 0.0f && y == 0.0f && z == 0.0f; }

	float Length(void) const { return sqrtf(x*x+y*y+z*z); }
	float LengthSqr(void) const { return x*x+y*y+z*z; }

	void Normalize(void) { float l = this->Length(); x /= l; y /= l; z /= l; }
	void Lerp(const Vector3& dest,float s) { x = lerpf(x,dest.x,s); y = lerpf(y,dest.y,s); z = lerpf(z,dest.z,s); }

    // Operators
	Vector3 operator + (const Vector3& other) const { return Vector3(x+other.x,y+other.y,z+other.z); }
	Vector3 operator - (const Vector3& other) const { return Vector3(x-other.x,y-other.y,z-other.z); }
	Vector3 operator * (const Vector3& other) const { return Vector3(x*other.x,y*other.y,z*other.z); }
	Vector3 operator / (const Vector3& other) const { return Vector3(x/other.x,y/other.y,z/other.z); }

	Vector3 operator + (float scalar) const { return Vector3(x+scalar,y+scalar,z+scalar); }
	Vector3 operator - (float scalar) const { return Vector3(x-scalar,y-scalar,z-scalar); }
	Vector3 operator * (float scalar) const { return Vector3(x*scalar,y*scalar,z*scalar); }
	Vector3 operator / (float scalar) const { return Vector3(x/scalar,y/scalar,z/scalar); }
    
	Vector3& operator += (const Vector3& other) { x += other.x; y += other.y; z += other.z; return *this; }
	Vector3& operator -= (const Vector3& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
	Vector3& operator *= (const Vector3& other) { x *= other.x; y *= other.y; z *= other.z; return *this; }
	Vector3& operator /= (const Vector3& other) { x /= other.x; y /= other.y; z /= other.z; return *this; }

	Vector3& operator += (float scalar) { x += scalar; y += scalar; z += scalar; return *this; }
	Vector3& operator -= (float scalar) { x -= scalar; y -= scalar; z -= scalar; return *this; }
	Vector3& operator *= (float scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }
	Vector3& operator /= (float scalar) { x /= scalar; y /= scalar; z /= scalar; return *this; }

	friend Vector3 operator * (float,const Vector3&);

	Vector3 operator-(void) const { return Vector3(-x,-y,-z); }

	bool operator == (const Vector3& other) const { return x == other.x && y == other.y && z == other.z; }
	bool operator != (const Vector3& other) const { return x != other.x && y != other.y && z != other.z; }
	bool operator < (const Vector3& other) const { return x < other.x && y < other.y && z < other.z; }
	bool operator > (const Vector3& other) const { return x > other.x && y > other.y && z > other.z; }
	bool operator <= (const Vector3& other) const { return x <= other.x && y <= other.y && z <= other.z; }
	bool operator >= (const Vector3& other) const { return x >= other.x && y >= other.y && z >= other.z; }

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
			float z;
        };

        float m[3];
    };
};

class Vector2
{
public:
	Vector2() { /*SetEmpty();*/ }
	Vector2(float x,float y) { Set(x,y); }
	Vector2(float xy) { Set(xy); }
	Vector2(const float* m) { Set(m); }
	Vector2(const Vector2& other) { Set(other); }

	void Set(float x,float y) { this->x = x; this->y = y; }
	void Set(float xy) { x = y = xy; }
	void Set(const float* m) { this->m[0] = m[0]; this->m[1] = m[1]; }
	void Set(const Vector2& other) { x = other.x; y = other.y; }
	void SetEmpty(void) { x = y = 0.0f; }
	bool IsEmpty(void) const { return x == 0.0f && y == 0.0f; }

	float Length(void) const { return sqrtf(x*x+y*y); }
	float LengthSqr(void) const { return x*x+y*y; }

	void Normalize(void) { float l = this->Length(); x /= l; y /= l; }
	void Lerp(const Vector2& dest,float s) { x = lerpf(x,dest.x,s); y = lerpf(y,dest.y,s); }

    // Operators
	Vector2 operator + (const Vector2& other) const { return Vector2(x+other.x,y+other.y); }
	Vector2 operator - (const Vector2& other) const { return Vector2(x-other.x,y-other.y); }
	Vector2 operator * (const Vector2& other) const { return Vector2(x*other.x,y*other.y); }
	Vector2 operator / (const Vector2& other) const { return Vector2(x/other.x,y/other.y); }

	Vector2 operator + (float scalar) const { return Vector2(x+scalar,y+scalar); }
	Vector2 operator - (float scalar) const { return Vector2(x-scalar,y-scalar); }
	Vector2 operator * (float scalar) const { return Vector2(x*scalar,y*scalar); }
	Vector2 operator / (float scalar) const { return Vector2(x/scalar,y/scalar); }
    
	Vector2& operator += (const Vector2& other) { x += other.x; y += other.y; return *this; }
	Vector2& operator -= (const Vector2& other) { x -= other.x; y -= other.y; return *this; }
	Vector2& operator *= (const Vector2& other) { x *= other.x; y *= other.y; return *this; }
	Vector2& operator /= (const Vector2& other) { x /= other.x; y /= other.y; return *this; }

	Vector2& operator += (float scalar) { x += scalar; y += scalar; return *this; }
	Vector2& operator -= (float scalar) { x -= scalar; y -= scalar; return *this; }
	Vector2& operator *= (float scalar) { x *= scalar; y *= scalar; return *this; }
	Vector2& operator /= (float scalar) { x /= scalar; y /= scalar; return *this; }

	friend Vector2 operator * (float,const Vector2&);

	Vector2 operator-(void) const { return Vector2(-x,-y); }

	bool operator == (const Vector2& other) const { return x == other.x && y == other.y; }
	bool operator != (const Vector2& other) const { return x != other.x && y != other.y; }
	bool operator < (const Vector2& other) const { return x < other.x && y < other.y; }
	bool operator > (const Vector2& other) const { return x > other.x && y > other.y; }
	bool operator <= (const Vector2& other) const { return x <= other.x && y <= other.y; }
	bool operator >= (const Vector2& other) const { return x >= other.x && y >= other.y; }

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
        };

        float m[2];
    };
};

class Vector4
{
public:
	Vector4(void) { /*SetEmpty();*/ }
	Vector4(float x,float y,float z,float w) { Set(x,y,z,w); }
	Vector4(float xyzw) { Set(xyzw); }
	Vector4(const float* m) { Set(m); }
	Vector4(__m128 xmm) { Set(xmm); }
	Vector4(const Vector4& other) { Set(other); }

	void Set(float x,float y,float z,float w) { xmm = _mm_set_ps(w,z,y,x); }
	void Set(float xyzw) { xmm = _mm_set_ps1(xyzw); }
	void Set(const float* m) { xmm = _mm_set_ps(m[3],m[2],m[1],m[0]); }
	void Set(__m128 xmm) { this->xmm = xmm; }
	void Set(const Vector4& other) { xmm = other; }
	void SetEmpty(void) { xmm = _mm_set_ps(0.0f,0.0f,0.0f,0.0f); }
	bool IsEmpty(void) const { return x == 0.0f && y == 0.0f && z == 0.0f; }

	float Length(void) const { Vector4 t = _mm_sqrt_ps(_mm_mul_ps(xmm,xmm)); return t.x + t.y + t.z + t.w; }
	float LengthSqr(void) const { Vector4 t = _mm_mul_ps(xmm,xmm); return t.x + t.y + t.z + t.w; }

	void Normalize(void) { __m128 t = _mm_set_ps1(Length()); xmm = _mm_div_ps(xmm,t); }
	void Lerp(const Vector4& dest,float s) { xmm = _mm_add_ps(xmm,_mm_mul_ps(_mm_sub_ps(dest,xmm),_mm_set_ps1(s))); }

    // Operators
	Vector4 operator + (const Vector4& other) const { return _mm_add_ps(xmm,other); }
	Vector4 operator - (const Vector4& other) const { return _mm_sub_ps(xmm,other); }
	Vector4 operator * (const Vector4& other) const { return _mm_mul_ps(xmm,other); }
	Vector4 operator / (const Vector4& other) const { return _mm_div_ps(xmm,other); }

	Vector4 operator + (float scalar) const { return _mm_add_ps(xmm,_mm_set_ps1(scalar)); }
	Vector4 operator - (float scalar) const { return _mm_sub_ps(xmm,_mm_set_ps1(scalar)); }
	Vector4 operator * (float scalar) const { return _mm_mul_ps(xmm,_mm_set_ps1(scalar)); }
	Vector4 operator / (float scalar) const { return _mm_div_ps(xmm,_mm_set_ps1(scalar)); }
    
	Vector4& operator += (const Vector4& other) { return *this = _mm_add_ps(xmm,other); }
	Vector4& operator -= (const Vector4& other) { return *this = _mm_sub_ps(xmm,other); }
	Vector4& operator *= (const Vector4& other) { return *this = _mm_mul_ps(xmm,other); }
	Vector4& operator /= (const Vector4& other) { return *this = _mm_div_ps(xmm,other); }

	Vector4& operator += (float scalar) { return *this = _mm_add_ps(xmm,_mm_set_ps1(scalar)); }
	Vector4& operator -= (float scalar) { return *this = _mm_sub_ps(xmm,_mm_set_ps1(scalar)); }
	Vector4& operator *= (float scalar) { return *this = _mm_mul_ps(xmm,_mm_set_ps1(scalar)); }
	Vector4& operator /= (float scalar) { return *this = _mm_div_ps(xmm,_mm_set_ps1(scalar)); }

	friend Vector4 operator * (float,const Vector4&);

	Vector4 operator-(void) const { return Vector4(-x,-y,-z,-w); }

	bool operator == (const Vector4& other) const { return x == other.x && y == other.y && z == other.z && w == other.w; }
	bool operator != (const Vector4& other) const { return x != other.x && y != other.y && z != other.z && w != other.w; }
	bool operator < (const Vector4& other) const { return x < other.x && y < other.y && z < other.z && w < other.w; }
	bool operator > (const Vector4& other) const { return x > other.x && y > other.y && z > other.z && w > other.w; }
	bool operator <= (const Vector4& other) const { return x <= other.x && y <= other.y && z <= other.z && w <= other.w; }
	bool operator >= (const Vector4& other) const { return x >= other.x && y >= other.y && z >= other.z && w >= other.w; }

	// Float array conversion
	operator float*(void) { return m; }
	operator const float*(void) const { return m; }

	operator __m128(void) const { return xmm; }

public:
	union 
	{
        struct 
		{
        	float x;
			float y;
			float z;
			float w;
        };

        float m[4];

		__m128 xmm;
    };
};

// Some global functions that operate on vectors
inline float Vector3Distance(const Vector3& v1,const Vector3& v2)
{
	return (v2-v1).Length();
}

inline float Vector2Distance(const Vector2& v1,const Vector2& v2)
{
	return (v2-v1).Length();
}

inline Vector2 Vector2Normalize(const Vector2& v)
{
	Vector2 out(v);

	out.Normalize();

	return out;
}

inline Vector3 Vector3Normalize(const Vector3& v)
{
	Vector3 out(v);

	out.Normalize();

	return out;
}

inline float Vector3Dot(const Vector3& v1,const Vector3& v2)
{
	return v1.x*v2.x+v1.y*v2.y+v1.z*v2.z;
}

inline float Vector2Dot(const Vector2& v1,const Vector2& v2)
{
	return v1.x*v2.x+v1.y*v2.y;
}

inline Vector3 Vector3LinearInterpolation(const Vector3& v1,const Vector3& v2,float s)
{
	return Vector3(v1 + (v2 - v1) * s);
}

inline Vector2 nVecotr2LinearInterpolation(const Vector2& v1,const Vector2& v2,float s)
{
	return Vector2(v1 + (v2 - v1) * s);
}

inline Vector3 Vector3CubicInterpolation(const Vector3& v1,const Vector3& v2,float s)
{
	float s1 = 2.0f * powf(s,3.0f);
	float s2 = 3.0f * powf(s,2.0f);

	return Vector3(v1 * (s1-s2+1.0f) + v2 * (s2-s1));
}

inline Vector2 Vector2CubicInterpolation(const Vector2& v1,const Vector2& v2,float s)
{
	float s1 = 2.0f * powf(s,3.0f);
	float s2 = 3.0f * powf(s,2.0f);

	return Vector2(v1 * (s1-s2+1.0f) + v2 * (s2-s1));
}

inline Vector3 Vector3HermiteInterpolation(const Vector3& v1,const Vector3& v2,const Vector3& v3,const Vector3& v4, float s)
{
	float t2 = s * s;
	float t3 = t2 * s;

	float kp0 = (2.0f * t2 - t3 - s) / 2.0f;
	float kp1 = (3.0f * t3 - 5.0f * t2 + 2.0f) / 2.0f;
	float kp2 = (4.0f * t2 - 3.0f * t3 + s) / 2.0f;
	float kp3 = (t3 - t2) / 2.0f;

	return v1 * kp0 + v2 * kp1 + v3 * kp2 + v4 * kp3;
}

inline Vector3 Vector3CatmullRomInterpolation(const Vector3& v1,const Vector3& v2,const Vector3& v3,const Vector3& v4, float s)
{
	float s2 = powf(s,2.0f);
	float s3 = powf(s,3.0f);

	return (v1 * (-s3 + 2.0f * s2 - s) + v2 * (3.0f * s3 - 5.0f * s2 + 2.0f) + v3 * (-3.0f * s3 + 4.0f * s2 + s) + v4 * (s3 - s2)) / 2.0f;
}

inline Vector3 Vector3CatmullRomTangentInterpolation(const Vector3& v1,const Vector3& v2,const Vector3& t1,const Vector3& t2, float s)
{
	float s3 = powf(s,3.0f);
	float s2 = powf(s,2.0f);

	return v1 * (2.0f * s3 - 3.0f * s2 + 1.0f) + v2 * (3.0f * s2 - 2.0f * s3) + t1 * (s3 - 2.0f * s2 + s) + t2 * (s3 - s2);
}

inline Vector3 Vector3Cross(const Vector3& v1,const Vector3& v2)
{
	return Vector3(v1.y * v2.z - v1.z * v2.y,v1.z * v2.x - v1.x * v2.z,v1.x * v2.y - v1.y * v2.x);
}

inline Vector3 Vector3Normal(const Vector3& v1, const Vector3& v2, const Vector3& v3)
{
	Vector3 vNormal = Vector3Cross(v3-v1,v2-v1);

	vNormal.Normalize();

	return vNormal;
}

inline Vector2 Vector2Perpendicular(const Vector2& vec)
{
	return Vector2(-vec.y,vec.x);
}