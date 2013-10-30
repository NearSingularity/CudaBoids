#pragma once
#include "MathHelpers.h"
#include "GL\freeglut.h"
#include "util.h"

const int SCREEN_W = 1024;
const int SCREEN_H = 768;

const int MAX_BOIDS = 200;
const int RADIUS	= 15;
const int NEIGHBORHOOD = 500;

const int FOV_DIST	= 30;
const int ANGLE		= 140;

struct Vertex
{
	Vec3 m_Pos, m_Normal;

	Vertex() {}
	Vertex(const Vec3& p, const Vec3& n)
	{
		m_Pos = p;
		m_Normal = n;
	}
};

class Boid
{
public:
	Boid(GLuint, int);
	Boid(GLuint, Vec2 p, int);
	~Boid();

	bool Init();
	bool InitMesh();
	bool LoadMesh();
	void Render(unsigned int num, const Matrix4* WVPMats, const Matrix4* WorldMats);
	void Clear();


	void AddForce(Vec2 force);
	void Seek(Vec2 target);
	void Avoid(Vec2 target);
	void Arrive(Vec2 target);
	Vec2 Separate(Boid* boids[]);
	Vec2 Align(Boid* boids[]);
	Vec2 Cohesion(Boid* boids[]);
	Vec2 Steer(Vec2 target, bool slow);

	void Flock(Boid* boids[], float&, float&, float&);

	void Update(Boid* boids[], float&, float&, float&);
	void Update();
	void UpdateVelocity();
	
	GLuint texture;

	//for linking flock
	Boid* next, *mate, *enemy;

	//Change to Vec3 soon.
	Vec2 pos, dir, vel, maxVel, acc;

	float size, attraction, maxForce, maxSpeed, r;

	bool dead, slow;
	
	int index;

private:

#define INVALID_MATERIAL 0xFFFFFFF
#define INDEX_BUFFER 0 
#define POS_		 1
#define NORMAL_		 2
#define TEXCOORD_	 3
#define WVP_MAT		 4
#define WORLD_MAT	 5

	GLuint m_VAO;
	GLuint m_Buffers[3];

	struct MeshEntry {
		MeshEntry()
		{
			numIndices = 0;
			baseVertex = 0;
			baseIndex  = 0;
			matIndex   = INVALID_MATERIAL;
		}
		unsigned int numIndices, baseVertex, baseIndex, matIndex;
	};

	//can later convert to vector or [] to produce multiple meshs
	MeshEntry entry;


};