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

	// Overrided methods
		// Scene
	void loadMedia() override;
	void onUpdate() override;
	void handleEvent(const SDL_Event& event) override;
		// Timer
	void onTimerFinish(void* param) override;

    // GameObjects
	Bow* bow = nullptr;
	Player* player = nullptr;
    Bow* p2_bow = nullptr;
    PlayerAI* player2 = nullptr;

	std::vector<Player*> players;

	// Level Dimensions
	static const int LEVEL_WIDTH;
	static const int LEVEL_HEIGHT;

	// Object level Placement
	void placeFloorBlocks();

    // GameState
    PlayerTurn turn = PLAYER_ONE_TURN;
    bool canPlayerAct(Player* player);
    void finishTurn();
	void onClickBow();

	// Game
	bool isPlayerPosValid(Player* player);
	void resetPlayerPosition(Player* player);

	static bool isObjectPositionValid(GameObject* go);

	// GUI
	void exitGame();

	// Test
	void moveCamera(int xOffset, int yOffset);
	void printMousePos();
    int cam_speed = 5;
    bool free_camera = true;
};