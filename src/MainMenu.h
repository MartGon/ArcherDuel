#pragma once
#include "Scene.h"

#include "Button.h"
#include "TextLabel.h"
#include "TextInput.h"
#include "Checkbox.h"

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
	OPTIONS_MENU_LAYER = 128,
	EVERY_LAYER = 255
};

enum ConnectionCodes
{
	FAILED_CONNECTION_TO_SERVER,
	FAILED_WAITING_FOR_CLIENT
};

class MainMenu : public Scene
{
public:

	// Overrided methods
		// Scene
	void loadMedia() override;
	void onUpdate() override;
	void onDestroy() override;
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
	Button* options_button = nullptr;

		// TextLabel
	TextLabel* ip_label = nullptr;
	TextLabel* player_amount_label = nullptr;
	TextLabel* frame_amount_label = nullptr;
	TextLabel* connecting_label = nullptr;
	TextLabel* player_info_label = nullptr;
	TextLabel* player_color_info_label = nullptr;
	TextLabel* connection_action_label = nullptr;
	TextLabel* connected_players_label = nullptr;
	TextLabel* graphics_options_title = nullptr;
	TextLabel* sound_options_title = nullptr;
	TextLabel* fullscreen_label = nullptr;
	TextLabel* window_width_label = nullptr;
	TextLabel* window_height_label = nullptr;
	TextLabel* sound_label = nullptr;
	TextLabel* sound_volume_label = nullptr;

		// TextInput
	TextInput* ip_input = nullptr;
	TextInput* player_amount_input = nullptr;
	TextInput* frame_amount_input = nullptr;
	TextInput* window_width_input = nullptr;
	TextInput* window_height_input = nullptr;
	TextInput* sound_volume_input = nullptr;

		// Checkboxes
	CheckBox* fullscreen_checkbox = nullptr;
	CheckBox* sound_checkbox = nullptr;
	CheckBox* friendly_fire_checkbox = nullptr;
	CheckBox* shared_power_up_checkbox = nullptr;

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
	bool connection_established = false;
	bool disconnected = false;

	// Dimensions
	// Level Dimensions
	static const int LEVEL_WIDTH;
	static const int LEVEL_HEIGHT;

	// Button methods
	void enableLayer(Uint8 layer);
	void exitGame();
	void loadLevelOne(SceneMode mode = SceneMode::SINGLE_PLAYER, Uint32 player_amount = 2);
	void playButtonHandler();
	void onlineButtonHandler();
	void serverButtonHandler();
	void clientButtonHandler();
	void connectButtonHandler();
	void optionsButtonHandler();
	void backButtonHandler();

	// TextInput methods
	void onSelectIPInput();
	void setConnectedPlayers(Uint32 connected_players, Uint32 max_players);

	// Network agent handler
	void HandleServerAgentEvent(NetworkAgent::NetworkAgentEvent event);
	void HandleClientAgentEvent(NetworkAgent::NetworkAgentEvent event);
	
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
