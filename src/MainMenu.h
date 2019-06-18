#pragma once
#include "Scene.h"

#include "Button.h"
#include "TextLabel.h"
#include "TextInput.h"

#include "Player.h"

enum MenuLayer
{
	MAIN_MENU_LAYER = 1,
	ONLINE_MENU_LAYER = 2,
	SERVER_MENU_LAYER = 4,
	CLIENT_MENU_LAYER = 8,
	PLAYER_MENU_LAYER = 16,
	EVERY_LAYER = 255

};

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
	Button* connect_button = nullptr;
	Button* back_button = nullptr;
	Button* exit_button = nullptr;

		// TextLabel
	TextLabel* ip_label = nullptr;
	TextLabel* player_amount_label = nullptr;
	TextLabel* frame_amount_label = nullptr;

		// TextInput
	TextInput* ip_input = nullptr;
	TextInput* player_amount_input = nullptr;
	TextInput* frame_amount_input = nullptr;

		// Game title
	TextLabel* game_title_1 = nullptr;
	TextLabel* game_title_2 = nullptr;
	GameObject* game_title_bow = nullptr;

	// GUI handling
	Uint8 current_layer = 1;

	// GameObjects
	Player* player = nullptr;

	// Level One attributes
	SceneMode next_mode = SINGLE_PLAYER;

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
	void connectButtonHandler();
	void backButtonHandler();
};