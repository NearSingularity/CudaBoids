#include "Camera.h"
#include "GL\freeglut.h"

const static int BUFFER = 10;
const static float STEP = .1f;

Camera::Camera(int width, int height)
{
	m_WindowWidth	= width;
	m_WindowHeight	= height;
	m_Pos			= Vec3(0.0f, 0.0f, 0.0f);
	m_Target		= Vec3(0.0f, 0.0f, 1.0f);
	m_Up			= Vec3(0.0f, 1.0f, 0.0f);
	m_Speed			= .1f;
}

Camera::Camera(int width, int height, const Vec3& pos, const Vec3& targ, const Vec3& up)
{
	m_WindowWidth	= width;
	m_WindowHeight	= height;
	m_Pos			= pos;
	m_Target		= targ;
	m_Up			= up;
	m_Speed			= .1f;

	m_Target.Normalize();
	m_Up.Normalize();

	Init();
}

void Camera::Init()
{
	Vec3 target(m_Target.x, 0.0, m_Target.z);
	target.Normalize();

	if(target.z >= 0.0f)
	{
		target.x >= 0.0f ? m_AngleHorizontal = 360.0f - ToDegree(asin(target.z)) : m_AngleHorizontal = 180.0f + ToDegree(asin(target.z));
	}
	else
	{
		target.x >= 0.0f ? m_AngleHorizontal = ToDegree(asin(-target.z)) : m_AngleHorizontal = 90.0f + ToDegree(asin(-target.z));
	}

	m_AngleVertical = -ToDegree(asin(m_Target.y));

	m_OnUpperEdge = false;
	m_OnLowerEdge = false;
	m_OnLeftEdge  = false;
	m_OnRightEdge = false;

	m_MousePos.x = m_WindowWidth / 2;
	m_MousePos.y = m_WindowHeight / 2;

	glutWarpPointer(m_MousePos.x, m_MousePos.y);
}

void Camera::OnMouse(int x, int y)
{
	const int deltaX = x - m_MousePos.x;
	const int deltaY = y - m_MousePos.y;

	m_MousePos.x = (float)x;
	m_MousePos.y = (float)y;

	m_AngleHorizontal += (float)deltaX / 20.0f;
	m_AngleVertical	  += (float)deltaY / 20.0f;

	if(deltaX == 0)
	{
		if(x <= BUFFER)
			m_OnLeftEdge = true;
		else if(x >= m_WindowWidth - BUFFER)
			m_OnRightEdge = true;
	}
	else
	{
		m_OnLeftEdge  = false;
		m_OnRightEdge = false;
	}

	if(deltaY == 0)
	{
		if(y <= BUFFER)
			m_OnUpperEdge = true;
		else if(y >= m_WindowHeight - BUFFER)
			m_OnLowerEdge = true;
	}
	else
	{
		m_OnUpperEdge = false;
		m_OnLowerEdge = false;
	}

	Update();
}

void Camera::OnRender()
{
	bool flag = false;

	if(m_OnLeftEdge)
	{
		m_AngleHorizontal -= .1f;
		flag = true;
	}
	else if(m_OnRightEdge)
	{
		m_AngleHorizontal += .1f;
		flag = true;
	}

	if(m_OnUpperEdge)
	{
		if(m_AngleVertical > -90.0f)
		{
			m_AngleVertical -= .1f;
			flag = true;
		}
	}
	else if(m_OnLowerEdge)
	{
		if(m_AngleVertical < 90.0f)
		{
			m_AngleVertical += .1f;
			flag = true;
		}
	}

	if(flag)
		Update();
}

void Camera::Update()
{
	const Vec3 yAxis(0.0f, 1.0f, 0.0f);

	Vec3 view(1.0f, 0.0f, 0.0f);
	view.Rotate(m_AngleHorizontal, yAxis);
	view.Normalize();

	Vec3 xAxis = yAxis.Cross(view);
	xAxis.Normalize();

	view.Rotate(m_AngleVertical, xAxis);
	view.Normalize();

	m_Target = view;
	m_Up = m_Target.Cross(xAxis);

	m_Target.Normalize();
	m_Up.Normalize();
}

bool Camera::InputDetect(int input)
{
	switch(input)
	{
	case GLUT_KEY_UP:
		{
		m_Pos += m_Target * m_Speed;
		return true;
		}
		break;
	case GLUT_KEY_DOWN:
		{
		m_Pos -= m_Target * m_Speed;
		return true;
		}
		break;
	case GLUT_KEY_LEFT:
		{
		Vec3 l = m_Target.Cross(m_Up);
		l.Normalize();
		l *= m_Speed;

		m_Pos += l;
		return true;
		}
		break;
	case GLUT_KEY_RIGHT:
		{
		Vec3 r = m_Up.Cross(m_Target);
		r.Normalize();
		r *= m_Speed;

		m_Pos += r;
		return true;
		}
		break;
	}
	return false;
}