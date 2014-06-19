#pragma once
#include "MathHelpers.h"

class Camera
{
public:
	Camera(int, int);
	Camera(int, int, const Vec3&, const Vec3&, const Vec3&);
	bool InputDetect(int key);
	void Init();
	void Update();
	void OnMouse(int, int);
	void OnRender();

	const Vec3& GetPos() const
	{
		return m_Pos;
	}
	const Vec3& GetTarget() const
	{
		return m_Target;
	}
	const Vec3& GetUp() const
	{
		return m_Up;
	}

private:
	Vec3 m_Pos, m_Target, m_Up;
	Vec2 m_MousePos;

	float m_Speed, m_AngleHorizontal, m_AngleVertical, zNear, zFar;
	int m_WindowHeight, m_WindowWidth;
	bool m_OnUpperEdge, m_OnLowerEdge, m_OnLeftEdge, m_OnRightEdge;
};
