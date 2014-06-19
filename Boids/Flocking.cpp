#include "Flocking.h"
#include "engine_common.h"

#define CHEAT	1
#define REAL	0
#define REDUCE	0


static const char* pVS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in float Type;                                                \n\
layout (location = 1) in vec3 Position;                                             \n\
layout (location = 2) in vec3 Velocity;                                             \n\
layout (location = 3) in float Age;                                                 \n\
                                                                                    \n\
out float Type0;                                                                    \n\
out vec3 Position0;                                                                 \n\
out vec3 Velocity0;                                                                 \n\
out float Age0;                                                                     \n\
out vec4 theColor																	\n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    Type0 = Type;                                                                   \n\
    Position0 = Position;                                                           \n\
    Velocity0 = Velocity;                                                           \n\
    Age0 = Age;                                                                     \n\
	theColor = vec4(clamp(Position, 0.75, 1.0), 1.0);								\n\
}";

static const char* pFS = "															\n\
in vec4 theColor																	\n\
out vec4 outputColor																\n\
																					\n\
void main()																			\n\
{																					\n\
	outputColor = theColor;															\n\
}";	

static const char* pGS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
layout(points) in;                                                                  \n\
layout(points) out;                                                                 \n\
layout(max_vertices = 30) out;                                                      \n\
                                                                                    \n\
in float Type0[];                                                                   \n\
in vec3 Position0[];                                                                \n\
in vec3 Velocity0[];                                                                \n\
in float Age0[];                                                                    \n\
                                                                                    \n\
out float Type1;                                                                    \n\
out vec3 Position1;                                                                 \n\
out vec3 Velocity1;                                                                 \n\
out float Age1;                                                                     \n\
                                                                                    \n\
uniform float gDeltaTimeMillis;                                                     \n\
uniform float gTime;                                                                \n\
uniform sampler1D gRandomTexture;                                                   \n\
uniform float gLauncherLifetime;                                                    \n\
uniform float gShellLifetime;                                                       \n\
uniform float gSecondaryShellLifetime;                                              \n\
                                                                                    \n\
#define PARTICLE_TYPE_LAUNCHER 0.0f                                                 \n\
#define PARTICLE_TYPE_SHELL 1.0f                                                    \n\
#define PARTICLE_TYPE_SECONDARY_SHELL 2.0f                                          \n\
                                                                                    \n\
vec3 GetRandomDir(float TexCoord)                                                   \n\
{                                                                                   \n\
     vec3 Dir = texture(gRandomTexture, TexCoord).xyz;                              \n\
     Dir -= vec3(0.5, 0.5, 0.5);                                                    \n\
     return Dir;                                                                    \n\
}                                                                                   \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    float Age = Age0[0] + gDeltaTimeMillis;                                         \n\
                                                                                    \n\
    if (Type0[0] == PARTICLE_TYPE_LAUNCHER) {                                       \n\
        if (Age >= gLauncherLifetime) {                                             \n\
            Type1 = PARTICLE_TYPE_SHELL;                                            \n\
            Position1 = Position0[0];                                               \n\
            vec3 Dir = GetRandomDir(gTime/1000.0);                                  \n\
            Dir.y = max(Dir.y, 0.5);                                                \n\
            Velocity1 = normalize(Dir) / 20.0;                                      \n\
            Age1 = 0.0;                                                             \n\
            EmitVertex();                                                           \n\
            EndPrimitive();                                                         \n\
            Age = 0.0;                                                              \n\
        }                                                                           \n\
                                                                                    \n\
        Type1 = PARTICLE_TYPE_LAUNCHER;                                             \n\
        Position1 = Position0[0];                                                   \n\
        Velocity1 = Velocity0[0];                                                   \n\
        Age1 = Age;                                                                 \n\
        EmitVertex();                                                               \n\
        EndPrimitive();                                                             \n\
    }                                                                               \n\
    else {                                                                          \n\
        float DeltaTimeSecs = gDeltaTimeMillis / 1000.0f;                           \n\
        float t1 = Age0[0] / 1000.0;                                                \n\
        float t2 = Age / 1000.0;                                                    \n\
        vec3 DeltaP = DeltaTimeSecs * Velocity0[0];                                 \n\
        vec3 DeltaV = vec3(DeltaTimeSecs) * (0.0, -9.81, 0.0);                      \n\
                                                                                    \n\
        if (Type0[0] == PARTICLE_TYPE_SHELL)  {                                     \n\
	        if (Age < gShellLifetime) {                                             \n\
	            Type1 = PARTICLE_TYPE_SHELL;                                        \n\
	            Position1 = Position0[0] + DeltaP;                                  \n\
	            Velocity1 = Velocity0[0] + DeltaV;                                  \n\
	            Age1 = Age;                                                         \n\
	            EmitVertex();                                                       \n\
	            EndPrimitive();                                                     \n\
	        }                                                                       \n\
            else {                                                                  \n\
                for (int i = 0 ; i < 10 ; i++) {                                    \n\
                     Type1 = PARTICLE_TYPE_SECONDARY_SHELL;                         \n\
                     Position1 = Position0[0];                                      \n\
                     vec3 Dir = GetRandomDir((gTime + i)/1000.0);                   \n\
                     Velocity1 = normalize(Dir) / 20.0;                             \n\
                     Age1 = 0.0f;                                                   \n\
                     EmitVertex();                                                  \n\
                     EndPrimitive();                                                \n\
                }                                                                   \n\
            }                                                                       \n\
        }                                                                           \n\
        else {                                                                      \n\
            if (Age < gSecondaryShellLifetime) {                                    \n\
                Type1 = PARTICLE_TYPE_SECONDARY_SHELL;                              \n\
                Position1 = Position0[0] + DeltaP;                                  \n\
                Velocity1 = Velocity0[0] + DeltaV;                                  \n\
                Age1 = Age;                                                         \n\
                EmitVertex();                                                       \n\
                EndPrimitive();                                                     \n\
            }                                                                       \n\
        }                                                                           \n\
    }                                                                               \n\
}                                                                                   \n\
";

