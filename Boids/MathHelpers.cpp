
#include <cmath>
#include <float.h>
#include <assert.h>
#include <stdlib.h>
#include "MathHelpers.h"

float RandomFloat()
{
	float max = RAND_MAX;
	return ((float) rand() / max);

}

// Vec2
Vec2 Vec2::Zero(0, 0);

float Vec2::operator[](int i) const {
	return (&x)[i];
}

float& Vec2::operator[](int i) {
	return (&x)[i];
}

Vec2 Vec2::operator+=(const Vec2 &v) {
	// *this = *this + v;
	x += v.x;
	y += v.y;
	return *this;
}

Vec2 Vec2::operator-=(const Vec2 &v) {
	// *this = *this + v;
	x -= v.x;
	y -= v.y;
	return *this;
}

Vec2 Vec2::operator*=(float s) {
	// *this = *this * s;
	x *= s;
	y *= s;
	return *this;
}

Vec2 Vec2::operator/=(float s) {
	// *this = *this * (1.0f / s);
	float r = 1.0f / s;
	x *= s;
	y *= s;
	return *this;
}

float Vec2::Length()
{
	return sqrtf(x*x + y*y);
}

float Vec2::Length2()
{
	return x*x + y*y;
}

float Vec2::Normalize()
{
	float len = sqrtf(x*x + y*y);
	x /= len;
	y /= len;
	return len;
}

float Vec2::SafeNormalize()
{
	float len = sqrtf(x*x + y*y);
	const float EPSILON = 0.00001f;
	if( len > EPSILON )
	{
		x /= len;
		y /= len;
	}
	else
	{
		// Little or no length in line. Pretend there is a bit.
		y = 1;
		x = 0;
		const float fudge = 0.001f;
		len = fudge;
	}
	return len;
}

float Vec2::UnitFromDelta( const Vec2& posA, const Vec2& posB )
{
	x = posB.x - posA.x;
	y = posB.y - posA.y;

	float len = sqrtf(x*x + y*y);
	const float EPSILON = 0.00001f;
	if( len > EPSILON )
	{
		x /= len;
		y /= len;
		return len;
	}
	else
	{
		// Little or no length in line. Pretend there is a bit.
		y = 1;
		x = 0;
		return 0.001f;	// Some small amount
	}
}

float Vec2::Magnitude()
{
	return sqrt(x*x + y*y);
}

float Vec2::Distance(Vec2 b)
{
	//		sqrt (x1 - x2 ^ 2) + (y1 - y2 ^ 2)
	return sqrt((b.x - x) * (b.x - x) + (b.y - y) * (b.y - y));
}

void Vec2::Clamp(float f)
{
	if(x >= f)
		x = f;
	else if(x <= -f)
		x = -f;
	if(y >= f)
		y = f;
	else if(y <= -f)
		y = -f;


	
}

void Vec2::NanAssert()
{
	assert(!_isnan(x));
	assert(!_isnan(y));
}



Vec2 operator+(const Vec2 &v0, const Vec2 &v1)
{
	return Vec2(v0.x + v1.x, v0.y + v1.y);
}

Vec2 operator-(const Vec2 &v0, const Vec2 &v1)
{
	return Vec2(v0.x - v1.x, v0.y - v1.y);
}

Vec2 operator*(const Vec2 &v0, const Vec2 &v1)
{
	return Vec2(v0.x * v1.x, v0.y * v1.y);
}

Vec2 operator*(const Vec2 &v, float s)
{
	return Vec2(v.x * s, v.y * s);
}

Vec2 operator*(float s, const Vec2 &v)
{
	return v * s;
}

Vec2 operator/(const Vec2 &v, float s)
{
	return v * (1.0f / s);
}

float dot(const Vec2 &v0, const Vec2 &v1)
{
	return v0.x * v1.x + v0.y * v1.y;
}


float Plane2D::distanceOfPoint(Vec2& point)
{
	float dist = dot( normal, point ) - distFromOrigin;
	return dist;
}



//Vector 3

