#pragma once
#include "Scene.h"
#include "Bow.h"
#include "PlayerAI.h"

class Player;
class LevelOne : public Scene
{
public:
    enum PlayerTurn
    {
        PLAYER_ONE_TURN,
        PLAYER_TWO_TURN
    };

	void loadMedia() override;

	void onClickBow();

	void onUpdate() override;

	void handleEvent(const SDL_Event& event) override;

    // GameObjects
	Bow* bow = nullptr;
	Player* player = nullptr;
    Bow* p2_bow = nullptr;
    PlayerAI* player2 = nullptr;

	// Level Dimensions
	static const int LEVEL_WIDTH;
	static const int LEVEL_HEIGHT;

	// Object level Placement
	void placeFloorBlocks();

    // GameState
    PlayerTurn turn = PLAYER_ONE_TURN;
    bool canPlayerAct(Player* player);
    void finishTurn();

	// GUI
	void exitGame();

	// Test
	void moveCamera(int xOffset, int yOffset);
	void printMousePos();
    int cam_speed = 5;
    bool free_camera = true;
};