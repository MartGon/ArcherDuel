#include "MainGameLoop.h"
#include "LevelOne.h"

int main(int argc, char* args[])
{
	// Setup 720x480
	gFirstScene = new LevelOne();
	setWindowResolution(960, 640);

	// Engine loop
	engine_main();

	return 0;
}