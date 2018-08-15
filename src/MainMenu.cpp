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

	MapRGB *colorKey = new MapRGB();
	colorKey->blue = 0xFF;

	GameObject *arrow = new GameObject();
	arrow->setComponent(new TextureRenderer("arrow.png", colorKey));
	arrow->setComponent(new Navigator());
	arrow->transform.position = Vector2<float>(bow->transform.position.x + 7, bow->transform.position.y);

	bow->arrow = arrow;
}

void MainMenu::onClickBow()
{
	printf("I was clicked\n");
}