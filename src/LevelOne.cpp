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

#include <iomanip>


// Original 720 * 480

// TODO - Escalar todo a x2 para evitar problemas con arco, arquero
const int LevelOne::LEVEL_WIDTH = 480;
const int LevelOne::LEVEL_HEIGHT = 270;

LevelOne::LevelOne(SceneMode mode, Uint32 player_amount)
{
	this->player_amount = player_amount;
	this->mode = mode;
}

void LevelOne::loadMedia()
{
	// Setting managers
	setManager(new CollisionManager());
	AudioManager* aManager = new AudioManager();
	aManager->enable();
	setManager(aManager);
	setManager(new RendererManager());

	// Background
	GameObject *background = new GameObject();
	background->setComponent(new TextureRenderer("ShittyBackground.png", nullptr, 1));
	background->transform.scale = Vector2<float>(2, 2);

	// Red Tower
	tower = new Tower(this);
	Vector2<float> tower_pos(0, LEVEL_HEIGHT - 160 - 31);
	tower->transform.position = tower_pos;
	int blue_players = player_amount / 2;
	tower->max_health = blue_players * 500;
	tower->health = tower->max_health;

	// Blue Tower
	tower2 = new Tower(this, Tower::ROOF_COLOR_BLUE);
	Vector2<float> tower2_pos(LEVEL_WIDTH - 128, LEVEL_HEIGHT - 160 - 31);
	tower2->transform.position = tower2_pos;
	int red_players = player_amount / 2 + player_amount % 2;
	tower2->max_health = red_players * 500;
	tower2->health = tower2->max_health;

	// Grass blocks
	placeFloorBlocks();

	// Create players
	for (int i = 0; i < player_amount; i++)
	{
		bool isAI = isOnline() ? false : i != 0;
		createPlayer(static_cast<PlayerNumber>(i + 1), isAI);
	}

	// Set references
	for(int i = 0; i < players.size(); i++)
	{
		Player* player = players[i];
		if (PlayerAI* playerAI = dynamic_cast<PlayerAI*>(player))
		{
			int index = i + 1 == players.size() ? i - 1 : i + 1;
			playerAI->enemy = players.at(index);
			playerAI->enemy_tower = playerAI->enemy->tower;
		}
	}

	// Renderer Manager setup
	RendererManager::setCameraPosition(Vector2<int>(0, 0), Vector2<int>(LEVEL_WIDTH, LEVEL_HEIGHT));

	// Exit Button
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

	// Create PowerUpObject timers
	spawn_pu_timer = new TimerObject(5 * 1000, 0);
	spawn_pu_timer->callback = std::bind(&LevelOne::onSpawnPowerUpTimerFinish, this, std::placeholders::_1);

	// Create gamestart counter
	start_counter = new GameStartCounter(3, this);
	start_counter->transform.position = Vector2<float>(LEVEL_WIDTH / 2, LEVEL_HEIGHT / 2);
	start_counter->time_display->setCenteredWithinParent({0.f, -100.f});
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
		block->transform.position = Vector2<float>(i * block_width, LEVEL_HEIGHT - block_texture.mHeight + 1);
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
		//std::cout << "FPS: " << frame_rate << "\n";
	}
}

void LevelOne::handleConnectionEstablished()
{
	// Create players when connection is handled
	//int player_amount = networkAgent->player_amount;
	//for (int i = 0; i < player_amount; i++)
	//	createPlayer(static_cast<PlayerNumber>(i + 1));
}


// Timer handler

void LevelOne::onPlayerTimerFinish(Uint32 flag)
{
	if (Player* player = dynamic_cast<Player*>(getGameObjectById(flag))) 
	{
		player->isActive = true;
		player->isInmunne = true;
		player->tRenderer->setBlink(6, 60 * 3);
	}
}

void LevelOne::onEndGameTimerFininsh(Uint32 flag)
{
	exitGame();
}

