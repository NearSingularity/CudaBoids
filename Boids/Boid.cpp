#include "Boid.h"
#include "GL\glew.h"
#include <assert.h>

#define POS_LOC		0
#define TEX_COORD	1
#define NORMAL_LOC	2
#define WVP_LOC		3
#define WORLD_LOC	7

Boid::Boid(GLuint tex, int i)
{
	pos = Vec2(rand() % SCREEN_H, rand() % SCREEN_H);
	//vel = Vec2(rand() % 10, rand() % 10);
	maxVel = Vec2(10, 10);

	dir = Vec2(0, 0);
	acc = Vec2(0,0);

	texture = tex;

	maxSpeed = 1;
	attraction = 0.1f;
	r = 3;

	index = i;
	m_VAO = 0;
}

Boid::Boid(GLuint tex, Vec2 p, int i)
{
	texture = tex;
	pos = p;
	//vel = Vec2(rand() % 10, rand() % 10);
	maxVel = Vec2(10, 10);

	dir = Vec2(0, 0);
	acc = Vec2(0,0);

	texture = tex;

	maxSpeed = 10;
	attraction = 0.1f;
	r = 3;

	index = i;
}

Boid::~Boid()
{

}

void Boid::Clear()
{
	if(m_Buffers[0] != 0)
		glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
	if(m_VAO != 0)
	{
		glDeleteVertexArrays(1, &m_VAO);
		m_VAO = 0;
	}
}

bool Boid::InitMesh()
{
	
}

bool Boid::LoadMesh()
{
	bool flag = false;
	Clear();

	//create VAO
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
	//create buffer for vertice attributes
	glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

	flag = InitMesh();

	glBindVertexArray(0);

	return flag;

}

void Boid::AddForce(Vec2 force)
{
	//acc += force / mass;
	acc += force;
}

void Boid::UpdateVelocity()
{

}

void Boid::Update()
{
	UpdateVelocity();
}

void Boid::Seek(Vec2 target)
{
	acc += Steer(target, false);
}

void Boid::Avoid(Vec2 target)
{
	acc -= Steer(target, false);
}

void Boid::Arrive(Vec2 target)
{
	acc += Steer(target, true);
}

//Steer to avoid crowding local flockmates
Vec2 Boid::Separate(Boid* boids[MAX_BOIDS])
{
	Vec2 steer = Vec2(0,0);
	int count = 0;

	for(int i = 0; i < MAX_BOIDS; i++)
	{
		Boid *other = boids[i];

		if(other->index != index)
		{
			float d = pos.Distance(other->pos);

			//if distance is <= RADIUS + safe val (close to it) 
			if(d <= NEIGHBORHOOD)
			{
				//get direction of difference from other nearby boid to self
				Vec2 dif = Vec2(pos.x - other->pos.x, pos.y - other->pos.y);
				//steer away 
				steer += dif;
				//count how many are in close proximity
				count++;
			}
		}
	}
	if(count > 0)
		steer /= (float)count;

	
	steer.Normalize();
	return steer;
}

Vec2 Boid::Align(Boid* boids[MAX_BOIDS])
{
	Vec2 steer = Vec2(0,0);
	int count = 0;

	for(int i = 0; i < MAX_BOIDS; i++)
	{
		Boid* other = boids[i];

		if(other->index != index)
		{
			float d = pos.Distance(other->pos);

			if(d < NEIGHBORHOOD)
			{
				steer += other->vel;
				count++;
			}
		}
	}

	if(count > 0)
		steer /= (float)count;

	steer.Normalize();

	return steer;
}

Vec2 Boid::Cohesion(Boid* boids[MAX_BOIDS])
{
	Vec2 steer = Vec2(0,0);
	int count = 0;

	for (int i = 0; i < MAX_BOIDS; i++)
	{
		Boid* other = boids[i];

		if(other->index != index)
		{
			float d = pos.Distance(other->pos);

			if( d < NEIGHBORHOOD)
			{
				steer += other->pos;
				count++;
			}
		}
	}
	if(count > 0)
	{
		steer /= (float) count;
	}
	
	Vec2 distance = Vec2(steer.x - pos.x, steer.y - pos.y);
	distance.Normalize();
	return distance;
}

void Boid::Flock(Boid* boids[MAX_BOIDS], float& s, float& a, float& c)
{
	Vec2 sep = Separate(boids);
	Vec2 ali = Align(boids);
	Vec2 coh = Cohesion(boids);

	sep *= s;
	ali *= a;
	coh *= c;

	acc = sep + ali + coh;
}


Vec2 Boid::Steer(Vec2 target, bool slow)
{
	Vec2 steer;
	Vec2 dest = target - pos;
	float safeDistance = 35.0f;

	float d = pos.Distance(target);

	if(d > 0)
	{
		dest /= d;

		if( (slow) && (d < safeDistance) )
			dest *= maxSpeed * (d/safeDistance);
		else
			dest *= maxSpeed;

		steer = dest - vel;
		steer.Clamp(attraction);
		//clamp steer.x and steer.y with -attraction +attraction
	}
	return steer;
}

void Boid::Update(Boid* boids[MAX_BOIDS], float& s, float& a, float& c)
{
	Flock(boids, s, a, c);
	//normalize acceleration
	acc.Normalize();
	//times it by hardcoded speed
	
	//add velocity
	vel += acc;

	vel.Normalize();

	vel *= maxSpeed;

	//adjust position
	pos += vel;
	//reset acceleration
	//acc = Vec2(0,0);

	if(pos.x <= 0)
	{
#if BOUNCE
		vel.x = -1;
#endif
#if WRAP
		pos.x = SCREEN_W;
#endif
	}
	if(pos.x >= SCREEN_W)
	{
#if BOUNCE
		vel.x = -1;
#endif
#if WRAP
		pos.x = 0;
#endif
	}
	if(pos.y <= 0)
	{
#if BOUNCE
		vel.y *= -1;
#endif
#if WRAP
		pos.y = SCREEN_H;
#endif
	}
	if(pos.y > SCREEN_H)
	{
#if BOUNCE
		vel.y = -vel.y;
#endif
#if WRAP
		pos.y = 0;
#endif
	}
}