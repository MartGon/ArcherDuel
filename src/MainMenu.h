#pragma once
#include "Scene.h"

#include "Button.h"
#include "TextLabel.h"
#include "TextInput.h"

#include "Player.h"

class MainMenu : public Scene
{
public:

	// Overrided methods
		// Scene
	void loadMedia() override;
	void onUpdate() override;
	void OnHandleEvent(const SDL_Event& event) override;

	// UI
		// Buttons
	std::vector<Button*> widgets;
	Button* play_button = nullptr;
	Button* online_button = nullptr;
	Button* server_button = nullptr;
	Button* client_button = nullptr;
	Button* back_button = nullptr;
	Button* exit_button = nullptr;

		// TextInput
	TextInput* ip_input = nullptr;

		// Game title
	TextLabel* game_title_1 = nullptr;
	TextLabel* game_title_2 = nullptr;
	GameObject* game_title_bow = nullptr;

	// GameObjects
	Player* player = nullptr;

	// Dimensions
	// Level Dimensions
	static const int LEVEL_WIDTH;
	static const int LEVEL_HEIGHT;

	// Button methods
	void enableLayer(Uint8 layer);
	void exitGame();
	void loadLevelOne(SceneMode mode = SceneMode::SINGLE_PLAYER);
	void playButtonHandler();
	void onlineButtonHandler();
	void serverButtonHandler();
	void clientButtonHandler();
	void backButtonHandler();
};