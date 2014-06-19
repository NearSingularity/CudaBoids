#pragma once
#include "engine_common.h"
#include "util.h"
#include "BoidSystem.h"
#include "MathHelpers.h"
//#include "Boid.h"

#define MAX_BOIDS 1000

#define MAX_PARTICLES 1000
#define PARTICLE_LIFETIME 10.0f

#define PARTICLE_TYPE_LAUNCHER 0.0f
#define PARTICLE_TYPE_SHELL 1.0f
#define PARTICLE_TYPE_SECONDARY_SHELL 2.0f


//Update Pos, velocity in Vertex Shader,
//Then use Geom Shader to create new verts (for pyramid) based on world pos in VS
struct Boidd
{
    Vec3 Pos;
    Vec3 Vel;
	Vec3 Normal;
};

struct Particle
{
	float Type; 
	float LifetimeMillis;  
    Vec3 Pos;
    Vec3 Vel;
};


BoidSystem::BoidSystem()
{
    m_currVB = 0;
    m_currTFB = 1; 
    m_isFirst = true;
    m_time = 0;
    //m_pTexture = NULL;            
    
    ZERO_MEM(m_transformFeedback);
    ZERO_MEM(m_boidBuffer);
}


BoidSystem::~BoidSystem()
{
    //SAFE_DELETE(m_pTexture);
    
    if (m_transformFeedback[0] != 0) {
        glDeleteTransformFeedbacks(2, m_transformFeedback);
    }
    
    if (m_boidBuffer[0] != 0) {
        glDeleteBuffers(2, m_boidBuffer);
    }
}

bool BoidSystem::InitBoids(const Vec3& Pos)
{
	Boid boids[MAX_BOIDS];
    ZERO_MEM(boids);

    //boids[0].Type = PARTICLE_TYPE_LAUNCHER;
	//boids[0].Pos = Pos;
	//boids[0].Vel = Vec3(0.0f, 0.0f, 0.0f);
    //boids[0].LifetimeMillis = 0.0f;

    glGenTransformFeedbacks(2, m_transformFeedback);    
    glGenBuffers(2, m_boidBuffer);
    
	//update normals and position.
    for (unsigned int i = 0; i < 2 ; i++) {
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[i]);
        glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(boids), boids, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_boidBuffer[i]);        
    }
                      
   
	if (!flock.Init()) {
       return false;
    }
    
    flock.Enable();
    
    flock.SetRandomTextureUnit(RANDOM_TEXTURE_UNIT_INDEX);
    flock.SetLauncherLifetime(100.0f);
    flock.SetShellLifetime(10000.0f);
    flock.SetSecondaryShellLifetime(2500.0f);

	return GLCheckError();
}

bool BoidSystem::InitParticles(const Vec3& Pos)
{   
    Particle particles[MAX_PARTICLES];
    ZERO_MEM(particles);

    particles[0].Type = PARTICLE_TYPE_LAUNCHER;
	particles[0].Pos = Pos;
	particles[0].Vel = Vec3(0.0f, 0.0f, 0.0f);
    particles[0].LifetimeMillis = 0.0f;

    glGenTransformFeedbacks(2, m_transformFeedback);    
    glGenBuffers(2, m_boidBuffer);
    
	//update normals and position.
    for (unsigned int i = 0; i < 2 ; i++) {
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[i]);
        glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(particles), particles, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_boidBuffer[i]);        
    }
         
	flock.initialize();

	if (!flock.Init()) {
       return false;
    }
    
    //flock.Enable();
    
   // flock.SetRandomTextureUnit(RANDOM_TEXTURE_UNIT_INDEX);
    //flock.SetLauncherLifetime(100.0f);
   // flock.SetShellLifetime(10000.0f);
    //flock.SetSecondaryShellLifetime(2500.0f);
     /*
    if (!m_randomTexture.InitRandomTexture(1000)) {
        return false;
    }
    
    m_randomTexture.Bind(RANDOM_TEXTURE_UNIT);

    if (!m_billboardTechnique.Init()) {
        return false;
    }
    
    m_billboardTechnique.Enable();

    m_billboardTechnique.SetColorTextureUnit(COLOR_TEXTURE_UNIT_INDEX);

    m_billboardTechnique.SetBillboardSize(0.01f);
    
    m_pTexture = new Texture(GL_TEXTURE_2D, "fireworks_red.jpg");
    
    if (!m_pTexture->Load()) {
        return false;
    }        
    */
    return GLCheckError();
}


void BoidSystem::Render(int DeltaTimeMillis, const Matrix4& VP, const Vec3& CameraPos)
{
    m_time += DeltaTimeMillis;

	flock.display();
    
    //UpdateBoids(DeltaTimeMillis);
    //RenderBoids(VP, CameraPos);

    m_currVB = m_currTFB;
    m_currTFB = (m_currTFB + 1) & 0x1;

}


void BoidSystem::UpdateBoids(int DeltaTimeMillis)
{
    flock.Enable();
    flock.SetTime(m_time);
    flock.SetDeltaTimeMillis(DeltaTimeMillis);
   
   // m_randomTexture.Bind(RANDOM_TEXTURE_UNIT);
    
    glEnable(GL_RASTERIZER_DISCARD);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[m_currVB]);    
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[m_currTFB]);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(Boid), 0);							// type
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Boid), (const GLvoid*)4);			// position
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Boid), (const GLvoid*)16);			// velocity
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Boid), (const GLvoid*)28);			// lifetime
    
    glBeginTransformFeedback(GL_POINTS);

    if (m_isFirst) {
        glDrawArrays(GL_POINTS, 0, 1);

        m_isFirst = false;
    }
    else {
        glDrawTransformFeedback(GL_POINTS, m_transformFeedback[m_currVB]);
    }            
    
    glEndTransformFeedback();

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
}

    

void BoidSystem::RenderBoids(const Matrix4& VP, const Vec3& CameraPos)
{
    //m_billboardTechnique.Enable();
    //m_billboardTechnique.SetCameraPosition(CameraPos);
    //m_billboardTechnique.SetVP(VP);
    //m_pTexture->Bind(COLOR_TEXTURE_UNIT);
    
    glDisable(GL_RASTERIZER_DISCARD);

    glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[m_currTFB]);    

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Boid), (const GLvoid*)4);  // position

    glDrawTransformFeedback(GL_POINTS, m_transformFeedback[m_currTFB]);

    glDisableVertexAttribArray(0);
}
