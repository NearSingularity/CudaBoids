#include <GL\freeglut.h>
#include "MathHelpers.h"
#include "engine_common.h"
#include "glut_backend.h"
#include "util.h"
#include "pipeline.h"
#include "Camera.h"



#define WIDTH 1024
#define HEIGHT 768

#define NUM_ROWS 50
#define NUM_COLS 50

const int NUM_BOIDS = NUM_ROWS * NUM_COLS;

class Manager : public ICallbacks
{
public:
	Manager();
	~Manager();

	bool Init();
	void Run()
	{
		GLUTBackendRun(this);
	}

	virtual void RenderCB();
	virtual void IdleCB();
	virtual void CamCB(int key, int x, int y);
	virtual void MouseTrackCB(int x, int y);
	virtual void MouseCB(int Button, int State, int x, int y);
	virtual void KeyboardCB(unsigned char key, int x, int y);

	void CalcFPS();
	void RenderFPS();
	void CalcPos();

private:
	//Mesh*	m_Mesh;
	Camera* m_Camera;
	PersProjInfo m_Info;
	Vec3 m_Positions[NUM_BOIDS];
	float m_Velocities[NUM_BOIDS];
	float m_Scale, m_FPS;
	int m_Time, m_FrameCount;

};