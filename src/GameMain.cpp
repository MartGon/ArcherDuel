#include "MainGameLoop.h"
#include "LevelOne.h"
#include "MainMenu.h"
#include "GameMain.h"
#include <chrono>

#include <fstream>

enum NetworMode
{
	SERVER_MODE,
	CLIENT_MODE
};

int main(int argc, char* args[])
{
	// Setup 720x480
	gFirstScene = new MainMenu();

	// Set window res
	setWindowResolution(960, 520);

	// Engine loop
	engine_main();

	return 0;
}