float3 make_float3(float X, float Y, float Z)
{
	float3 temp = {X,Y,Z};
	return temp;
}

float4 make_float4(float X, float Y, float Z, float W)
{
	float4 temp = {X,Y,Z, W};
	return temp;
}


Flocking::Flocking()
{
    mouse_buttons = 0;
	rotate_x = rotate_y = anim = 0;
	translate_z = -3.0;
}
    
    
bool Flocking::Init()
{
    if (!Shader::Init()) {
        return false;
    }

    if (!Add(GL_VERTEX_SHADER, pVS)) {
        return false; 
    } 
	if(!Add(GL_FRAGMENT_SHADER, pFS)) {
		return false;
	}
    if (!Add(GL_GEOMETRY_SHADER, pGS)) {
        return false;
    }

    const GLchar* Varyings[4];    
    Varyings[0] = "Type1";
    Varyings[1] = "Position1";
    Varyings[2] = "Velocity1";    
    Varyings[3] = "Age1";
    
    glTransformFeedbackVaryings(m_shaderProg, 4, Varyings, GL_INTERLEAVED_ATTRIBS);

    if (!Finalize()) {
        return false;
    }
    
    m_deltaTimeMillisLocation = GetUniformLocation("gDeltaTimeMillis");
    m_randomTextureLocation = GetUniformLocation("gRandomTexture");
    m_timeLocation = GetUniformLocation("gTime");
    m_launcherLifetimeLocation = GetUniformLocation("gLauncherLifetime");
    m_shellLifetimeLocation = GetUniformLocation("gShellLifetime");
    m_secondaryShellLifetimeLocation = GetUniformLocation("gSecondaryShellLifetime");

    if (m_deltaTimeMillisLocation == INVALID_UNIFORM_LOCATION ||
        m_timeLocation == INVALID_UNIFORM_LOCATION ||
        m_randomTextureLocation == INVALID_UNIFORM_LOCATION ||
        m_launcherLifetimeLocation == INVALID_UNIFORM_LOCATION ||
        m_shellLifetimeLocation == INVALID_UNIFORM_LOCATION ||
        m_secondaryShellLifetimeLocation == INVALID_UNIFORM_LOCATION) {
        return false;
    }
    
    return true;
}


void Flocking::SetDeltaTimeMillis(unsigned int DeltaTimeMillis)
{
    glUniform1f(m_deltaTimeMillisLocation, (float)DeltaTimeMillis);
}


void Flocking::SetTime(int Time)
{
    glUniform1f(m_timeLocation, (float)Time);
}


void Flocking::SetRandomTextureUnit(unsigned int TextureUnit)
{    
    glUniform1i(m_randomTextureLocation, TextureUnit);
}


void Flocking::SetLauncherLifetime(float Lifetime)
{
    glUniform1f(m_launcherLifetimeLocation, Lifetime);
}


void Flocking::SetShellLifetime(float Lifetime)
{
    glUniform1f(m_shellLifetimeLocation, Lifetime);
}


void Flocking::SetSecondaryShellLifetime(float Lifetime)
{
    glUniform1f(m_secondaryShellLifetimeLocation, Lifetime);
}


void Flocking::keyboard(unsigned char key, int x, int y)
{
    switch(key) {
    case(27) :
        exit(0);
        break;
	case('a'):
		if (mouse_buttons!=10)
			mouse_buttons=10;
		else
			mouse_buttons=0;
		break;
    }
}

void Flocking::mouse(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN) {
        mouse_buttons |= 1<<button;
    } else if (state == GLUT_UP) {
        mouse_buttons = 0;
    }

    mouse_x = x;
    mouse_y = y;
    glutPostRedisplay();
}

void Flocking::motion(int x, int y)
{
    float dx, dy;
    dx = (float)x - mouse_x;
    dy = (float)y - mouse_y;

    if (mouse_buttons & 1) {
        rotate_x += dy * 0.2f;
        rotate_y += dx * 0.2f;
    } else if (mouse_buttons & 4) {
        translate_z += dy * 0.01f;
    }

    mouse_x = x;
    mouse_y = y;
}

