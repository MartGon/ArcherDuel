#include "MainMenu.h"
#include "MainGameLoop.h"
#include "TextureRenderer.h"
#include "RendererManager.h"
#include "Animator.h"
#include "Navigator.h"
#include "Button.h"
#include "Texture.h"
#include "Transform.h"
#include "Bow.h"

void MainMenu::loadMedia()
{
	Bow *bow = new Bow();
	Vector2<int> nativeRes = RendererManager::getNativeResolution();
	bow->setRelativePosition(Vector2<float>(nativeRes.x/2, nativeRes.y/2));

	MapRGB *colorKey = new MapRGB();
	colorKey->blue = 0xFF;

	GameObject *arrow = new GameObject();
	arrow->setComponent(new TextureRenderer("arrow.png", colorKey));
	arrow->setComponent(new Navigator());
	arrow->transform.position = Vector2<float>(bow->transform.position.x + 16, bow->transform.position.y + 14);

	bow->arrow = arrow;
}

void MainMenu::onClickBow()
{
	printf("I was clicked\n");
}