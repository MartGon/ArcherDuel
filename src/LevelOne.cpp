#include "LevelOne.h"
#include "Arrow.h"

const int LevelOne::LEVEL_WIDTH = 720;
const int LevelOne::LEVEL_HEIGHT = 480;

void LevelOne::loadMedia()
{
	// Setting managers
	setManager(new RendererManager());
	setManager(new CollisionManager());

	// Background
	GameObject *background = new GameObject();
	background->setComponent(new TextureRenderer("BackgroundTest.png", nullptr, 1));

	// Bow
	bow = new Bow();
	Vector2<int> nativeRes = RendererManager::getNativeResolution();
	bow->setRelativePosition(Vector2<float>(nativeRes.x / 2 , LEVEL_HEIGHT - nativeRes.y/2));
    bow->transform.rotationCenter = new Vector2<int>(-20, 10);

	MapRGB *colorKey = new MapRGB();
	colorKey->blue = 0xFF;

	// Arrow
	bow->arrow = new Arrow();
	bow->arrow->transform.position = bow->getArrowInitialPosition();
    bow->arrow->setAbsoluteRotationCenter(bow->getAbsoluteRotationCenter());

    printf("Bow center %s\n", (*(bow->transform.rotationCenter) + (Vector2<int>)bow->transform.position).toStr().c_str());
    printf("Arrow center %s\n", (*(bow->arrow->transform.rotationCenter) + (Vector2<int>)bow->arrow->transform.position).toStr().c_str());

	// Player
	player = new Player();
    player->bCollider->debug = true;
    player->transform.scale = Vector2<float>(0.65f, 0.75f);
	Vector2<float> offset(-25, 0);
	player->setRelativePosition(Vector2<float>(nativeRes.x / 2, LEVEL_HEIGHT - nativeRes.y / 2) + offset);
	
	// PlayerTwo
	playerTwo = new Player();
    playerTwo->transform.scale = Vector2<float>(0.65f, 0.75f);
	playerTwo->setRelativePosition(Vector2<float>(LEVEL_WIDTH - nativeRes.x / 2, LEVEL_HEIGHT - nativeRes.y / 2));
	playerTwo->getComponent<TextureRenderer>()->flip = SDL_FLIP_HORIZONTAL;

    // Bow Two
    p2_bow = new Bow();
    p2_bow->setRelativePosition(Vector2<float>(LEVEL_WIDTH - nativeRes.x / 2, LEVEL_HEIGHT - nativeRes.y / 2) - offset);
    p2_bow->transform.rotationCenter = new Vector2<int>(-20, 10);
    p2_bow->transform.zRotation = 180;
    //p2_bow->getComponent<TextureRenderer>()->flip = SDL_FLIP_HORIZONTAL;

    // Arrow two
    p2_bow->arrow = new Arrow();
    p2_bow->arrow->transform.position = p2_bow->getArrowInitialPosition();
    p2_bow->arrow->setAbsoluteRotationCenter(p2_bow->getAbsoluteRotationCenter());

    printf("Bow center %s\n", (*(p2_bow->transform.rotationCenter) + (Vector2<int>)p2_bow->transform.position).toStr().c_str());
    printf("Arrow center %s\n", (*(p2_bow->arrow->transform.rotationCenter) + (Vector2<int>)p2_bow->arrow->transform.position).toStr().c_str());

	// Renderer Manager setup
	RendererManager::setCameraPosition(Vector2<int>(0, 0), Vector2<int>(LEVEL_WIDTH, LEVEL_HEIGHT));
}

void LevelOne::onClickBow()
{

}

void LevelOne::onUpdate()
{
	Vector2<int> res = RendererManager::getNativeResolution();
	Vector2<int> arrowPos = bow->arrow->transform.position;

	// TODO - Change camera to consider center to be the actual camera position
    if (!free_camera)
    {
        Vector2<int> cameraPos = Vector2<int>(arrowPos.x - res.x / 2, arrowPos.y - res.y / 2);
        RendererManager::setCameraPosition(cameraPos, Vector2<int>(LEVEL_WIDTH, LEVEL_HEIGHT));
    }
}

void LevelOne::handleEvent(const SDL_Event& event)
{
	TextureRenderer *aRenderer = nullptr;

	switch (event.key.keysym.sym)
	{
	case SDLK_UP:
		moveCamera(0, -cam_speed);
		break;
	case SDLK_DOWN:
		moveCamera(0, cam_speed);
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
    case SDLK_3:
        free_camera = false;
        break;
    case SDLK_4:
        free_camera = true;
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