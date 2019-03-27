#include "LevelOne.h"
#include "Arrow.h"
#include "Player.h"
#include "Tower.h"
#include "GrassBlock.h"
#include "HealthBar.h"
#include "Button.h"
#include "AudioManager.h"
#include "Random.h"
#include "Timer.h"

// Original 720 * 480

// TODO - Escalar todo a x2 para evitar problemas con arco, arquero
const int LevelOne::LEVEL_WIDTH = 480;
const int LevelOne::LEVEL_HEIGHT = 320;

void LevelOne::loadMedia()
{
	// Setting managers
	setManager(new CollisionManager());
	setManager(new RendererManager());
	AudioManager* aManager = new AudioManager();
	aManager->enable();
	setManager(aManager);

	// Background
	GameObject *background = new GameObject();
	background->setComponent(new TextureRenderer("ShittyBackground.png", nullptr, 1));
	background->transform.scale = Vector2<float>(2, 2);

	// Player
	player = new Player();
	Vector2<float> player_pos(64, LEVEL_HEIGHT - 143);
	player->transform.position = player_pos;
	player->level = this;
	players.push_back(player);

	// Tower
	Tower* tower = new Tower();
	Vector2<float> tower_pos(0, LEVEL_HEIGHT - 160 - 32);
	tower->transform.position = tower_pos;

	// Player2
	player2 = new PlayerAI();
	Vector2<float> player2_pos(LEVEL_WIDTH - 64, LEVEL_HEIGHT - 111 - 32);
	player2->transform.position = player2_pos;
	player2->level = this;
	player2->setBoundaries(Vector2<float>(368, 196), Vector2<float>(462, 196));
	player2->enemy = player;
	player2->player_number = Player::PlayerNumber::PLAYER_TWO;
	players.push_back(player2);
	//player2->isActive = false;

	// Tower
	Tower* tower2= new Tower(Tower::ROOF_COLOR_BLUE);
	Vector2<float> tower2_pos(LEVEL_WIDTH - 128, LEVEL_HEIGHT - 160 - 32);
	tower2->transform.position = tower2_pos;

	// Grass blocks
	placeFloorBlocks();

	// Renderer Manager setup
	RendererManager::setCameraPosition(Vector2<int>(0, 0), Vector2<int>(LEVEL_WIDTH, LEVEL_HEIGHT));

	// Button
	Button* button = new Button(Texture("ExitButton.png", RendererManager::renderer));
	button->setScale(Vector2<float>(0.25f, 0.25f));
	button->transform.position = Vector2<float>(5, 5);
	button->setOnClickListener(std::bind(&LevelOne::exitGame, this));

	// Music Player
	GameObject* music = new GameObject();
	AudioPlayer* aPlayer = music->setComponent(new AudioPlayer("adwsl.wav"));
	aPlayer->volume = SDL_MIX_MAXVOLUME / 4;
	aPlayer->loop = true;
	aPlayer->play();
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
	// Check for players position
	for (auto player : players)
	{
		if (player)
		{
			if (!isPlayerPosValid(player))
			{
				resetPlayerPosition(player);
				player->isActive = false;
				Timer* timer = new Timer(3 * 1000, this, player);
			}
		}
	}
}

void LevelOne::handleEvent(const SDL_Event& event)
{
	TextureRenderer *aRenderer = nullptr;
	if (event.type == SDL_KEYDOWN)
	{
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
			isPaused = false;
			break;
		case SDLK_4:
			free_camera = true;
			isPaused = true;
			break;
		case SDLK_m:
			printMousePos();
			if (player)
			{
				player->stun(240);
				player->knockback(Vector2<float>(1, 0), 4);
			}
			if (player2)
				player2->stun(120);
			break;
		default:
			break;
		}
	}

	// TODO - Set this code to Scene
	for (auto go : gameObjectMap)
		go.second->handleEvent(event);
}

// Timer handler

void LevelOne::onTimerFinish(void* param)
{
	std::cout << "On timer finish called\n";
	Player* player = static_cast<Player*>(param);
	
	player->isActive = true;
	player->isInmunne = true;
	player->tRenderer->setBlink(6, 60 * 3);
}

void LevelOne::moveCamera(int xOffset, int yOffset)
{
	Vector2<int> cam_pos = RendererManager::getCameraPosition();

	cam_pos.x += xOffset;
	cam_pos.y += yOffset;

	RendererManager::setCameraPosition(cam_pos, Vector2<int>(LEVEL_WIDTH, LEVEL_HEIGHT));
}

void LevelOne::printMousePos()
{
	Vector2<int> mouse_pos;

	SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);

	auto scaler = RendererManager::getScaler();
	mouse_pos = (Vector2<float>)mouse_pos / scaler;

	std::cout << "Mouse pos is (" << mouse_pos.x << ", " << mouse_pos.y << ") " << std::endl;
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
        if (player == this->player2)
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
        if (player2->bow->arrow)
            return;

        player2->bow->loadArrow();
        break;
    default:
        break;
    }
}

// Game

bool LevelOne::isPlayerPosValid(Player* player)
{
	if (!player)
		return false;

	if (!player->isActive)
		return true;

	BoxCollider* col = player->bCollider;
	bool isInside = col->cRight > 0 && col->cLeft < LEVEL_WIDTH && col->cBottom > 0 && col->cTop < LEVEL_HEIGHT;

	return isInside;
}

void LevelOne::resetPlayerPosition(Player* player)
{
	Player::PlayerNumber pn = player->player_number;

	switch (pn)
	{
	case Player::PlayerNumber::PLAYER_ONE:
		player->transform.position = Vector2<float>(64, LEVEL_HEIGHT - 143);
		break;
	case Player::PlayerNumber::PLAYER_TWO:
		player->transform.position = Vector2<float>(LEVEL_WIDTH - 64, LEVEL_HEIGHT - 111 - 32);
		break;
	default:
		break;
	}

	// TODO - Needed to avoid despawning the player again
	player->bCollider->calculateColliderBoundaries();
}

// GUI

void LevelOne::exitGame()
{
	std::cout << "Exit game" << std::endl;
}