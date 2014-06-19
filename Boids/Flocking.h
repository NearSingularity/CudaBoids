#include <gl\glew.h>
#include <GL\freeglut.h>
#include "Shader.h"
#include "MathHelpers.h"

//Flock dem boids via Geometry shader..
#define INVALID_UNIFORM_LOCATION 0xFFFFFFFF

struct float3
{
	float x, y, z;
};

struct float4
{
	float x, y, z, w;
};


class Flocking : public Shader
{
public:
    Flocking();
    
    virtual bool Init();    

	//Particles
    
    void SetParticleLifetime(float Lifetime);
    
    void SetDeltaTimeMillis(unsigned int DeltaTimeMillis);
    
    void SetTime(int Time);

    void SetRandomTextureUnit(unsigned int TextureUnit);
    
    void SetLauncherLifetime(float Lifetime);
    
    void SetShellLifetime(float Lifetime);
    
    void SetSecondaryShellLifetime(float Lifetime);


	//Boids

	//Vec3 Separate(Boid* boids[]);
	//Vec3 Align(Boid* boids[]);
	//Vec3 Cohesion(Boid* boids[]);
	//Vec3 Steer(Vec3 target, bool slow);

	void keyboard(unsigned char key, int /*x*/, int /*y*/);
	void mouse(int button, int state, int x, int y);
	void initialize();
	void motion(int x, int y);
	void display(void);
	void flock();
	
	static const unsigned int mesh_width = 256;
	static const unsigned int mesh_height = 256;

	float rnd1[mesh_width*mesh_height];
	float rnd2[mesh_width*mesh_height];

	int mouse_x, mouse_y;
	int mouse_buttons;
	float rotate_x, rotate_y, translate_z;//= -3.0;

	float anim;// = 0.0;

	float3 pos[mesh_width][mesh_height];
	float3 vel[mesh_width][mesh_height];
	float4 col[mesh_width][mesh_height];

    
 private:
    GLuint m_deltaTimeMillisLocation;
    GLuint m_randomTextureLocation;
    GLuint m_timeLocation;
    GLuint m_launcherLifetimeLocation;
    GLuint m_shellLifetimeLocation;
    GLuint m_secondaryShellLifetimeLocation;

};