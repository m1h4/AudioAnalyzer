// Copyright 2004/2006 Marko Mihovilic

#include "Globals.h"
#include "Camera.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Camera::Camera() :
mFov((float)M_PI/4.0f)
{
	Reset();
}

Camera::~Camera()
{
}

void Camera::MoveBy(const Vector3& amount)
{
	mEye += mLookAt * amount.z;
	mEye += mRight * amount.x;
	mEye += mUp * amount.y;
}

void Camera::MoveTo(const Vector3& dest)
{
	mEye = dest;
}

void Camera::MoveLerp(const Vector3& dest,float s)
{
	mEye.Lerp(dest,s);
}

void Camera::RotateBy(const Vector3& yawpitchroll)
{
	Matrix4 matRot;

	// Yaw
	if(yawpitchroll.x != 0.0f)
	{
		matRot.Rotate(mStaticUp, yawpitchroll.x);	// We coud use our Up vector here to get a more reallife camera
		matRot.Transform(&mLookAt);
		matRot.Transform(&mUp);
		matRot.Transform(&mRight);
	}

	// Pitch
	if(yawpitchroll.y != 0.0f)
	{
		matRot.Rotate(mRight, yawpitchroll.y);
		matRot.Transform(&mLookAt);
		matRot.Transform(&mUp);
	}

	// Roll
	if(yawpitchroll.z != 0.0f)
	{
		matRot.Rotate(mLookAt, yawpitchroll.z);
		matRot.Transform(&mUp);
		matRot.Transform(&mRight);	
		matRot.Transform(&mStaticUp); // Also roll the static-up so yaw can work
	}
}

void Camera::RotateTo(const Vector3& yawpitchroll)
{
	Reset();
	RotateBy(yawpitchroll);
}