#pragma once

// Float to unsigned long conversion
__forceinline unsigned long F2DW(float f) { return *((unsigned long*)&f); }

// Catmull Rom interpolation function
__forceinline float catmullromf(float p1,float p2,float p3,float p4,float s)
{
	float s2 = powf(s,2.0f);
	float s3 = powf(s,3.0f);

	return (p1 * (-s3 + 2.0f * s2 - s) + p2 * (3.0f * s3 - 5.0f * s2 + 2.0f) + p3 * (-3.0f * s3 + 4.0f * s2 + s) + p4 * (s3 - s2)) / 2.0f;
}

__forceinline double catmullrom(double p1,double p2,double p3,double p4,double s)
{
	double s2 = pow(s,2.0);
	double s3 = pow(s,3.0);

	return (p1 * (-s3 + 2.0 * s2 - s) + p2 * (3.0 * s3 - 5.0 * s2 + 2.0) + p3 * (-3.0 * s3 + 4.0 * s2 + s) + p4 * (s3 - s2)) / 2.0;
}

// Hermite interpolation function
__forceinline float hermitef(float p1,float p2,float p3,float p4,float s)
{
	float t2 = s * s;
	float t3 = t2 * s;

	float kp0 = (2.0f * t2 - t3 - s) / 2.0f;
	float kp1 = (3.0f * t3 - 5.0f * t2 + 2.0f) / 2.0f;
	float kp2 = (4.0f * t2 - 3.0f * t3 + s) / 2.0f;
	float kp3 = (t3 - t2) / 2.0f;

	return p1 * kp0 + p2 * kp1 + p3 * kp2 + p4 * kp3;
}

__forceinline double hermite(double p1,double p2,double p3,double p4,double s)
{
	double t2 = s * s;
	double t3 = t2 * s;

	double kp0 = (2.0 * t2 - t3 - s) / 2.0;
	double kp1 = (3.0 * t3 - 5.0 * t2 + 2.0) / 2.0;
	double kp2 = (4.0 * t2 - 3.0 * t3 + s) / 2.0;
	double kp3 = (t3 - t2) / 2.0;

	return p1 * kp0 + p2 * kp1 + p3 * kp2 + p4 * kp3;
}

// Linear interpolation function
__forceinline float lerpf(float p1,float p2,float s)
{ 
	return p1 + s * (p2 - p1);
}

__forceinline double lerp(double p1,double p2,double s)
{
	return p1 + s * (p2 - p1);
}

// Cubic interpolation function
__forceinline float cubicf(float p1,float p2,float s)
{
	float s1 = 2.0f * powf(s,3.0f);
	float s2 = 3.0f * powf(s,2.0f);

	return p1 * (s1 - s2 + 1.0f) + p2 * (s2 - s1);
}

__forceinline double cubic(double p1,double p2,double s)
{
	double s1 = 2.0 * pow(s,3.0);
	double s2 = 3.0 * pow(s,2.0);

	return p1 * (s1 - s2 + 1.0) + p2 * (s2 - s1);
}

// Cos interpolation function
__forceinline double cosp(double p1,double p2,double s)
{
	double f1 = (1.0 - cos(M_PI * s)) * 0.5;

	return  p1 * (1.0 - f1) + p2 * f1;
}

__forceinline float cospf(float p1,float p2,float s)
{
	float f1 = (1.0f - cosf(M_PI * s)) * 0.5f;

	return  p1 * (1.0f - f1) + p2 * f1;
}

__forceinline float randf(float min, float max)
{
    float randnum = (float)rand() / RAND_MAX;
    return min + (max - min) * randnum;
}

__forceinline double rand(double min, double max)
{
    double randnum = (double)rand() / RAND_MAX;
    return min + (max - min) * randnum;
}