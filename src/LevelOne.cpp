#include "LevelOne.h"
#include "Arrow.h"
#include "Player.h"
#include "Tower.h"
#include "GrassBlock.h"

// Original 720 * 480

const int LevelOne::LEVEL_WIDTH = 480;
const int LevelOne::LEVEL_HEIGHT = 320;

void LevelOne::loadMedia()
{
	// Setting managers
	setManager(new RendererManager());
	setManager(new CollisionManager());

	// Background
	GameObject *background = new GameObject();
	background->setComponent(new TextureRenderer("ShittyBackground.png", nullptr, 1));
	background->transform.scale = Vector2<float>(2, 2);

	// Player
	Player* player = new Player();
	Vector2<float> player_pos(64, LEVEL_HEIGHT - 111 - 32);
	player->transform.position = player_pos;

	// Tower
	Tower* tower = new Tower();
	Vector2<float> tower_pos(0, LEVEL_HEIGHT - 160 - 32);
	tower->transform.position = tower_pos;

	// Player2
	Player* player2 = new Player();
	Vector2<float> player2_pos(LEVEL_WIDTH - 64, LEVEL_HEIGHT - 111 - 32);
	player2->transform.position = player2_pos;

	// Tower
	Tower* tower2= new Tower(Tower::ROOF_COLOR_BLUE);
	Vector2<float> tower2_pos(LEVEL_WIDTH - 128, LEVEL_HEIGHT - 160 - 32);
	tower2->transform.position = tower2_pos;

	// Grass blocks
	placeFloorBlocks();

	// Renderer Manager setup
	RendererManager::setCameraPosition(Vector2<int>(0, 0), Vector2<int>(LEVEL_WIDTH, LEVEL_HEIGHT));
}

void LevelOne::placeFloorBlocks()
{
	Texture block_texture("HDGrass.png", RendererManager::renderer);
	int block_height = block_texture.mHeight;
	int block_width = block_texture.mWidth;

	int block_amount = LEVEL_WIDTH / block_width;
	for (int i = 0; i <= block_amount; i++)
	{
		GrassBlock* block = new GrassBlock(i & 1);
		block->transform.position = Vector2<float>(i * block_width, LEVEL_HEIGHT - block_texture.mHeight);
	}
}

void LevelOne::onClickBow()
{

}

void LevelOne::onUpdate()
{
    // Camera measures
	// TODO - Change camera to consider center to be the actual camera position
    if (!free_camera)
    {
        Vector2<int> res = RendererManager::getNativeResolution();
        Vector2<int> arrowPos;

        if (turn == PLAYER_ONE_TURN)
            arrowPos = bow->arrow->transform.position;
        else
            arrowPos = p2_bow->arrow->transform.position;
        
        Vector2<float> cameraPos = Vector2<int>(arrowPos.x - res.x / 2, arrowPos.y - res.y / 2);
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

// Gamestate 

bool LevelOne::canPlayerAct(Player* player)
{
    switch (turn)
    {
    case PLAYER_ONE_TURN:
        if (player == this->player)
            return true;
        break;
    case PLAYER_TWO_TURN:
        if (player == this->playerTwo)
            return true;
        break;
    default:
        break;
    }

    return false;
}

void LevelOne::finishTurn()
{
    turn = (PlayerTurn)((turn + 1) % 2);

    switch (turn)
    {
    case PLAYER_ONE_TURN:
        if (player->bow->arrow)
            return;

        player->bow->loadArrow();
        break;
    case PLAYER_TWO_TURN:
        if (playerTwo->bow->arrow)
            return;

        playerTwo->bow->loadArrow();
        break;
    default:
        break;
    }
}