void Flocking::initialize()
{
#if REDUCE
	for (int i=0;i<(mesh_height*.5f)*(mesh_width*.5f);++i)
		rnd1[i]=(rand()%100-100)/2000.0f;
	for (int i=0;i<mesh_height*mesh_width;++i)
		rnd2[i]=(rand()%100-100)/2000.0f;
	#pragma omp parallel for
	for (int x=0;x<mesh_width*.5f;++x)
		for (int y=0;y<mesh_height*.5f;++y)
		{
			float u = x / (float) mesh_width/2 + rnd1[y*(mesh_width*.5f)+x];
			float v = y / (float) mesh_height/2 + rnd2[y**mesh_height*.5f)+x];

			float freq = 2.0f;
			float w = sin(u*freq + anim) * cos(v*freq + anim) * 0.2f;

			col[y][x] = make_float4(0,255,255,1);
			pos[y][x] = make_float3(u, w, v);
			vel[y][x] = make_float3(0.0, 0.0, 0.0);
		}
#endif
	for (int i=0;i<mesh_height*mesh_width;++i)
		rnd1[i]=(rand()%100-100)/2000.0f;
	for (int i=0;i<mesh_height*mesh_width;++i)
		rnd2[i]=(rand()%100-100)/2000.0f;
	#pragma omp parallel for
	for (int x=0;x<mesh_width;++x)
		for (int y=0;y<mesh_height;++y)
		{
			float u = x / (float) mesh_width + rnd1[y*mesh_width+x];
			float v = y / (float) mesh_height + rnd2[y*mesh_height+x];

			float freq = 2.0f;
			float w = sin(u*freq + anim) * cos(v*freq + anim) * 0.2f;

			col[y][x] = make_float4(0,255,255,1);
			pos[y][x] = make_float3(u, w, v);
			vel[y][x] = make_float3(0.0, 0.0, 0.0);
		}
}

void Flocking::flock()
{
	INT_MAX;
	const float speed = 0.0005f;
	const float threshold = 0.1f;
	#pragma omp parallel for
	for (int x=0;x<mesh_width;++x)
		for (int y=0;y<mesh_height;++y)
		{
			float u = x / (float) mesh_width;
			float v = y / (float) mesh_height;
			float xX = (mouse_x - (float)mesh_width/2-256)/(float)mesh_width/2;
			float yY = (mouse_y - (float)mesh_height/2-256)/(float)mesh_height/2;
			float dx = -pos[y][x].x + xX;
			float dz = -pos[y][x].z + yY;
			float length = sqrt(dx*dx+dz*dz);
			if (mouse_buttons==10)
			{
				vel[y][x].x=0;
				vel[y][x].z=0;
				dx = -pos[y][x].x + u;
				dz = -pos[y][x].z + v;
				length = sqrt(dx*dx+dz*dz);
				pos[y][x].x+=dx/length*speed*10;
				pos[y][x].z+=dz/length*speed*10;
			}
			else if (!(mouse_buttons & 4) && !(mouse_buttons & 6))
			{
				float norX = dx/length*speed;
				float norZ = dz/length*speed;
				vel[y][x].x+=norX;
				vel[y][x].z+=norZ;
				dx = vel[y][x].x;
				dz = vel[y][x].z;
				float velocity = sqrt(dx*dx+dz*dz);
				if (velocity>threshold)
				{
					vel[y][x].x=dx/velocity*threshold;
					vel[y][x].z=dz/velocity*threshold;
				}
				float green = (int)(255/(velocity*51))/255.0f;
				if (green>=1.0f)
					green=1.0f;
				col[y][x] = make_float4(128/length/255.0f,green,1.0f,0.1f);
				if (pos[y][x].x<-5.0f && vel[y][x].x<0.0f)
					vel[y][x].x=-vel[y][x].x;
				if (pos[y][x].x>5.0f && vel[y][x].x>0.0f)
					vel[y][x].x=-vel[y][x].x;
				pos[y][x].x+=vel[y][x].x;
				pos[y][x].z+=vel[y][x].z;
			}
			else if (!(mouse_buttons & 4))
			{
				vel[y][x].x=0;
				vel[y][x].z=0;
				pos[y][x].x+=dx/length*speed*10;
				pos[y][x].y+=dx/length*speed*10;
				pos[y][x].z+=dz/length*speed*10;
				col[y][x] = make_float4(1.0f/length,1.0f/length, 1.0f, 10);
			}
			float freq = 2.0f;
			float w = sin(u*freq + anim) * cos(v*freq + anim) * 0.2f;
			pos[y][x].y=w;
		}
}

void Flocking::display(void)
{
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	flock();

	glBegin(GL_POINTS);
		for (int x=0;x<mesh_width;++x)
			for (int y=0;y<mesh_height;++y)
			{
				glColor4f(col[y][x].x, col[y][x].y, col[y][x].z, col[y][x].z);
				glVertex3f(pos[y][x].x, pos[y][x].y, pos[y][x].z);
			}
	glEnd();


    anim += 0.01f;
}