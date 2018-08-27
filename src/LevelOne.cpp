#include "LevelOne.h"
#include "MainGameLoop.h"
#include "TextureRenderer.h"
#include "RendererManager.h"
#include "Animator.h"
#include "Navigator.h"
#include "Button.h"
#include "Texture.h"
#include "Transform.h"
#include "RotatableBoxCollider.h"

const int LevelOne::LEVEL_WIDTH = 720;
const int LevelOne::LEVEL_HEIGHT = 480;

void LevelOne::loadMedia()
{
	// Background
	GameObject *background = new GameObject();
	background->setComponent(new TextureRenderer("BackgroundTest.png"));

	bow = new Bow();
	Vector2<int> nativeRes = RendererManager::getNativeResolution();
	bow->setRelativePosition(Vector2<float>(nativeRes.x / 2 , LEVEL_HEIGHT - nativeRes.y/2));

	MapRGB *colorKey = new MapRGB();
	colorKey->blue = 0xFF;

	arrow = new GameObject();
	TextureRenderer* aRenderer = arrow->setComponent(new TextureRenderer("ArrowTrim.png", colorKey));
	Navigator* nav = arrow->setComponent(new Navigator());
	//arrow->setComponent(new BoxCollider(aRenderer->texture));
	nav->isEnabled = false;
	// x + 6, y + 13
	arrow->transform.position = Vector2<float>(bow->transform.position.x + 6, bow->transform.position.y + 13);
    RotatableBoxCollider* col = arrow->setComponent(new RotatableBoxCollider(Vector2<int>(0, 0), Vector2<int>(0, 3), Vector2<int>(14, 0), Vector2<int>(14, 3)));

	// Rotation
	arrow->transform.rotationCenter = new Vector2<int>(2, 2);

	// Setting
	bow->arrow = arrow;
    

	// Renderer Manager setup
	RendererManager::setCameraPosition(Vector2<int>(0, 0), Vector2<int>(LEVEL_WIDTH, LEVEL_HEIGHT));

	// Test new rotatable Colliders
	test = new GameObject();
    test->setComponent(new TextureRenderer("ArrowTrim.png", colorKey));
    test->setRelativePosition(Vector2<int>(LEVEL_WIDTH / 2, LEVEL_HEIGHT / 2));
    test->setComponent(new BoxCollider(124, 24));
    bow->test = test;

	//col->rotate(Vector2<int>(0, 0), 180);
	std::cout << col->vertexValuesToStr();

    // Project test
    Vector2<int> a(1, 1);
    Vector2<int> b(1, 0);

    printf("El m�dulo del proyectado es %f", b.project(a));
}

void LevelOne::onClickBow()
{
	printf("I was clicked\n");
}

void LevelOne::onUpdate()
{
	BoxCollider* col = arrow->getComponent<BoxCollider>();
	//col->drawCollisionBoundaries(RendererManager::renderer);
	Vector2<int> res = RendererManager::getNativeResolution();
	Vector2<int> arrowPos = arrow->transform.position;

	// TODO - Change camera to consider center to be the actual camera position
	Vector2<int> cameraPos = Vector2<int>(arrowPos.x - res.x / 2, arrowPos.y - res.y / 2);
	RendererManager::setCameraPosition(cameraPos, Vector2<int>(LEVEL_WIDTH, LEVEL_HEIGHT));

	// Testing rotable colliders
	/*RotatableBoxCollider* rot = arrow->getComponent<RotatableBoxCollider>();
        if (rot->checkCollision(*test->getComponent<RotatableBoxCollider>()) || test->getComponent<RotatableBoxCollider>()->checkCollision(*rot))
            printf("Colisi�n");
			*/
    test->getComponent<BoxCollider>()->drawCollisionBoundaries();
	//rot->draw();
	//rot->rotate(*(arrow->transform.rotationCenter), arrow->transform.zRotation);
}

void LevelOne::handleEvent(const SDL_Event& event)
{
	TextureRenderer *aRenderer = nullptr;
	RotatableBoxCollider* rot = arrow->getComponent<RotatableBoxCollider>();
	rot->drawCollisionBoundaries();

	int cam_speed = 5;

	switch (event.key.keysym.sym)
	{
	case SDLK_UP:
		moveCamera(0, -cam_speed);
		rot->setRotation(Vector2<int>(-1, 1), cam_speed += 5);
		std::cout << rot->vertexValuesToStr();
		break;
	case SDLK_DOWN:
		moveCamera(0, cam_speed);
		rot->setRotation(Vector2<int>(-1, 1), cam_speed -= 5);
		std::cout << rot->vertexValuesToStr();
		break;
	case SDLK_RIGHT:
		moveCamera(cam_speed, 0);
		break;
	case SDLK_LEFT:
		moveCamera(-cam_speed, 0);
		break;
	case SDLK_PLUS:
		cam_speed++;
		break;
	case SDLK_MINUS:
		cam_speed--;
		break;
	default:
		break;
	}

	for (auto gameObjectPair : gameObjectMap)
		gameObjectPair.second->handleEvent(event);
}

void LevelOne::moveCamera(int xOffset, int yOffset)
{
	Vector2<int> cam_pos = RendererManager::getCameraPosition();

	cam_pos.x += xOffset;
	cam_pos.y += yOffset;

	RendererManager::setCameraPosition(cam_pos, Vector2<int>(LEVEL_WIDTH, LEVEL_HEIGHT));
}