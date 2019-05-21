#include "LevelOne.h"
#include "Arrow.h"
#include "Player.h"
#include "Tower.h"
#include "GrassBlock.h"
#include "HealthBar.h"
#include "Button.h"
#include "AudioManager.h"
#include "SceneManager.h"
#include "Random.h"
#include "Timer.h"
#include "MainMenu.h"
#include "InputManager.h"

// Original 720 * 480

// TODO - Escalar todo a x2 para evitar problemas con arco, arquero
const int LevelOne::LEVEL_WIDTH = 480;
const int LevelOne::LEVEL_HEIGHT = 320;

LevelOne::LevelOne(SceneMode mode)
{
	this->mode = mode;
}

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
	Tower* tower = new Tower(this);
	Vector2<float> tower_pos(0, LEVEL_HEIGHT - 160 - 32);
	tower->transform.position = tower_pos;

	// Player2
	if (isOnline())
		player2 = new Player();
	else
		player2 = new PlayerAI();

	Vector2<float> player2_pos(LEVEL_WIDTH - 64, LEVEL_HEIGHT - 111 - 32);
	player2->transform.position = player2_pos;
	player2->level = this;
	player2->player_number = Player::PlayerNumber::PLAYER_TWO;
	player2->player_team = Player::PlayerTeam::BLUE_TEAM;
	player2->network_owner = NetworkOwner::OWNER_CLIENT_1;
	players.push_back(player2);

	if (PlayerAI* playerAI = dynamic_cast<PlayerAI*>(player2))
	{
		playerAI->setBoundaries(Vector2<float>(368, 196), Vector2<float>(462, 196));
		playerAI->enemy = player;
	}
	//player2->isActive = false;

	// Tower
	Tower* tower2 = new Tower(this, Tower::ROOF_COLOR_BLUE);
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
	button->tLabel->isActive = false;

	// Winner banner
	winner_banner = new TextLabel();
	winner_banner->setTextScale(Vector2<float>(3, 3));
	winner_banner->setText("red team wins");
	winner_banner->transform.position = Vector2<float>(LEVEL_WIDTH / 2 - 2.5f * 3 * winner_banner->getText().size()
		, LEVEL_HEIGHT / 10 - 2.5f * winner_banner->transform.scale.y);
	winner_banner->isActive = false;

	// Music Player
	GameObject* music = new GameObject();
	AudioPlayer* aPlayer = music->setComponent(new AudioPlayer("adwsl.wav"));
	aPlayer->volume = SDL_MIX_MAXVOLUME / 4;
	aPlayer->loop = true;
	aPlayer->play();

	// Text Label
	TextLabel* label = new TextLabel();
	label->transform.position = Vector2<float>(LEVEL_WIDTH / 2, 10);
	if (mode == ONLINE_SERVER)
		label->setText("Server");
	else if (mode == ONLINE_CLIENT)
		label->setText("Client");
	else
		label->setText("Offline");

	// Framerate Display
	framerate_display = new TextLabel();
	label->transform.position = Vector2<float>(LEVEL_WIDTH - LEVEL_WIDTH / 10, 10);
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

	// Update framerate
	Uint32 frame_ticks = SDL_GetTicks();
	Uint32 diff = frame_ticks - previous_ticks;
	previous_ticks = frame_ticks;
	Uint32 frame_rate = (Uint32)(1.0f / (float)((float)diff / 1000.0f));
	framerate_display->setText(std::to_string(frame_rate));
}

void LevelOne::OnHandleEvent(const SDL_Event& event)
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
}

GameObject* LevelOne::createGameObjectByTemplateId(int template_id)
{
	GameObject* go = nullptr;
	switch (template_id)
	{
	case 1:
		if (mode == ONLINE_SERVER)
			go = player2->bow->loadArrow();
		else
			go = player->bow->loadArrow();
		break;
	case -1:
	default:
		break;
	}

	return go;
}

// Timer handler

void LevelOne::onTimerFinish(void* param)
{
	std::cout << "On timer finish called\n";
	if (Player* player = static_cast<Player*>(param)) 
	{

		player->isActive = true;
		player->isInmunne = true;
		player->tRenderer->setBlink(6, 60 * 3);
	}
	else
	{
		exitGame();
	}
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

void LevelOne::setWinnerTeam(Player::PlayerTeam winner_team)
{
	// Check flag
	if (isGameOver)
		return;

	// Set flag
	isGameOver = true;

	// Update winner banner
	std::string winner_str = winner_team == Player::PlayerTeam::RED_TEAM ? "red" : "blu";
	winner_banner->setText(winner_str + " team wins");
	winner_banner->isActive = true;

	// Stop players
	//for (auto player : players) player->isStopped = true;

	// Set timer for finishing
	Timer* timer = new Timer(10 * 1000, this, nullptr);
}

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

bool LevelOne::isObjectPositionValid(GameObject* go)
{
	if (!go)
		return false;

	if (!go->isActive)
		return true;

	const int left_limit = -100;
	const int right_limit = LEVEL_WIDTH + 100;
	const int down_limit = LEVEL_HEIGHT + 100;
	Vector2<float> pos = go->transform.position;
	bool isInside = pos.x > left_limit && pos.x < right_limit && pos.y < down_limit;

	return isInside;
}

// GUI

void LevelOne::exitGame()
{
	std::cout << "Se pulso el botonsito\n";
	SceneManager::loadNextScene(new MainMenu());
}