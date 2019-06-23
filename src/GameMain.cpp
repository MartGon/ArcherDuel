#include "MainGameLoop.h"
#include "LevelOne.h"
#include "MainMenu.h"
#include <chrono>

enum NetworMode
{
	SERVER_MODE,
	CLIENT_MODE
};

int main(int argc, char* args[])
{
	// Setup 720x480
	// Previous 1440 960
	gFirstScene = new MainMenu();
	
	/*Scene::SceneMode mode = Scene::SceneMode::ONLINE_SERVER;
#ifdef _DEBUG
	mode = Scene::SceneMode::ONLINE_CLIENT;
#endif
	gFirstScene->setSceneMode(mode);*/

	setWindowResolution(960, 520);

	// Engine loop
	engine_main();

	return 0;
}