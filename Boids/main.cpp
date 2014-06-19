#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <ctime>
#include "Manager.h"
#include "util.h"


int main(int argc, char** argv)
{
	GLUTBackendInit(argc, argv);

	if(!GLUTBackendCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, 32, false, "Cuda Boids"))
		return 1;

	SRANDOM;

	Manager* gm = new Manager();

	if(!gm->Init())
		return 1;

	gm->Run();

	delete gm;
	return 0;
}