void LevelOne::onSpawnPowerUpTimerFinish(Uint32 flag)
{
	// Spawn power up Object
	PowerUpType type = (PowerUpType)Random::getRandomUniformInteger(1, 6);
	PowerUpObject* power_up_object = new PowerUpObject(type);
	power_up_object->setScale({ 1.5f, 1.5f });

	// Choose a random location
	Vector2<float> spawn_pos;
	spawn_pos.y = Random::getRandomUniformInteger(20, 75);
	spawn_pos.x = Random::getRandomUniformInteger(20, LEVEL_WIDTH - 20 - 22);

	// Set position
	power_up_object->transform.position = spawn_pos;

	int players = player_amount ? player_amount : 1;
	float mod = 2.f / (float)players;

	// Reset Timer
	spawn_pu_timer->timer->delay = Random::getRandomUniformFloat((4.f * mod) + 2, (10.f * mod) + 2) * 1000;
	spawn_pu_timer->timer->reset();
}

// Game

MapRGB LevelOne::getColorMod(PlayerNumber player_number)
{
	MapRGB color(0, 0, 0);

	switch (player_number)
	{
	case PlayerNumber::PLAYER_TWO:
		color.blue = 255;
		break;
	case PlayerNumber::PLAYER_THREE:
		color.red = 255;
		color.green = 255;
		break;
	case PlayerNumber::PLAYER_FOUR:
		color.green = 255;
		break;
	default:
		color.red = 255;
		break;
	}

	return color;
}

Vector2<float> LevelOne::getCannonBarPos(PlayerNumber player_number)
{
	Vector2<float> pos;
	int number = static_cast<int>(player_number);

	// It's odd
	if (number & 1)
	{
		auto def_pos = tower->healthBar->getAbsolutePosition() + Vector2<float>{0, -15};
		float offset_x = 27.5f * (number - 1);
		pos = { def_pos.x + offset_x, def_pos.y };
	}
	else
	{
		auto def_pos = tower2->healthBar->getAbsolutePosition() + Vector2<float>{0, -15};
		float offset_x = 27.5f * (number - 2);
		pos = { def_pos.x + offset_x, def_pos.y };
	}
	
	return pos;
}

Vector2<float> LevelOne::getCanonnBarPosByPlayer(Player* player)
{
	Vector2<float> pos;
	int number = static_cast<int>(player->player_number);

	// It's odd
	if (number & 1)
	{
		auto def_pos = player->tower->healthBar->getAbsolutePosition() + Vector2<float>{0, -15};
		float offset_x = 27.5f * (number - 1);
		pos = { def_pos.x + offset_x, def_pos.y };
	}
	else
	{
		auto def_pos = player->tower->healthBar->getAbsolutePosition() + Vector2<float>{0, -15};
		float offset_x = 27.5f * (number - 2);
		pos = { def_pos.x + offset_x, def_pos.y };
	}

	return pos;
}

NetworkOwner LevelOne::getNetworkOwner(PlayerNumber player_number)
{
	int number = static_cast<int>(player_number) - 1;
	NetworkOwner owner = static_cast<NetworkOwner>(number);

	return owner;
}

Vector2<float> LevelOne::getPlayerPos(PlayerNumber player_number)
{
	Vector2<float> player_pos = { 0, 0 };
	int number = static_cast<int>(player_number) % 4;

	// It's odd
	if (number & 1)
	{
		float offset_x = (number - 1) * 10;
		player_pos = { 44 + offset_x, LEVEL_HEIGHT - 143 };
	}
	else
	{
		float offset_x = (number - 2) * 10;
		player_pos = { LEVEL_WIDTH - 84 + offset_x, LEVEL_HEIGHT - 111 - 32 };
	}

	return player_pos;
}

