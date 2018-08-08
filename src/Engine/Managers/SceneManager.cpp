#include "SceneManager.h"

Scene* SceneManager::scene = nullptr;
bool SceneManager::quit = false;

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::loadScene(Scene &s)
{
	if (scene)
		scene->destroy();

	scene = &s;
	scene->loadMedia();
	scene->start();
}

void SceneManager::quitGame()
{
	quit = true;
}