Vec3 Vec3::Cross(const Vec3& v) const
{
    const float _x = y * v.z - z * v.y;
    const float _y = z * v.x - x * v.z;
    const float _z = x * v.y - y * v.x;

    return Vec3(_x, _y, _z);
}

Vec3& Vec3::Normalize()
{
    const float len = sqrtf(x * x + y * y + z * z);

    x /= len;
    y /= len;
    z /= len;

    return *this;
}

void Vec3::Rotate(float angle, const Vec3& v)
{
	const float halfSin = sinf(ToRadian(angle/2));
	const float halfCos = cosf(ToRadian(angle/2));

	const float rX = v.x * halfSin;
	const float rY = v.y * halfSin;
	const float rZ = v.z * halfSin;
	const float rW = halfCos;

	Quaternion R(rX, rY, rZ, rW);
	Quaternion conR = R.Conjugate();
	Quaternion W = R * (*this) * conR;

	x = W.x;
	y = W.y;
	z = W.z;
}

//Matrix4

void Matrix4::InitPersProjTransform(const PersProjInfo& p)
{
	const float ratio	= p.width / p.height;
	const float zRange	= p.zNear - p.zFar;
	const float tan		= tanf(ToRadian(p.FOV / 2.0f));

	m[0][0] = 1.0f/(tan * ratio);	  m[0][1] = 0.0f;            m[0][2] = 0.0f;						m[0][3] = 0.0;
    m[1][0] = 0.0f;                   m[1][1] = 1.0f/tan;		 m[1][2] = 0.0f;						m[1][3] = 0.0;
    m[2][0] = 0.0f;                   m[2][1] = 0.0f;            m[2][2] = (-p.zNear - p.zFar)/zRange ; m[2][3] = 2.0f*p.zFar*p.zNear/zRange;
    m[3][0] = 0.0f;                   m[3][1] = 0.0f;            m[3][2] = 1.0f;						m[3][3] = 0.0;    
}

void Matrix4::InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ)
{
    m[0][0] = ScaleX; m[0][1] = 0.0f;   m[0][2] = 0.0f;   m[0][3] = 0.0f;
    m[1][0] = 0.0f;   m[1][1] = ScaleY; m[1][2] = 0.0f;   m[1][3] = 0.0f;
    m[2][0] = 0.0f;   m[2][1] = 0.0f;   m[2][2] = ScaleZ; m[2][3] = 0.0f;
    m[3][0] = 0.0f;   m[3][1] = 0.0f;   m[3][2] = 0.0f;   m[3][3] = 1.0f;
}

void Matrix4::InitRotateTransform(float RotateX, float RotateY, float RotateZ)
{
    Matrix4 rx, ry, rz;

    const float x = ToRadian(RotateX);
    const float y = ToRadian(RotateY);
    const float z = ToRadian(RotateZ);

    rx.m[0][0] = 1.0f; rx.m[0][1] = 0.0f   ; rx.m[0][2] = 0.0f    ; rx.m[0][3] = 0.0f;
    rx.m[1][0] = 0.0f; rx.m[1][1] = cosf(x); rx.m[1][2] = -sinf(x); rx.m[1][3] = 0.0f;
    rx.m[2][0] = 0.0f; rx.m[2][1] = sinf(x); rx.m[2][2] = cosf(x) ; rx.m[2][3] = 0.0f;
    rx.m[3][0] = 0.0f; rx.m[3][1] = 0.0f   ; rx.m[3][2] = 0.0f    ; rx.m[3][3] = 1.0f;

    ry.m[0][0] = cosf(y); ry.m[0][1] = 0.0f; ry.m[0][2] = -sinf(y); ry.m[0][3] = 0.0f;
    ry.m[1][0] = 0.0f   ; ry.m[1][1] = 1.0f; ry.m[1][2] = 0.0f    ; ry.m[1][3] = 0.0f;
    ry.m[2][0] = sinf(y); ry.m[2][1] = 0.0f; ry.m[2][2] = cosf(y) ; ry.m[2][3] = 0.0f;
    ry.m[3][0] = 0.0f   ; ry.m[3][1] = 0.0f; ry.m[3][2] = 0.0f    ; ry.m[3][3] = 1.0f;

    rz.m[0][0] = cosf(z); rz.m[0][1] = -sinf(z); rz.m[0][2] = 0.0f; rz.m[0][3] = 0.0f;
    rz.m[1][0] = sinf(z); rz.m[1][1] = cosf(z) ; rz.m[1][2] = 0.0f; rz.m[1][3] = 0.0f;
    rz.m[2][0] = 0.0f   ; rz.m[2][1] = 0.0f    ; rz.m[2][2] = 1.0f; rz.m[2][3] = 0.0f;
    rz.m[3][0] = 0.0f   ; rz.m[3][1] = 0.0f    ; rz.m[3][2] = 0.0f; rz.m[3][3] = 1.0f;

    *this = rz * ry * rx;
}

