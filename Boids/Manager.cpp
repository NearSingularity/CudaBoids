#include "Manager.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>

//const int SCREEN_H = 1280;
//const int SCREEN_W = 720;

const UINT BOUND_X = 1024;
const UINT BOUND_Y = 1024;
const UINT BOUND_Z = 512;

#define WIDTH	 1024
#define HEIGHT	 512
#define DEPTH	 256
#define NUM_B    4096

#define EPS		10
#define DT		.1f

//float prev[BOUND_X*BOUND_Y*BOUND_Z];
//float next[BOUND_X*BOUND_Y*BOUND_Z];

int mouseX, mouseY, buttons;

float translate, time = 0.0f;

GLuint theProgram, gWVPLocation;
GLuint position, color, modelCamMat, camClipMat;

GLuint VBO, IBO;
void* vboBuffer = NULL;

UINT uiVBOverts;
UINT uiVBOindices;
UINT uiVAOmap;

float4 *dArray;
float* dPrev, *dNext;

static const char *pVS = "													\n\
#version 330																\n\
																			\n\
uniform mat4 projectionMatrix;												\n\
uniform mat4 modelViewMatrix;												\n\
uniform mat4 gWVP;															\n\
uniform vec3 offset;														\n\
																			\n\
layout (location = 0) in vec3 inPosition;									\n\
layout (location = 1) in vec3 inColor;										\n\
																			\n\
out vec4 theColor;															\n\
																			\n\
void main()																	\n\
{																			\n\
	gl_Position = gWVP * vec4(inPosition, 1.0);								\n\
	theColor = vec4(clamp(inPosition, 0.0, 1.0), 1.0);						\n\
}";

static const char *pFS = "													\n\
#version 330																\n\
																			\n\
in vec4 theColor;															\n\
out vec4 outputColor;														\n\
																			\n\
void main()																	\n\
{																			\n\
	outputColor = theColor;													\n\
}";																			

Vec3 fTriangle[3];

float fTetra[36];
float fRotationAngle = 0.0f;
const float PIover180 = 3.1415f/180.0f;

bool showFPS = false;
bool vertSync = true;

void CheckGL()
{
	GLenum err = glGetError();

	assert(err == GL_NO_ERROR);
}


Manager::Manager()
{
	m_Camera = NULL;
	//m_Mesh = NULL;

	m_Info.FOV = 60.0f;
	m_Info.height = SCREEN_HEIGHT;
	m_Info.width = SCREEN_WIDTH;
	m_Info.zFar = 100.0f;
	m_Info.zNear = 1.0f;

	m_FrameCount = 0;
	m_FPS = 0.0f;
	m_Scale = 0.0f;
	m_Time = m_OldTime = 0;
}

Manager::~Manager()
{
	SAFE_DELETE(m_Camera);
	//free(boids);
	//SAFE_DELETE(m_Mesh);
	//free Cuda
	//cudaFree(dArray);
	//cudaFree(dPrev);
	//cudaFree(dNext);
}

float Manager::CalcFrustrumScale(float fovDeg)
{
	const float degToRad = 3.14159f * 2.0f / 360.0f;
	float fovRad = fovDeg * degToRad;
	m_FrustrumScale = 1.0f / tan(fovRad / 2.0f);
	return m_FrustrumScale;
}

void Manager::InitCuda()
{
	//cudaMalloc(&dArray, width*height*sizeof(float4));
	//cudaMalloc(&dArray, width*height*sizeof(float4));
	//cudaMemcpy(dPrev, prev, width*height*sizeof(float), cudaMemcpyHostToDevice);
	//cudaMalloc(&dArray, width*height*sizeof(float4));
	//cudaMemcpy(dNext, next, width*height*sizeof(float), cudaMemcpyHostToDevice);

}

void Manager::InitVBO()
{
	//float4 *dPtr;
	//cudaGLMapBufferObject((void**)&dPtr, VBO);

	//dim3 block(8,8,8);
	//dim3 grid(width / block.x, height / block.y, depth / block.z);
	//	initialize_kernel<<<grid,block>>>(dPtr, width, height, time, dArray, dPrev, dNext);

	//cudaGLUnmapBufferObject(VBO);
}

