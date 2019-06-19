#pragma once
#include "Scene.h"

#include "Button.h"
#include "TextLabel.h"
#include "TextInput.h"

#include "Player.h"

#include <optional>

enum MenuLayer
{
	MAIN_MENU_LAYER = 1,
	ONLINE_MENU_LAYER = 2,
	SERVER_MENU_LAYER = 4,
	CLIENT_MENU_LAYER = 8,
	PLAYER_MENU_LAYER = 16,
	CLIENT_CONNECTION_MENU_LAYER = 32,
	SERVER_CONNECTION_MENU_LAYER = 64,
	EVERY_LAYER = 255
};

enum ConnectionCodes
{
	FAILED_CONNECTION_TO_SERVER
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
	std::vector<GUIComponent*> widgets;
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
	TextLabel* connecting_label = nullptr;

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

	// Online Handling
	NetworkClient* network_client = nullptr;
	NetworkServer* network_server = nullptr;

	// Online timer
	TimerObject* timer = nullptr;
	Uint32 connection_tries = 0;
	Uint32 connection_tries_limit = 5;
	void handleTimer(Uint32 flag);


	SDL_mutex* flag_mutex = nullptr;
	SDL_Thread* connection_thread = nullptr;
	bool time_over = true;
	bool connection_attempt_finished = true;
	bool connection_attempt_succeeded = false;

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

	// Validation methods
	std::optional<std::pair<std::string, std::optional<int>>> getAddressIfValid(std::string address);
	
};

namespace MainMenuConnection 
{
	template <typename T>
	int handleConnection(T* data)
	{
		int result = 0;

		if (MainMenu* menu = static_cast<MainMenu*>(data))
		{
			result = menu->network_client->establishConnection();

			// We have finished
			SDL_LockMutex(menu->flag_mutex);
			menu->connection_attempt_finished = true;
			SDL_UnlockMutex(menu->flag_mutex);

		}

		return result;
	}
}
