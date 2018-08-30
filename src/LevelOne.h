#pragma once
#include "Scene.h"
#include "Bow.h"
#include "Player.h"

class LevelOne : public Scene
{
public:
	void loadMedia() override;

	void onClickBow();

	void onUpdate() override;

	void handleEvent(const SDL_Event& event) override;

	Bow* bow = nullptr;
	Player* player = nullptr;

	// Level Dimensions

	static const int LEVEL_WIDTH;
	static const int LEVEL_HEIGHT;

	// Test
	void moveCamera(int xOffset, int yOffset);
};