#pragma once
#include "Scene.h"
#include "Bow.h"
#include "PlayerAI.h"
#include "TimerHandler.h"


class Player;
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
	Vector2<float> getCannonBarPos(PlayerNumber number);
	NetworkOwner getNetworkOwner(PlayerNumber number);
	Vector2<float> getPlayerPos(PlayerNumber number);
	Player* createPlayer(PlayerNumber player_number, bool isAI = false);
	void setWinnerTeam(Player::PlayerTeam winner_team);
	bool isPlayerPosValid(Player* player);
	void resetPlayerPosition(Player* player);

	static bool isObjectPositionValid(GameObject* go);

	// GUI
	TextLabel* winner_banner = nullptr;
	void exitGame();

private:
	bool isGameOver = false;
};