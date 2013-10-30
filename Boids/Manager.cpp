#include "Manager.h"

Manager::Manager()
{
	m_Camera = NULL;
	//m_Mesh = NULL;

	m_Info.FOV = 60.0f;
	m_Info.height = HEIGHT;
	m_Info.width = WIDTH;
	m_Info.zFar = 100.0f;
	m_Info.zNear = 1.0f;

	m_FrameCount = 0;
	m_FPS = 0.0f;
	m_Scale = 0.0f;
}

Manager::~Manager()
{
	SAFE_DELETE(m_Camera);
	//SAFE_DELETE(m_Mesh);
	//delete cam, mesh
}

bool Manager::Init()
{
	Vec3 pos(5.0f, 5.0f, 5.0f);
	Vec3 targ(0.0f, 0.0f, 0.0f);
	Vec3 up(0.0f, 1.0f, 0.0f);

	m_Camera = new Camera(WIDTH, HEIGHT);

	//m_Mesh = new Mesh();

	m_Time = glutGet(GLUT_ELAPSED_TIME);

	CalcPos();

	return true;
}

void Manager::RenderFPS()
{
	char text[32];

	SNPRINTF(text, sizeof(text), "FPS: %.2f", m_FPS);
}

void Manager::CalcFPS()
{
	m_FrameCount++;

	int time = glutGet(GLUT_ELAPSED_TIME);

	if(time - m_Time > 1000)
	{
		m_FPS = (float)m_FrameCount * 1000.0f / (time-m_Time);
		m_Time = time;
		m_FrameCount = 0;
	}
}

void Manager::CalcPos()
{
	for(unsigned int y = 0; y < NUM_ROWS; y++)
	{
		for(unsigned int x = 0; x < NUM_COLS; x++)
		{
			unsigned int index = y * NUM_COLS + x;

			m_Positions[index].x = (float)x;
			m_Positions[index].y = RandomFloat() * 5.0f;
			m_Positions[index].z = (float)y;
			m_Velocities[index] = RandomFloat();

			if(y & 1)
				m_Velocities[index] *= (-1.0f);
		}
	}
}

void Manager::IdleCB()
{
	RenderCB();
}

void Manager::CamCB(int key, int x, int y)
{
	m_Camera->InputDetect(key);
}

void Manager::KeyboardCB(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 'q':
		glutLeaveMainLoop();
		break;
	}
}

void Manager::MouseTrackCB(int x, int y)
{
	m_Camera->OnMouse(x, y);
}

void Manager::MouseCB(int button, int state, int x, int y)
{

}

void Manager::RenderCB()
{
	CalcFPS();

	m_Scale += .005f;
	m_Camera->OnRender();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Pipeline p;
	p.SetCamera(m_Camera->GetPos(), m_Camera->GetTarget(), m_Camera->GetUp());
	p.SetPerspectiveProj(m_Info);
	p.Rotate(0.0f, 90.0f, 0.0f);
	p.Scale(0.005f, 0.005f, 0.005f);

	Matrix4 vpMatrics[NUM_BOIDS];
	Matrix4 worldMatrices[NUM_BOIDS];

	for(unsigned int i = 0; i < NUM_BOIDS; i++)
	{
		Vec3 pos(m_Positions[i]);
		pos.y += sinf(m_Scale * m_Velocities[i]);
		p.WorldPos(pos.x, pos.y, pos.z);
		vpMatrics[i] = p.GetWVPTrans().Transpose();
		worldMatrices[i] = p.GetWorldTrans().Transpose();
	}

	//m_Mesh->Render(NUM_BOIDS, vpMatrices, worldMatrices);

	RenderFPS();

	glutSwapBuffers();
}