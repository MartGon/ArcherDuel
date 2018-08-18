#include "MainMenu.h"
#include "MainGameLoop.h"
#include "TextureRenderer.h"
#include "RendererManager.h"
#include "Animator.h"
#include "Navigator.h"
#include "Button.h"
#include "Texture.h"
#include "Transform.h"


void MainMenu::loadMedia()
{
	bow = new Bow();
	Vector2<int> nativeRes = RendererManager::getNativeResolution();
	bow->setRelativePosition(Vector2<float>(nativeRes.x/2, nativeRes.y/2));

	MapRGB *colorKey = new MapRGB();
	colorKey->blue = 0xFF;

	arrow = new GameObject();
	TextureRenderer* aRenderer = arrow->setComponent(new TextureRenderer("ArrowTrim.png", colorKey));
	Navigator* nav = arrow->setComponent(new Navigator());
	nav->isEnabled = false;
	// x + 6, y + 13
	arrow->transform.position = Vector2<float>(bow->transform.position.x + 6, bow->transform.position.y + 13);

	// Rotation
	aRenderer->center = new Vector2<int>(2, 2);

	// Setting
	bow->arrow = arrow;
}

void MainMenu::onClickBow()
{
	printf("I was clicked\n");
}

void MainMenu::handleEvent(const SDL_Event& event)
{
	TextureRenderer *aRenderer = nullptr;

	switch (event.key.keysym.sym)
	{
	case SDLK_UP:
		bow->transform.zRotation += 5;
		arrow->transform.zRotation += 5;
		aRenderer = arrow->getComponent<TextureRenderer>();
		//printf("La posición de la flecha es %f, %f\n", arrow->transform.position.x, arrow->transform.position.y);
		printf("El centro de rotación de la flecha es %f, %f\n\n", (arrow->getComponent<TextureRenderer>()->center->x + arrow->transform.position.x), (arrow->getComponent<TextureRenderer>()->center->y + arrow->transform.position.y));
		//printf("La posición del arco es %f, %f\n", bow->transform.position.x, bow->transform.position.y);
		printf("El centro de rotación del arco es %f, %f\n\n", (bow->transform.position.x + 8), (bow->transform.position.y + 15));
		break;
	case SDLK_DOWN:
		bow->transform.zRotation -= 5;
		arrow->transform.zRotation -= 5;
		break;
	default:
		break;
	}

	for (auto gameObjectPair : gameObjectMap)
		gameObjectPair.second->handleEvent(event);
}