#include "MainMenu.h"
#include "MainGameLoop.h"

void MainMenu::loadMedia()
{
	const char* gameTitleTexturePath = "GameTitle.png";
	Texture gameTitleTexture = Texture(gameTitleTexturePath, renderer);
	GameObject *gameTitle = new GameObject(gameTitleTexture);
	gameTitle->setScale(Vector2<float>(1.5f, 1.5f));
	gameTitle->transform.position = Vector2<float>(WINDOW_WIDTH / 2 - gameTitleTexture.mWidth / 2 * gameTitle->transform.scale.x, 0);
}