bool Manager::Init()
{
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//Vec3 pos(0.0f, 0.0f, 0.0f);
	//Vec3 targ(0.0f, 0.0f, 0.0f);
	//Vec3 up(0.0f, 1.0f, 0.0f);
/*
	//Triangle
	fTriangle[0] = Vec3(1, 0, 0);
	fTriangle[1] = Vec3(0, 1, 0);
	fTriangle[2] = Vec3(-1, 0, 0);

	// Front face
	fTetra[0] = 0.0f; fTetra[1] = 5.0f; fTetra[2] = 0.0f;
	fTetra[3] = -3.0f; fTetra[4] = 0.0f; fTetra[5] = 3.0f;
	fTetra[6] = 3.0f; fTetra[7] = 0.0f; fTetra[8] = 3.0f;

	// Back face
	fTetra[9] = 0.0f; fTetra[10] = 5.0f; fTetra[11] = 0.0f;
	fTetra[12] = 3.0f; fTetra[13] = 0.0f; fTetra[14] = -3.0f;
	fTetra[15] = -3.0f; fTetra[16] = 0.0f; fTetra[17] = -3.0f;

	// Left face
	fTetra[18] = 0.0f; fTetra[19] = 5.0f; fTetra[20] = 0.0f;
	fTetra[21] = -3.0f; fTetra[22] = 0.0f; fTetra[23] = -3.0f;
	fTetra[24] = -3.0f; fTetra[25] = 0.0f; fTetra[26] = 3.0f;

	// Right face
	fTetra[27] = 0.0f; fTetra[28] = 5.0f; fTetra[29] = 0.0f;
	fTetra[30] = 3.0f; fTetra[31] = 0.0f; fTetra[32] = 3.0f;
	fTetra[33] = 3.0f; fTetra[34] = 0.0f; fTetra[35] = -3.0f;

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POSITIONS]);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * NUM_BOIDS * 5, );

	
	glGenVertexArrays(1, &uiVAOmap); //create one VAO
	CheckGL();
	glGenBuffers(1, &uiVBOverts); //create VBO for vert pos
	CheckGL();
	glGenBuffers(1, &uiVBOindices); //create VBO for index
	CheckGL();
	glBindVertexArray(uiVAOmap); //activate VAO
	CheckGL();
	glBindBuffer(GL_ARRAY_BUFFER, uiVBOverts); //Activate vert array
	CheckGL();
	glBufferData(GL_ARRAY_BUFFER, 36*sizeof(float), fTetra, GL_STATIC_DRAW);
	CheckGL(); 
	glEnableVertexAttribArray(0);
	CheckGL();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	CheckGL();
	//load shaders
*/
	//create program
	//theProgram = glCreateProgram();
	//add shaders
	//AddShader(theProgram, "shader.vert", GL_VERTEX_SHADER);
	//AddShader(theProgram, "shader.frag", GL_FRAGMENT_SHADER);

	//CheckGL();
	//enable depth test
	//clear depth

	//init boids here.

	//m_Mesh = new Mesh();


	m_Camera = new Camera(SCREEN_WIDTH, SCREEN_HEIGHT);
	
	Sim sim = {WIDTH, HEIGHT, DEPTH, NUM_B, EPS, DT, NULL, NULL, NULL};

	//boids = InitFlock(sim.n);
	sim.boids = boids;

	Vec3 temp = Vec3(0, 0, 0);
	system.InitParticles(temp);
	//system.InitBoids(temp);

	CreateVertexBuffer();
	CreateIndexBuffer(); 
	CompileShaders(theProgram);

	InitCuda();
	
	//init VBO

	m_Time = glutGet(GLUT_ELAPSED_TIME);

	//CalcPos();

	return true;
}

void Manager::RenderFPS()
{
	//glLoadIdentity();

	char text[32];
	
	SNPRINTF(text, sizeof(text), "FPS: %.2f", m_FPS);

	//sprintf(text, "FPS: %f", m_FPS);
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

			//m_Positions[index].x = (float)x;
			//m_Positions[index].y = RandomFloat() * 5.0f;
			//m_Positions[index].z = (float)y;
			//m_Velocities[index] = RandomFloat();

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
	//m_Camera->OnMouse(x, y);
	system.flock.motion(x, y);
}

void Manager::MouseCB(int button, int state, int x, int y)
{
	system.flock.mouse(button, state, x, y);	
}

