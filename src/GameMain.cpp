#include "MainGameLoop.h"
#include "LevelOne.h"
#include "MainMenu.h"

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
	setWindowResolution(960, 640);

	// Engine loop
	engine_main();

	return 0;
}