Player* LevelOne::createPlayer(PlayerNumber player_number, bool isAI) 
{
	Player* player = nullptr;

	// Create player
	if (isAI)
		player = new PlayerAI(player_number);
	else
		player = new Player(player_number);

	// Set pos and level ref
	player->transform.position = getPlayerPos(player_number);
	player->level = this;

	// Set player team
	player->player_team = static_cast<int>(player_number) & 1 ? Player::PlayerTeam::RED_TEAM : Player::PlayerTeam::BLUE_TEAM;

	// Set player tower
	player->tower = static_cast<int>(player_number) & 1 ? tower : tower2;
	player->network_owner = getNetworkOwner(player_number);

	// Disable movement and bow at start
	player->mov_enabled = false;
	player->bow->isEnabled = false;

	// Setting AI extra data
	if (PlayerAI* playerAI = dynamic_cast<PlayerAI*>(player))
	{
		// Set boundaries
		BoxCollider* floor_collider = playerAI->tower->getComponent<BoxCollider>();
		int height = floor_collider->cHeight + floor_collider->offset.y + playerAI->tower->getAbsolutePosition().y;
		int lLimit = floor_collider->offset.x + playerAI->tower->getAbsolutePosition().x;
		int rLimit = floor_collider->cWidth + floor_collider->offset.x + playerAI->tower->getAbsolutePosition().x;

		playerAI->setBoundaries(Vector2<float>(lLimit, height), Vector2<float>(rLimit, height));

		// Set stopped until game starts
		playerAI->isStopped = true;
	}

	// Create player skill_bar
	StatusBar* skill_bar = new StatusBar();
	skill_bar->setScale({ 48, 1 });
	skill_bar->setHealthPercentage(0);
	skill_bar->tLabel->setText("Cannon");
	skill_bar->tLabel->setTextColor(getColorMod(player_number));
	skill_bar->transform.position = getCannonBarPos(player_number);
	player->skill_bar = skill_bar;

	// Create player timers
	TimerObject* player_timer = new TimerObject(3 * 1000, player->id);
	player_timer->timer->isOver = true;
	player_timer->callback = std::bind(&LevelOne::onPlayerTimerFinish, this, std::placeholders::_1);

	// Insert timer
	player_timers.insert({ player->id,  player_timer });

	// Add to player list
	players.push_back(player);

	return player;
}

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
	int number = (int)player->player_number % 4;
	PlayerNumber pn = (PlayerNumber)(number);

	switch (pn)
	{
	case PlayerNumber::PLAYER_ONE:
	case PlayerNumber::PLAYER_THREE:
		player->transform.position = Vector2<float>(64, LEVEL_HEIGHT - 143);
		break;
	case PlayerNumber::PLAYER_TWO:
	case PlayerNumber::PLAYER_FOUR:
	default:
		player->transform.position = Vector2<float>(LEVEL_WIDTH - 64, LEVEL_HEIGHT - 111 - 32);
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

// GameStartCounter

GameStartCounter::GameStartCounter(float seconds, LevelOne* level)
{
	// Set TimerComponent
	timer = setComponent(new TimerComponent(3 * 1000, 0));

	// Set textlabel
	time_display = new TextLabel();
	time_display->setText("3.00");
	time_display->setTextColor({ 255, 255, 255 });

		// Pos and scale
	time_display->transform.parent = &this->transform;
	time_display->setCenteredWithinParent();
	time_display->setTextScale({ 4.f, 4.f });

	// Set Level ref
	this->level = level;
}


// Overrided methods

void GameStartCounter::onUpdate()
{
	if (!timer->isOver)
	{
		// Set time_display text to buff duration
		Uint32 remaining_ms = timer->getTimeRemaining();
		double remaining_sec = remaining_ms / (double)1000;

		// Limit to two decimal values
		std::ostringstream stream;
		stream << std::setprecision(2) << std::fixed << remaining_sec;
		std::string num_text = stream.str();

		// Output
		time_display->setText(num_text);
	}
}


void GameStartCounter::onTimerEnd(Uint32 flag)
{
	std::vector<Player*> players = level->players;
	for (auto player : players)
	{
		player->mov_enabled = true;
		
		if (Bow* bow = player->bow)
		{
			bow->isEnabled = true;
		}

		if (PlayerAI* playerAI = dynamic_cast<PlayerAI*>(player))
		{
			playerAI->isStopped = false;
		}
	}

	// Set text
	time_display->setText("GO!");
	time_display->setCenteredWithinParent({2.f, -100.f});
	
	for (auto tRenderer : time_display->font_tRenderers)
	{
		tRenderer->vanish_rate = 2;
		tRenderer->isVanishing = true;
	}
}