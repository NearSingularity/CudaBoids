#include <GL\glew.h>
#include <GL\freeglut.h>
#include <list>
#include "glut_backend.h"
#include "util.h"
#include "pipeline.h"
#include "Camera.h"
#include "Boid.h"
#include "BoidSystem.h" //BoidSystem should include flocking

#define NUM_ROWS	50
#define NUM_COLS	50

#define POSITIONS	0
#define TEXTURES	1
#define NORMALS		2

//const int NUM_BOIDS = NUM_ROWS * NUM_COLS;
const int NUM_BOIDS = 10000;

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

	static void CreateVertexBuffer();
	static void CreateIndexBuffer();
	static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
	static void CompileShaders(GLuint&);
	static void InitCuda();
	static void InitVBO();

	float CalcFrustrumScale(float f);

	void CalcFPS();
	void RenderFPS();
	void CalcPos();

private:
	//Mesh*	m_Mesh;
	Camera* m_Camera;
	PersProjInfo m_Info;
	Pipeline p;

	typedef std::list<GLuint> ShaderObjList;
	ShaderObjList m_ShaderObjList;

	GLuint m_Buffers[2];
	GLuint m_TransformFeedback[2];

	BoidSystem system;

	//Sim sim;
	Boid* boids;

	Vec3 m_Positions[NUM_BOIDS];
	float m_Velocities[NUM_BOIDS];

	float m_Scale, m_FPS;
	float m_FrustrumScale;

	int m_Time, m_OldTime, m_FrameCount;

	bool m_first;
};