void Matrix4::InitTranslationTransform(float x, float y, float z)
{
    m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = x;
    m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = y;
    m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = z;
    m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
}


void Matrix4::InitCameraTransform(const Vec3& Target, const Vec3& Up)
{
    Vec3 N = Target;
    N.Normalize();
    Vec3 U = Up;
    U.Normalize();
    U = U.Cross(N);
    Vec3 V = N.Cross(U);

    m[0][0] = U.x;   m[0][1] = U.y;   m[0][2] = U.z;   m[0][3] = 0.0f;
    m[1][0] = V.x;   m[1][1] = V.y;   m[1][2] = V.z;   m[1][3] = 0.0f;
    m[2][0] = N.x;   m[2][1] = N.y;   m[2][2] = N.z;   m[2][3] = 0.0f;
    m[3][0] = 0.0f;  m[3][1] = 0.0f;  m[3][2] = 0.0f;  m[3][3] = 1.0f;
}

void Matrix4::InitPersProjTransform(float FOV, float Width, float Height, float zNear, float zFar)
{
    const float ar         = Width / Height;
    const float zRange     = zNear - zFar;
    const float tanHalfFOV = tanf(ToRadian(FOV / 2.0f));

    m[0][0] = 1.0f/(tanHalfFOV * ar); m[0][1] = 0.0f;            m[0][2] = 0.0f;					m[0][3] = 0.0;
    m[1][0] = 0.0f;                   m[1][1] = 1.0f/tanHalfFOV; m[1][2] = 0.0f;					m[1][3] = 0.0;
    m[2][0] = 0.0f;                   m[2][1] = 0.0f;            m[2][2] = (-zNear -zFar)/zRange ;	m[2][3] = 2.0f * zFar*zNear/zRange;
    m[3][0] = 0.0f;                   m[3][1] = 0.0f;            m[3][2] = 1.0f;					m[3][3] = 0.0;
}

//Quaternion

void Quaternion::Normalize()
{
	float len = sqrtf(x * x + y * y + z * z + w * w);

	x /= len;
	y /= len;
	z /= len;
	w /= len;
}

Quaternion Quaternion::Conjugate()
{
	Quaternion temp(-x, -y, -z, -w);
	return temp;
}


Quaternion operator*(const Quaternion& l, const Quaternion& r)
{
    const float w = (l.w * r.w) - (l.x * r.x) - (l.y * r.y) - (l.z * r.z);
    const float x = (l.x * r.w) + (l.w * r.x) + (l.y * r.z) - (l.z * r.y);
    const float y = (l.y * r.w) + (l.w * r.y) + (l.z * r.x) - (l.x * r.z);
    const float z = (l.z * r.w) + (l.w * r.z) + (l.x * r.y) - (l.y * r.x);

    Quaternion ret(x, y, z, w);

    return ret;
}

Quaternion operator*(const Quaternion& q, const Vec3& v)
{
    const float w = - (q.x * v.x) - (q.y * v.y) - (q.z * v.z);
    const float x =   (q.w * v.x) + (q.y * v.z) - (q.z * v.y);
    const float y =   (q.w * v.y) + (q.z * v.x) - (q.x * v.z);
    const float z =   (q.w * v.z) + (q.x * v.y) - (q.y * v.x);

    Quaternion ret(x, y, z, w);

    return ret;
}


