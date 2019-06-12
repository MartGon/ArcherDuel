#pragma once
#include "Scene.h"
#include "Bow.h"
#include "PlayerAI.h"
#include "TimerHandler.h"


class Player;
class LevelOne : public Scene
{
public:
    enum PlayerTurn
    {
        PLAYER_ONE_TURN,
        PLAYER_TWO_TURN
    };

	// Constructor
	LevelOne(SceneMode mode = SceneMode::SINGLE_PLAYER);

	// Overrided methods
		// Scene
	void loadMedia() override;
	void onUpdate() override;
	GameObject* createGameObjectByTemplateId(int template_id) override;
		// Timer
	std::unordered_map<Uint32, TimerObject*> player_timers;
	TimerObject* spawn_pu_timer = nullptr;
	void onPlayerTimerFinish(Uint8 flag);
	void onEndGameTimerFininsh(Uint8 flag);
	void onSpawnPowerUpTimerFinish(Uint8 flag);

    // GameObjects
	Bow* bow = nullptr;
	Player* player = nullptr;
    Bow* p2_bow = nullptr;
    Player* player2 = nullptr;

	std::vector<Player*> players;

	// Debug
	TextLabel* framerate_display = nullptr;
	Uint32 previous_ticks = 0;

	// Level Dimensions
	static const int LEVEL_WIDTH;
	static const int LEVEL_HEIGHT;

	// Object level Placement
	static void placeFloorBlocks();

    // GameState
    PlayerTurn turn = PLAYER_ONE_TURN;

	// Game
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