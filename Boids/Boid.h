#pragma once
#include "MathHelpers.h"
#include "GL\freeglut.h"
#include "util.h"

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

class Boid2D
{
public:
	Boid2D();
	Boid2D(GLuint, int);
	Boid2D(GLuint, Vec2 p, int);
	Boid2D(GLuint, Vec3, int);
	~Boid2D();

	bool Init();
	bool InitMesh();
	bool LoadMesh();
	void Render(unsigned int num, const Matrix4* WVPMats, const Matrix4* WorldMats);
	void Clear();


	void AddForce(Vec2 force);
	void Seek(Vec2 target);
	void Avoid(Vec2 target);
	void Arrive(Vec2 target);
	Vec2 Separate(Boid2D* boids[]);
	Vec2 Align(Boid2D* boids[]);
	Vec2 Cohesion(Boid2D* boids[]);
	Vec2 Steer(Vec2 target, bool slow);

	void Flock(Boid2D* boids[], float&, float&, float&);

	void Update(Boid2D* boids[], float&, float&, float&);
	void Update(); //update shader
	void UpdateVelocity();

	//Need Set functions for shader
	
	GLuint texture;

	//for linking flock
	Boid2D* next, *mate, *enemy;

	Vec3 m_Pos, m_Dir, m_Vel, m_MaxVel, m_Acc;
	//Change to Vec3 soon
	Vec2 pos, dir, vel, maxVel, acc;

	float size, attraction, maxForce, maxSpeed, r;

	bool dead, slow;
	
	int index;

private:
	GLuint m_Buffers[3];
	GLuint m_VAO;
};