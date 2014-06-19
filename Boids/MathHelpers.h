#pragma once
#define _USE_MATH_DEFINES
//#include <glm\glm.hpp>
//#include <glm\gtc\matrix_transform.hpp>
//#include <glm\gtc\type_ptr.hpp>
#include <stdio.h>
#include <math.h>


#define ToRadian(x) (float)(((x) * M_PI / 180.0f))
#define ToDegree(x) (float)(((x) * 180.0f / M_PI))

float RandomFloat();

struct Vec2 {
	float x, y;

	Vec2()
	{}
	Vec2(float _x, float _y)
		: x(_x), y(_y)
	{}
	float operator[](int i) const;
	float& operator[](int i);
	Vec2 operator+=(const Vec2 &v);
	Vec2 operator-=(const Vec2 &v);
	Vec2 operator*=(float s);
	Vec2 operator/=(float s);

	float Length();
	float Length2();
	float Normalize();
	float SafeNormalize();
	void  NanAssert();
	float UnitFromDelta( const Vec2& posA, const Vec2& posB );
	float Magnitude();
	float Distance(Vec2 b);
	void Clamp(float f);

	static Vec2 Zero;
};

// vector ops
Vec2 operator-(const Vec2 &v);
Vec2 operator+(const Vec2 &v0, const Vec2 &v1);
Vec2 operator-(const Vec2 &v0, const Vec2 &v1);
Vec2 operator*(const Vec2 &v0, const Vec2 &v1);
Vec2 operator*(const Vec2 &v, float s);
Vec2 operator*(float s, const Vec2 &v);
Vec2 operator/(const Vec2 &v, float s);
float dot(const Vec2 &v0, const Vec2 &v1);

struct Vec3 {
	float x, y, z;

	Vec3()
	{}
	Vec3(float _x, float _y, float _z)
		: x(_x), y(_y), z(_z)
	{}
	float operator[](int i) const;
	float& operator[](int i);
	
	Vec3 operator/=(float s);

	Vec3& operator+=(const Vec3& r)
    {
        x += r.x;
        y += r.y;
        z += r.z;

        return *this;
    }

    Vec3& operator-=(const Vec3& r)
    {
        x -= r.x;
        y -= r.y;
        z -= r.z;

        return *this;
    }

    Vec3& operator*=(float f)
    {
        x *= f;
        y *= f;
        z *= f;

        return *this;
    }

	float Length();
	float Length2();
	//float Normalize();
	float SafeNormalize();
	void  NanAssert();
	float UnitFromDelta( const Vec3& posA, const Vec3& posB );
	float Clamp(float f);

	Vec3 Cross(const Vec3& v) const;
    Vec3& Normalize();
	void Rotate(float angle, const Vec3& axis);

	static Vec3 Zero;
};


inline Vec3 operator+(const Vec3& l, const Vec3& r)
{
	Vec3 temp(l.x + r.x, l.y + r.y, l.z + r.z);

	return temp;
}

inline Vec3 operator-(const Vec3& l, const Vec3& r)
{
    Vec3 temp(l.x - r.x, l.y - r.y, l.z - r.z);

    return temp;
}

inline Vec3 operator*(const Vec3& l, float f)
{
    Vec3 temp(l.x * f, l.y * f, l.z * f);

    return temp;
}

struct Plane2D
{
	Vec2 normal;
	float distFromOrigin;

	Plane2D( Vec2& norm, float dist )
		:
	normal(norm),
		distFromOrigin(dist)
	{}

	float distanceOfPoint(Vec2& point);
};

struct PersProjInfo
{
	float FOV, width, height, zNear, zFar;
};


class Matrix4
{
public:
    float m[4][4];

    Matrix4()
    {        
    }

	Matrix4 Transpose() const
	{
		Matrix4 n;

		for(unsigned int i = 0; i < 4; i++)
		{
			for(unsigned int j = 0; j < 4; j++)
			{
				n.m[i][j] = m[j][i];
			}
		}
		return n;
	}


    inline void InitIdentity()
    {
        m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
        m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
        m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
        m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
    }

    inline Matrix4 operator*(const Matrix4& Right) const
    {
        Matrix4 Ret;

        for (unsigned int i = 0 ; i < 4 ; i++) {
            for (unsigned int j = 0 ; j < 4 ; j++) {
                Ret.m[i][j] = m[i][0] * Right.m[0][j] +
                              m[i][1] * Right.m[1][j] +
                              m[i][2] * Right.m[2][j] +
                              m[i][3] * Right.m[3][j];
            }
        }

        return Ret;
    }

    void InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ);
    void InitRotateTransform(float RotateX, float RotateY, float RotateZ);
    void InitTranslationTransform(float x, float y, float z);
    void InitCameraTransform(const Vec3& Target, const Vec3& Up);
    void InitPersProjTransform(float FOV, float Width, float Height, float zNear, float zFar);
	void InitPersProjTransform(const PersProjInfo& p);
};

struct Quaternion
{
	float x, y, z, w;

	Quaternion(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {};
	Quaternion Conjugate();

	void Normalize();
	
};

Quaternion operator*(const Quaternion& l, const Quaternion& r);
Quaternion operator*(const Quaternion& q, const Vec3& v);