void Manager::RenderCB()
{
	m_Time		= glutGet(GLUT_ELAPSED_TIME);
	int dt		= m_Time - m_OldTime;
	m_OldTime	= m_Time;
	CalcFPS();

	m_Camera->OnRender();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	p.Rotate(90.0f, 0.0f, 0.0f);
	p.WorldPos(0.0f, 0.0f, 5.0f);
	p.SetCamera(m_Camera->GetPos(), m_Camera->GetTarget(), m_Camera->GetUp());
	p.SetPerspectiveProj(m_Info);
	
	system.Render(dt, p.GetVPTrans(), m_Camera->GetPos());
	//Update system
	
	
	glUniformMatrix4fv(gWVPLocation, 1, GL_TRUE, (const GLfloat*)p.GetTrans());

	//glEnableVertexAttribArray(0);

	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	//glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

	//glDisableVertexAttribArray(0);
	
	glutSwapBuffers();
	glutPostRedisplay();

	/*
	glBindVertexArray(uiVAOmap);

	int modelView = glGetUniformLocation(theProgram, "molViewMatrix");
	CheckGL();
	int projView  = glGetUniformLocation(theProgram, "projectionMatrix");
	CheckGL();
	glm::mat4 mModelView = glm::lookAt(glm::vec3(0, 0, 40), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 mCurrent = glm::rotate(mModelView, fRotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(modelView, 1, GL_FALSE, glm::value_ptr(mCurrent));
	glDrawArrays(GL_TRIANGLES, 0, 12);

	
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
	*/
	//m_Mesh->Render(NUM_BOIDS, vpMatrices, worldMatrices);

	RenderFPS();

}

void Manager::CreateVertexBuffer()
{ 
	//Create a [] VBO of verts 
	//4 verts for pyramid (3 for base, 1 for point);
	Vec3 verts[4];
	//left
	verts[0] = Vec3(-1.0f, 0.0f, 0.0f);
	//right
	verts[1] = Vec3(1.0f, 0.0f, 0.0f);
	//center
	verts[2] = Vec3(0.0f, 0.5f, -1.0f);
	//top
	verts[3] = Vec3(0.0f, 1.0f, 0.0f);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Init VBO
	UINT size = NUM_BOIDS;
	//glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//cudaGLRegisterBufferObject(&VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
}

void Manager::CreateIndexBuffer()
{
	//Create a [] VBO of indices
	UINT indices[] = { 0, 3, 1,
					   1, 3, 2,
					   0, 3, 2,
					   0, 1, 2 };
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void Manager::AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(0);
    }

    const GLchar* p[1];
    p[0] = pShaderText;
    GLint Lengths[1];
    Lengths[0]= strlen(pShaderText);
    glShaderSource(ShaderObj, 1, p, Lengths);
    glCompileShader(ShaderObj);
    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }

    glAttachShader(ShaderProgram, ShaderObj);
}

void Manager::CompileShaders(GLuint& program)
{
	program = glCreateProgram();
	CheckGL();

    if (program == 0) {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

    AddShader(program, pVS, GL_VERTEX_SHADER);
    AddShader(program, pFS, GL_FRAGMENT_SHADER);

    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };

    glLinkProgram(program);
	CheckGL();
    glGetProgramiv(program, GL_LINK_STATUS, &Success);
	CheckGL();
	if (Success == 0) {
		glGetProgramInfoLog(program, sizeof(ErrorLog), NULL, ErrorLog);
		CheckGL();
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
	}

    glValidateProgram(program);
	CheckGL();
    glGetProgramiv(program, GL_VALIDATE_STATUS, &Success);
	CheckGL();
    if (!Success) {
        glGetProgramInfoLog(program, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    glUseProgram(program);
	CheckGL();
    gWVPLocation = glGetUniformLocation(program, "gWVP");
    assert(gWVPLocation != 0xFFFFFFFF);
}
/*
static Boid* InitFlock(int n)
{
	int i, j, cur = 0;
	float sqrt_n = ceilf(sqrtf(n)), dx = WIDTH / (sqrt_n + 1),
		  dy = HEIGHT / (sqrt_n + 1);
	Boid *boids = calloc(n, sizeof(Boid));
	if (!boids)
		return NULL;
	for (i = 0; i < sqrt_n; ++i)
		for (j = 0; j < sqrt_n && cur < n; ++j) {
			boids[cur].vX = sinf((1559 * cur) ^ 50969);
			boids[cur].vY = cosf((1567 * cur) ^ 51853);
			boids[cur].vZ = 0;
			boids[cur].y = (i + 1) * dy;
			boids[cur].x = (j + 1) * dx;
			boids[cur].z = 0;
			++cur;
		}
	return boids;
}

static void Draw(Sim *s) {
	int i, j;
	char value;
	memset(screen->pixels, 0, HEIGHT * screen->pitch);
	#pragma omp parallel for private(j, value)
	glBegin(GL_POINTS);
	for (i = 0; i < sp->height; ++i)
		for (j = 0; j < sp->width; ++j) {
			value = s->intensity[i * s->width + j];
			//set_pixel(screen, j, i, value, value, value);
		}
	glEnd();
	//if (sp->attractor)
	//	draw_attractor(screen, sp->attractor);
	//if (SDL_MUSTLOCK(screen))
	//	SDL_UnlockSurface(screen);
	//SDL_Flip(screen); 
}

static void Simulation(Sim* s)
{
	Simulate(sp);
	Draw(sp);
}
*/