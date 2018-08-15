#include "MainGameLoop.h"
#include "MainMenu.h"

int main(int argc, char* args[])
{
	// Setup
	gFirstScene = new MainMenu();
	setWindowResolution(720, 480);

	// Engine loop
	engine_main();

	return 0;
}