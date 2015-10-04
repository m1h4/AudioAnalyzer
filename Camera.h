// Copyright 2004/2006 Marko Mihovilic

#pragma once

// Base camera class
class Camera
{
public:
	Camera();
	virtual ~Camera();

	// Override if necessary
	virtual void SetFov(float fov) { mFov = fov; }
	virtual void MoveBy(const Vector3& amount);
	virtual void MoveTo(const Vector3& dest);
	virtual void MoveLerp(const Vector3& dest,float s);
	virtual void RotateBy(const Vector3& amount);
	virtual void RotateTo(const Vector3& dest);

	// Override if necessary
	virtual float GetFov() const { return mFov; }
	virtual Vector3 GetEye() const { return mEye; }
	virtual Vector3 GetLookAt() const { return mLookAt; }
	virtual Vector3 GetUp() const { return mUp; }
	virtual Vector3 GetRight() const { return mRight; }
	virtual Vector3 GetStaticUp() const { return mStaticUp; }

	virtual void Reset();
	
private:
	float	mFov;
	Vector3	mEye;
	Vector3	mLookAt;
	Vector3	mUp;
	Vector3	mStaticUp;
	Vector3	mRight;
};

inline void Camera::Reset()
{
	mLookAt = Vector3(0.0f,0.0f,-1.0f);
	mUp = Vector3(0.0f,1.0f,0.0f);
	mStaticUp = Vector3(0.0f,1.0f,0.0f);
	mRight = Vector3(1.0f,0.0f,0.0f);

	mLookAt.Normalize();
	mUp.Normalize();
	mStaticUp.Normalize();
	mRight.Normalize();
}