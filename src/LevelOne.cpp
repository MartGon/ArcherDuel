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
#include "PowerUp.h"
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
	AudioManager* aManager = new AudioManager();
	aManager->enable();
	setManager(aManager);
	
	// Renderer Manager
	setManager(new RendererManager());

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

	// Set player tower
	player->tower = tower;

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

	// Set player2 tower
	player2->tower = tower2;

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
	label->isActive = false;

	// Create player one skill bar
	StatusBar* skill_bar = new StatusBar();
	skill_bar->setScale({ 55, 1 });
	skill_bar->setHealthPercentage(0);
	skill_bar->tLabel->setText("Cannon");
	skill_bar->transform.position = tower->healthBar->getAbsolutePosition() + Vector2<float>{0, -15};
	player->skill_bar = skill_bar;

	// Create player two skill bar
	StatusBar* skill_bar2 = new StatusBar();
	skill_bar2->setScale({ 55, 1 });
	skill_bar2->setHealthPercentage(0);
	skill_bar2->tLabel->setText("Cannon");
	skill_bar2->transform.position = tower2->healthBar->getAbsolutePosition() + Vector2<float>{0, -15};
	player2->skill_bar = skill_bar2;

	// Create player timers
	TimerObject* player_timer = new TimerObject(3 * 1000, player->id);
	player_timer->timer->isOver = true;
	player_timer->callback = std::bind(&LevelOne::onPlayerTimerFinish, this, std::placeholders::_1);

	TimerObject* player_timer2 = new TimerObject(3 * 1000, player2->id);
	player_timer2->timer->isOver = true;
	player_timer2->callback = std::bind(&LevelOne::onPlayerTimerFinish, this, std::placeholders::_1);

	player_timers.insert({ player->id,  player_timer });
	player_timers.insert({ player2->id, player_timer2 });

	// Create PowerUpObject timers
	spawn_pu_timer = new TimerObject(5 * 1000, 0);
	spawn_pu_timer->callback = std::bind(&LevelOne::onSpawnPowerUpTimerFinish, this, std::placeholders::_1);
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

void LevelOne::onUpdate()
{
	// Check for players position
	for (auto player : players)
	{
		if (player)
		{
			if (!isPlayerPosValid(player))
			{
				// Reset player pos and deactivate
				resetPlayerPosition(player);
				player->jump_nav->speed = 1;
				player->isActive = false;
				
				// Remove player buffs
				while(!player->power_ups.empty())
				{
					PowerUp* power_up = player->power_ups.begin()->second;
					power_up->remove();
					player->removePowerUp(power_up);
				}

				// Activate timer
				player_timers.at(player->id)->timer->reset();
			}
		}
	}

	// Update framerate
	Uint32 frame_ticks = SDL_GetTicks();
	Uint32 diff = frame_ticks - previous_ticks;
	previous_ticks = frame_ticks;
	Uint32 frame_rate = (Uint32)(1.0f / (float)((float)diff / 1000.0f));

	if (isOnline())
	{
		framerate_display->setText(std::to_string(calc_frame_count));
		//std::cout << "Frame is: " << last_packet->frame_count << "\n";
	}
	else
	{
		framerate_display->setText(std::to_string(frame_rate));
		std::cout << "FPS: " << frame_rate << "\n";
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

void LevelOne::onPlayerTimerFinish(Uint8 flag)
{
	if (Player* player = dynamic_cast<Player*>(getGameObjectById(flag))) 
	{

		player->isActive = true;
		player->isInmunne = true;
		player->tRenderer->setBlink(6, 60 * 3);
	}
	
}

void LevelOne::onEndGameTimerFininsh(Uint8 flag)
{
	exitGame();
}

void LevelOne::onSpawnPowerUpTimerFinish(Uint8 flag)
{
	// Spawn power up Object
	PowerUpType type = (PowerUpType)Random::getRandomUniformInteger(1, 6);
	PowerUpObject* power_up_object = new PowerUpObject(type);
	power_up_object->setScale({ 1.5f, 1.5f });

	// Choose a random location
	Vector2<float> spawn_pos;
	spawn_pos.y = Random::getRandomUniformInteger(20, 100);
	spawn_pos.x = Random::getRandomUniformInteger(20, LEVEL_WIDTH - 20 - 22);

	// Set position
	power_up_object->transform.position = spawn_pos;

	// Reset Timer
	spawn_pu_timer->timer->delay = Random::getRandomUniformInteger(6, 12) * 1000;
	spawn_pu_timer->timer->reset();
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
	TimerObject* timer = new TimerObject(10 * 1000, 0);
	timer->callback = std::bind(&LevelOne::onEndGameTimerFininsh, this, 0);
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

	const int left_limit = -15;
	const int right_limit = LEVEL_WIDTH + 15;
	const int down_limit = LEVEL_HEIGHT + 15;
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