//update an array of boids.
#pragma once
#include <GL/glew.h>
#include "Flocking.h"
#include "MathHelpers.h"


class BoidSystem
{
public:
    BoidSystem();
    
    ~BoidSystem();
    
    bool InitBoids(const Vec3& Pos);
	bool InitParticles(const Vec3& Pos);

    void Render(int DeltaTimeMillis, const Matrix4& VP, const Vec3& CameraPos);

	Flocking flock;
    
private:
    
    void UpdateBoids(int DeltaTimeMillis);
    void RenderBoids(const Matrix4& VP, const Vec3& CameraPos);
    
    bool m_isFirst;
    unsigned int m_currVB;
    unsigned int m_currTFB;
    GLuint m_boidBuffer[2];
	GLuint m_particleBuffer[2];
    GLuint m_transformFeedback[2];
    int m_time;
};

