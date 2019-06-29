#pragma once
#include "Scene.h"
#include "Bow.h"
#include "PlayerAI.h"
#include "TimerHandler.h"

class Player;
class GameStartCounter;

class LevelOne : public Scene
{
public:
	// Constructor
	LevelOne(SceneMode mode = SceneMode::SINGLE_PLAYER, Uint32 player_amount = 2);

	// Overrided methods
		// Scene
	void loadMedia() override;
	void onUpdate() override;
	void handleConnectionEstablished() override;
		// Timer
	std::unordered_map<Uint32, TimerObject*> player_timers;
	TimerObject* spawn_pu_timer = nullptr;
	void onPlayerTimerFinish(Uint32 flag);
	void onEndGameTimerFininsh(Uint32 flag);
	void onSpawnPowerUpTimerFinish(Uint32 flag);

    // GameObjects
	Uint32 player_amount = 2;
	Tower* tower = nullptr;
	Tower* tower2 = nullptr;
	std::vector<Player*> players;

	// Debug
	TextLabel* framerate_display = nullptr;
	Uint32 previous_ticks = 0;

	// Settings
	bool shared_powerups = false;
	bool friendly_fire = false;

	// Level Dimensions
	static const int LEVEL_WIDTH;
	static const int LEVEL_HEIGHT;

	// Object level Placement
	static void placeFloorBlocks();

	// Game
	static MapRGB getColorMod(PlayerNumber number);
	static bool isPlayerPosValid(Player* player);
	static void resetPlayerPosition(Player* player);
	static bool isObjectPositionValid(GameObject* go);
	static Vector2<float> getCanonnBarPosByPlayer(Player* player);

	Vector2<float> getCannonBarPos(PlayerNumber number);
	NetworkOwner getNetworkOwner(PlayerNumber number);
	Vector2<float> getPlayerPos(PlayerNumber number);
	Player* createPlayer(PlayerNumber player_number, bool isAI = false);
	void setWinnerTeam(Player::PlayerTeam winner_team);

	// GUI
	GameStartCounter* start_counter = nullptr;
	TextLabel* winner_banner = nullptr;
	void exitGame();

	// Callbacks
	void handleNaEvent(NetworkAgent::NetworkAgentEvent e);

private:
	bool isGameOver = false;
};

class GameStartCounter : public GameObject
{
public:
	// Constructor
	GameStartCounter(float seconds, LevelOne* level);

	// Refs
	LevelOne* level = nullptr;

	// Members
	TextLabel* time_display = nullptr;

	// Components
	TimerComponent* timer = nullptr;

	// Overrided methods
	void onUpdate();
	void onTimerEnd(Uint32 flag);
};