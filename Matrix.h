// Copyright 2003/2005 Marko Mihovilic

#pragma once

#include "Vector.h"

#ifdef ALIGN_DATA
__declspec(align(16))
#endif
class Matrix4
{
public:
	Matrix4(void) { /*this->SetEmpty();*/ }
	Matrix4(bool identity) { if(identity) SetIdentity(); }
	Matrix4(const float** m) { Set(m); }
	Matrix4(float _11, float _12, float _13, float _14, float _21, float _22, float _23, float _24, float _31, float _32, float _33, float _34, float _41, float _42, float _43, float _44) { Set(_11,_12,_13,_14,_21,_22,_23,_24,_31,_32,_33,_34,_41,_42,_43,_44); }
	Matrix4(const Matrix4& other) { Set(other); }

	void Set(const float** m);
	void Set(float _11, float _12, float _13, float _14, float _21, float _22, float _23, float _24, float _31, float _32, float _33, float _34, float _41, float _42, float _43, float _44);
	void Set(const Matrix4& other);
	void SetEmpty(void);
	bool IsEmpty(void) const;

	void SetIdentity(void);
	bool IsIdentity(void) const;

	//void Inverse(void) 
	//{ 
	//	D3DXMatrixInverse((LPD3DXMATRIX)this,NULL,(LPD3DXMATRIX)this);	// TODO Write own function
	//}

	void Translate(const Vector4& trans);
	void Translate(const Vector3& trans);
	void Translate(const Vector2& trans);

	void Rotate(const Vector3& axis,float theta);	// Axis must be normalized
	void Rotate(float azimuth, float elevation);

	void RotateX(float theta);
	void RotateY(float theta);
	void RotateZ(float theta);

	void Scale(const Vector4& scale);
	void Scale(const Vector3& scale);
	void Scale(const Vector2& scale);

	//void Perspective(float fovy,float aspect,float zn,float zf)
	//{
	//	D3DXMatrixPerspectiveFovLH((LPD3DXMATRIX)this,fovy,aspect,zn,zf);	// TODO Write own function
	//}

	//void View(const Vector3& eye,const Vector3& lookat,const Vector3& up,const Vector3& right)
	//{
	//	D3DXMatrixLookAtLH((LPD3DXMATRIX)this,(LPD3DXVECTOR3)&eye,(LPD3DXVECTOR3)&lookat,(LPD3DXVECTOR3)&up);	// TODO Write own function
	//}

	void Transform(Vector4* vec) const;
	void Transform(Vector3* vec) const;
	void Transform(Vector2* vec) const;

	Matrix4 Transpose(void) const { return Matrix4(_11,_21,_31,_41,_12,_22,_32,_42,_13,_23,_33,_43,_14,_24,_34,_44); }

	bool operator == (const Matrix4& other) const;
	bool operator != (const Matrix4& other) const;

	Matrix4 operator + (const Matrix4& other) const;
	Matrix4 operator - (const Matrix4& other) const;
	Matrix4 operator * (const Matrix4& other) const;

	Matrix4 operator * (float s) const;
	Matrix4 operator / (float s) const;

	Matrix4& operator += (const Matrix4& other);
	Matrix4& operator -= (const Matrix4& other);
	Matrix4& operator *= (const Matrix4& other);

	Matrix4& operator *= (float s);
	Matrix4& operator /= (float s);

	friend Matrix4 operator * (float,const Matrix4&);

	// Float array conversion
	operator float**(void) { return (float**)m; }
	operator const float**(void) const { return (const float**)m; }

public:
	union 
	{
		struct
		{
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;
        };

		float m[4][4];
	};
};