#pragma once
#include "Scene.h"
#include "Bow.h"
#include "PlayerAI.h"
#include "TimerHandler.h"


class Player;
class LevelOne : public Scene, public TimerHandler
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
	void handleEvent(const SDL_Event& event) override;
	GameObject* createGameObjectByTemplateId(int template_id) override;
		// Timer
	void onTimerFinish(void* param) override;

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
    bool canPlayerAct(Player* player);
    void finishTurn();
	void onClickBow();

	// Game
	void setWinnerTeam(Player::PlayerTeam winner_team);
	bool isPlayerPosValid(Player* player);
	void resetPlayerPosition(Player* player);

	static bool isObjectPositionValid(GameObject* go);

	// GUI
	TextLabel* winner_banner = nullptr;
	void exitGame();

	// Test
	void moveCamera(int xOffset, int yOffset);
	void printMousePos();
    int cam_speed = 5;
    bool free_camera = true;

private:
	bool isGameOver = false;
};