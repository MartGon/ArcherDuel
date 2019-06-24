#include "MainMenu.h"
#include "LevelOne.h"

#include "AudioManager.h"
#include "RendererManager.h"
#include "SceneManager.h"
#include "Random.h"

#include "Tower.h"
#include "HealthBar.h"
#include "Cannon.h"
#include "PowerUp.h"

#include <regex>
#include <fstream>

// Const values
const int MainMenu::LEVEL_WIDTH = 480;
const int MainMenu::LEVEL_HEIGHT = 270;

// Overrided methods

void MainMenu::loadMedia()
{
	// Setting managers
	setManager(new CollisionManager());
	setManager(new RendererManager());
	AudioManager* aManager = new AudioManager();
	aManager->enable();
	setManager(aManager);

	// Background
	GameObject *background = new GameObject();
	background->setComponent(new TextureRenderer("ShittyBackground.png", nullptr, 1));
	background->transform.scale = Vector2<float>(2, 2);

	// GameObjects

	// Tower
	Tower* tower = new Tower();
	Vector2<float> tower_pos(0, LEVEL_HEIGHT - 160 - 31);
	tower->transform.position = tower_pos;
	tower->healthBar->isActive = false;

	// Grass Blocks
	LevelOne::placeFloorBlocks();

	// UI

	// Game title 1
	game_title_1 = new TextLabel();
	game_title_1->setTextScale(Vector2<float>(4, 4));

	game_title_1->setText("Archers");
	game_title_1->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 5.5f, 3.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	game_title_1->setLayer(EVERY_LAYER);

	// Game title 2
	game_title_2 = new TextLabel();
	game_title_2->transform.parent = &game_title_1->transform;
	game_title_2->setTextScale(Vector2<float>(4, 4));

	game_title_2->setText("Duel");
	game_title_2->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 11.5f, 11.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	game_title_2->setLayer(EVERY_LAYER);

	// Play Button
	play_button = new Button(Texture("button_background4.png"));
	play_button->setScale(Vector2<float>(2, 2));
	play_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.f, 20.f }, {ALIGN_FROM_RIGHT, ALIGN_FROM_TOP});
	play_button->tLabel->setText("Play");
	play_button->tLabel->setTextScale(Vector2<float>(2.f, 2.f));
	play_button->tLabel->setCenteredWithinParent();
	play_button->setOnClickListener(std::bind(&MainMenu::playButtonHandler, this));
	play_button->setLayer(MAIN_MENU_LAYER | PLAYER_MENU_LAYER);

	// Online button
	online_button = new Button(Texture("button_background4.png"));
	online_button->setScale(Vector2<float>(2, 2));
	online_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.f, 32.5f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	online_button->tLabel->setText("Online");
	online_button->tLabel->setTextScale(Vector2<float>(2.f, 2.f));
	online_button->tLabel->setCenteredWithinParent();
	online_button->setOnClickListener(std::bind(&MainMenu::onlineButtonHandler, this));
	online_button->setLayer(MAIN_MENU_LAYER);

	// Online Sub-menu
		// Server button
	server_button = new Button(Texture("button_background4.png"));
	server_button->setScale(Vector2<float>(2, 2));
	server_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.f, 20.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	server_button->tLabel->setText("Server");
	server_button->tLabel->setTextScale(Vector2<float>(2.f, 2.f));
	server_button->tLabel->setCenteredWithinParent();
	server_button->setOnClickListener(std::bind(&MainMenu::serverButtonHandler, this));
	server_button->isActive = false;
	server_button->setLayer(ONLINE_MENU_LAYER);

			// Server Address / Server Port
	ip_input = new TextInput("button_background4.png", "127.0.0.1:1338");
	ip_input->setScale(Vector2<float>(2.5f, 1));
	ip_input->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 7.5f, 24.5f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	ip_input->tLabel->setTextScale(Vector2<float>(1.f, 1.f));
	ip_input->tLabel->setCenteredWithinParent();
	ip_input->isActive = false;
	ip_input->onSelect = std::bind(&MainMenu::onSelectIPInput, this);
	ip_input->valid_inputs = { R"(\d|\.|:|[a-z])" };
	ip_input->setLayer(CLIENT_MENU_LAYER | SERVER_MENU_LAYER);
		
			// Server address label
	ip_label = new TextLabel();
	ip_label->setScale(Vector2<float>(1, 1));
	ip_label->setText("Server Address");
	ip_label->transform.parent = &ip_input->transform;
	ip_label->setCenteredWithinParent({0, -15.f});
	ip_label->setTextColor({ 0, 0, 0 });

			// Connect button
	connect_button = new Button(Texture("button_background4.png"));
	connect_button->setScale(Vector2<float>(2, 2));
	connect_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.f, 32.5f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	connect_button->tLabel->setText("Connect");
	connect_button->tLabel->setTextScale(Vector2<float>(2.f, 2.f));
	connect_button->tLabel->setCenteredWithinParent();
	connect_button->setOnClickListener(std::bind(&MainMenu::connectButtonHandler, this));
	connect_button->isActive = false;
	connect_button->setLayer(CLIENT_MENU_LAYER | SERVER_MENU_LAYER);

		// Player amount input
	player_amount_input = new TextInput("button_background4.png", "4");
	player_amount_input->setScale(Vector2<float>(2.5f, 1));
	player_amount_input->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 7.5f, 35.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	player_amount_input->tLabel->setTextScale(Vector2<float>(1.f, 1.f));
	player_amount_input->tLabel->setCenteredWithinParent();
	player_amount_input->isActive = false;
	player_amount_input->valid_inputs = { R"(\d)" };
	player_amount_input->setLayer(SERVER_MENU_LAYER | PLAYER_MENU_LAYER);

		// Player amount label
	player_amount_label = new TextLabel();
	player_amount_label->setScale(Vector2<float>(1, 1));
	player_amount_label->setText("Player Amount");
	player_amount_label->transform.parent = &player_amount_input->transform;
	player_amount_label->setCenteredWithinParent({ 0, -15.f });
	player_amount_label->setTextColor({ 0, 0, 0 });

		// Friendly fire check box
	friendly_fire_checkbox = new CheckBox(Texture("button_background.png"));
	friendly_fire_checkbox->transform.parent = &player_amount_input->transform;
	friendly_fire_checkbox->setCenteredWithinParent({ 30, 20 });
	friendly_fire_checkbox->isActive = false;
	friendly_fire_checkbox->tLabel->isActive = true;
	friendly_fire_checkbox->tLabel->setText("Friendly Fire");
	friendly_fire_checkbox->tLabel->setTextColor({ 0,0,0 });
	friendly_fire_checkbox->tLabel->setCenteredWithinParent({ -45, 0 });
	friendly_fire_checkbox->setLayer(SERVER_MENU_LAYER | PLAYER_MENU_LAYER);

	// Friendly fire check box
	shared_power_up_checkbox = new CheckBox(Texture("button_background.png"));
	shared_power_up_checkbox->transform.parent = &player_amount_input->transform;
	shared_power_up_checkbox->setCenteredWithinParent({ 30, 40 });
	shared_power_up_checkbox->isActive = false;
	shared_power_up_checkbox->tLabel->isActive = false;
	shared_power_up_checkbox->tLabel->setText("Team PowerUps");
	shared_power_up_checkbox->tLabel->setTextColor({ 0,0,0 });
	shared_power_up_checkbox->tLabel->setCenteredWithinParent({ -45, 0 });
	shared_power_up_checkbox->setLayer(SERVER_MENU_LAYER | PLAYER_MENU_LAYER);

		// Frame amount input
	frame_amount_input = new TextInput("button_background4.png", "8");
	frame_amount_input->setScale(Vector2<float>(2.5f, 1));
	frame_amount_input->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 7.5f, 45.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	frame_amount_input->tLabel->setTextScale(Vector2<float>(1.f, 1.f));
	frame_amount_input->tLabel->setCenteredWithinParent();
	frame_amount_input->isActive = false;
	frame_amount_input->valid_inputs = { R"(\d)" };
	frame_amount_input->setLayer(SERVER_MENU_LAYER);

		// Frmae amount lable
	frame_amount_label = new TextLabel();
	frame_amount_label->setScale(Vector2<float>(1, 1));
	frame_amount_label->setText("Frame Buffer Size");
	frame_amount_label->transform.parent = &frame_amount_input->transform;
	frame_amount_label->setCenteredWithinParent({ 0, -15.f });
	frame_amount_label->setTextColor({ 0, 0, 0 });

		// Client button
	client_button = new Button(Texture("button_background4.png"));
	client_button->setScale(Vector2<float>(2, 2));
	client_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.f, 32.5f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	client_button->tLabel->setText("Client");
	client_button->tLabel->setTextScale(Vector2<float>(2.f, 2.f));
	client_button->tLabel->setCenteredWithinParent();
	client_button->setOnClickListener(std::bind(&MainMenu::clientButtonHandler, this));
	client_button->isActive = false;
	client_button->setLayer(ONLINE_MENU_LAYER);

		// Connection label
	connecting_label = new TextLabel();
	connecting_label->setScale(Vector2<float>(1, 1));
	connecting_label->setText("Connecting");
	connecting_label->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 11.25f, 24.5f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	connecting_label->setTextColor({ 0, 0, 0 });
	connecting_label->setLayer(CLIENT_CONNECTION_MENU_LAYER | SERVER_CONNECTION_MENU_LAYER);
	connecting_label->isActive = false;

		// Player info label
	player_info_label = new TextLabel();
	player_info_label->setScale(Vector2<float>(1, 1));
	player_info_label->setTextColor({ 0, 0, 0 });
	player_info_label->setText("You are player");
	player_info_label->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 12.5f, 30.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	player_info_label->isActive = false;

		// Player color report label
	player_color_info_label = new TextLabel();
	player_color_info_label->setScale({ 1, 1 });
	player_color_info_label->setTextColor({ 255, 0, 0 });
	player_color_info_label->setText("red");
	player_color_info_label->transform.parent = &player_info_label->transform;
	player_color_info_label->setCenteredWithinParent({ 0, 15.f });

	// Connected players label
	connected_players_label = new TextLabel();
	connected_players_label->setScale(Vector2<float>(1, 1));
	connected_players_label->setText("Connected players: 0/0");
	connected_players_label->setTextColor({ 0, 0, 0 });
	connected_players_label->transform.parent = &player_color_info_label->transform;
	connected_players_label->setCenteredWithinParent({ 0, 15.f });

		// Connection info label
	connection_action_label = new TextLabel();
	connection_action_label->setScale({ 1, 1 });
	connection_action_label->setText("Waiting for server");
	connection_action_label->transform.parent = &connected_players_label->transform;
	connection_action_label->setCenteredWithinParent({ 0, 15.f });
	connection_action_label->isActive = false;

		// Back button
	back_button = new Button(Texture("button_background4.png"));
	back_button->setScale(Vector2<float>(2, 2));
	back_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.f, 45.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	back_button->tLabel->setText("Back");
	back_button->tLabel->setTextScale(Vector2<float>(2.f, 2.f));
	back_button->tLabel->setCenteredWithinParent();
	back_button->setOnClickListener(std::bind(&MainMenu::backButtonHandler, this));
	back_button->isActive = false;
	back_button->setLayer(EVERY_LAYER - 1);

	// Exit button
	exit_button = new Button(Texture("ExitButton.png"));
	exit_button->setScale(Vector2<float>(0.25f, 0.25f));
	exit_button->transform.position = Vector2<float>(5, 5);
	exit_button->setOnClickListener(std::bind(&MainMenu::exitGame, this));
	exit_button->tLabel->isActive = false;
	exit_button->tLabel->setText("");
	exit_button->setLayer(EVERY_LAYER);

	// Options
	options_button = new Button(Texture("button_background4.png"));
	options_button->setScale(Vector2<float>(2, 2));
	options_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.f, 45.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	options_button->tLabel->setText("Settings");
	options_button->tLabel->setTextScale(Vector2<float>(2.f, 2.f));
	options_button->tLabel->setCenteredWithinParent();
	options_button->setOnClickListener(std::bind(&MainMenu::optionsButtonHandler, this));
	options_button->setLayer(MAIN_MENU_LAYER);

	// Graphics TextLabel
	graphics_options_title = new TextLabel();
	graphics_options_title->setScale(Vector2<float>(1.5f, 1.5f));
	graphics_options_title->setText("Graphics");
	graphics_options_title->setTextColor({ 0, 0, 0 });
	graphics_options_title->transform.parent = &game_title_2->transform;
	graphics_options_title->setCenteredWithinParent({ 0, 20.f });
	graphics_options_title->setLayer(OPTIONS_MENU_LAYER);
	graphics_options_title->isActive = false;

	// Fullscreen text label
	fullscreen_label = new TextLabel();
	fullscreen_label->setScale(Vector2<float>(1.f, 1.f));
	fullscreen_label->setText("Fullscreen");
	fullscreen_label->setTextColor({ 0, 0, 0 });
	fullscreen_label->transform.parent = &graphics_options_title->transform;
	fullscreen_label->setCenteredWithinParent({ -20, 20.f });
	fullscreen_label->setLayer(OPTIONS_MENU_LAYER);
	fullscreen_label->isActive = false;

	// Fullscreen check box
	fullscreen_checkbox = new CheckBox(Texture("button_background.png"));
	fullscreen_checkbox->transform.parent = &fullscreen_label->transform;
	fullscreen_checkbox->setCenteredWithinParent({40, 0});
	fullscreen_checkbox->setLayer(OPTIONS_MENU_LAYER);
	fullscreen_checkbox->isActive = false;
	fullscreen_checkbox->tLabel->isActive = false;
	fullscreen_checkbox->tLabel->setText("");

	// Window  width label
	window_width_label = new TextLabel();
	window_width_label->setScale(Vector2<float>(1.f, 1.f));
	window_width_label->setText("Window Width");
	window_width_label->setTextColor({ 0, 0, 0 });
	window_width_label->transform.parent = &fullscreen_label->transform;
	window_width_label->setCenteredWithinParent({ -20, 20 });
	window_width_label->setLayer(OPTIONS_MENU_LAYER);
	window_width_label->isActive = false;

	// Window size width
	window_width_input = new TextInput("button_background4.png", "960");
	window_width_input->setScale(Vector2<float>(1, 1));
	window_width_input->transform.parent = &window_width_label->transform;
	window_width_input->setCenteredWithinParent({ 0, 20 });
	window_width_input->tLabel->setTextScale(Vector2<float>(1.f, 1.f));
	window_width_input->tLabel->setCenteredWithinParent();
	window_width_input->isActive = false;
	window_width_input->valid_inputs = { R"(\d)" };
	window_width_input->setLayer(OPTIONS_MENU_LAYER);
	window_width_input->isActive = false;

	// Window height label
	window_height_label = new TextLabel();
	window_height_label->setScale(Vector2<float>(1.f, 1.f));
	window_height_label->setText("Window Height");
	window_height_label->setTextColor({ 0, 0, 0 });
	window_height_label->transform.parent = &fullscreen_label->transform;
	window_height_label->setCenteredWithinParent({ 60, 20 });
	window_height_label->setLayer(OPTIONS_MENU_LAYER);
	window_height_label->isActive = false;

	// Window size height
	window_height_input = new TextInput("button_background4.png", "520");
	window_height_input->setScale(Vector2<float>(1, 1));
	window_height_input->transform.parent = &window_height_label->transform;
	window_height_input->setCenteredWithinParent({ 0, 20 });
	window_height_input->tLabel->setTextScale(Vector2<float>(1.f, 1.f));
	window_height_input->tLabel->setCenteredWithinParent();
	window_height_input->isActive = false;
	window_height_input->valid_inputs = { R"(\d)" };
	window_height_input->setLayer(OPTIONS_MENU_LAYER);

	// Sound TextLabel title
	sound_options_title = new TextLabel();
	sound_options_title->setScale(Vector2<float>(1.5f, 1.5f));
	sound_options_title->setText("Sound");
	sound_options_title->setTextColor({ 0, 0, 0 });
	sound_options_title->transform.parent = &graphics_options_title->transform;
	sound_options_title->setCenteredWithinParent({ 0, 80.f });
	sound_options_title->setLayer(OPTIONS_MENU_LAYER);
	sound_options_title->isActive = false;

	// Sound text label
	sound_label = new TextLabel();
	sound_label->setScale(Vector2<float>(1.f, 1.f));
	sound_label->setText("Sound");
	sound_label->setTextColor({ 0, 0, 0 });
	sound_label->transform.parent = &sound_options_title->transform;
	sound_label->setCenteredWithinParent({ -34, 20.f });
	sound_label->setLayer(OPTIONS_MENU_LAYER);
	sound_label->isActive = false;

	// Sound checkbox
	sound_checkbox = new CheckBox(Texture("button_background.png"));
	sound_checkbox->transform.parent = &fullscreen_checkbox->transform;
	sound_checkbox->setCenteredWithinParent({ 0, 80 });
	sound_checkbox->setLayer(OPTIONS_MENU_LAYER);
	sound_checkbox->isActive = false;
	sound_checkbox->select();
	sound_checkbox->tLabel->isActive = false;
	sound_checkbox->tLabel->setText("");

	// Sound volume label
	sound_volume_label = new TextLabel();
	sound_volume_label->setScale(Vector2<float>(1.f, 1.f));
	sound_volume_label->setText("Volume");
	sound_volume_label->setTextColor({ 0, 0, 0 });
	sound_volume_label->transform.parent = &sound_options_title->transform;
	sound_volume_label->setCenteredWithinParent({ 0, 40.f });
	sound_volume_label->setLayer(OPTIONS_MENU_LAYER);
	sound_volume_label->isActive = false;

	// Sound volume input
	sound_volume_input = new TextInput("button_background4.png", "100");
	sound_volume_input->setScale(Vector2<float>(1, 1));
	sound_volume_input->transform.parent = &sound_volume_label->transform;
	sound_volume_input->setCenteredWithinParent({ 0, 20 });
	sound_volume_input->tLabel->setTextScale(Vector2<float>(1.f, 1.f));
	sound_volume_input->tLabel->setCenteredWithinParent();
	sound_volume_input->isActive = false;
	sound_volume_input->valid_inputs = { R"(\d)" };
	sound_volume_input->setLayer(OPTIONS_MENU_LAYER);

	// Player
	player = new Player(PlayerNumber::PLAYER_ONE);
	player->tower = tower;
	player->mov_enabled = false;
	Vector2<float> player_pos(64, LEVEL_HEIGHT - 143);
	player->transform.position = player_pos;

	// Renderer Manager setup
	RendererManager::setCameraPosition(Vector2<int>(0, 0), Vector2<int>(LEVEL_WIDTH, LEVEL_HEIGHT));

	// Create timer object
	timer = new TimerObject(1 * 1000, 1);
	timer->timer->isOver = true;
	timer->callback = std::bind(&MainMenu::handleTimer, this, std::placeholders::_1);

	// Init random
	std::random_device rd;
	Random::dre = std::mt19937(rd());
}

void MainMenu::onUpdate()
{
	// Fill vector
	if(widgets.empty())
		widgets = getGameObjects<GUIComponent*>();

	if (current_layer == CLIENT_CONNECTION_MENU_LAYER)
	{
		if (!disconnected)
		{
			if (network_client)
			{
				if (!connection_established)
				{
					// We have to try again
					SDL_LockMutex(flag_mutex);
					bool can_attempt_connection = connection_attempt_finished;
					SDL_UnlockMutex(flag_mutex);

					// Check if can try to connect now
					if (can_attempt_connection)
					{
						// Check if timer is over
						if (time_over)
						{
							// If it exists, wait to prev thread and store result
							if (connection_thread)
							{
								// Wait and free thread
								int result = 0;
								SDL_WaitThread(connection_thread, &result);
								connection_thread = nullptr;

								// Store result
								connection_established = result;
							}

							// Check if previous attempt was successful
							if (connection_established)
							{
								// Enable next layer or set text with connection info
								// Update connecting message
								std::string connect_msg = "Connected!";
								connecting_label->setTextColor({ 0, 255, 0 });
								connecting_label->setText(connect_msg);

								// Activate player info label
								player_info_label->isActive = true;

								// Activate player color info label
								PlayerNumber player_number = static_cast<PlayerNumber>((network_client->pair_identity % 4) + 1);

								// Get color text
								std::string player_color = player->getPathByPlayerNumber(player_number).empty() ? "_green" : player->getPathByPlayerNumber(player_number);
								if (!player_color.empty())
									player_color.erase(player_color.begin());
								player_color_info_label->setText(player_color);

								// Get color mod
								MapRGB mod = LevelOne::getColorMod(player_number);
								player_color_info_label->setTextColor(mod);
								player_color_info_label->setCenteredWithinParent({ 0, 15.f });

								player_color_info_label->isActive = true;

								// Enable connection action label
								connection_action_label->setText("Waiting for server");
								connection_action_label->isActive = true;
								connection_action_label->setTextColor({ 0, 0 , 0 });
								connection_action_label->setCenteredWithinParent({ 0, 15.f });
							}
							else
							{
								// Set flag to false
								SDL_LockMutex(flag_mutex);
								connection_attempt_finished = false;
								SDL_UnlockMutex(flag_mutex);

								// Create thread to connect to server
								connection_thread = SDL_CreateThread(MainMenuConnection::handleConnection, "connection", this);

								// Increment connection tries
								connection_tries++;

								// Set timer flag to false
								time_over = false;

								// Init timer to try again
								timer->timer->reset();
								timer->setFlag(FAILED_CONNECTION_TO_SERVER);
							}
						}
					}
				}
				else
				{
					// Wait for game start packet
					if (Packet* packet = network_client->recvPacket())
					{
						if (packet->packetType == DATA_PACKET)
						{
							DataPacket<GameConfig>* config_pkt = static_cast<DataPacket<GameConfig>*>(packet);

							auto data = config_pkt->data;

							// Enable players connected label
							int connected_players = data.connected_players;
							int max_players = data.max_players;
							connected_players_label->isActive = true;
							setConnectedPlayers(connected_players, max_players);

							// Set game config
							friendly_fire = data.friendly_fire;
							team_powerups = data.friendly_fire;

							// Enable game config checkboxes
							friendly_fire_checkbox->transform.parent = &connection_action_label->transform;
							shared_power_up_checkbox->transform.parent = &friendly_fire_checkbox->transform;
							friendly_fire_checkbox->setCenteredWithinParent({ 40, 20 });
							shared_power_up_checkbox->setCenteredWithinParent({ 0, 20 });
							friendly_fire_checkbox->isSelectable = false;
							shared_power_up_checkbox->isSelectable = false;

							friendly_fire_checkbox->isActive = true;
							friendly_fire_checkbox->tLabel->isActive = true;
							shared_power_up_checkbox->isActive = true;
							shared_power_up_checkbox->tLabel->isActive = true;

							friendly_fire_checkbox->setSelected(friendly_fire);
							shared_power_up_checkbox->setSelected(team_powerups);
						}

						if (packet->packetType == GAME_START_PACKET)
							playButtonHandler();
					}
				}
			}
		}
	}
	else if (current_layer == SERVER_CONNECTION_MENU_LAYER)
	{
		if (network_server)
		{
			if (time_over)
			{
				bool finished = network_server->establishConnection();

				if (finished)
				{
					// Change connecting label
					std::string game_ready_str = "Game Ready!";
					connecting_label->setTextColor({ 0, 255, 0 });
					connecting_label->setText(game_ready_str);
					connecting_label->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 13.f, 24.5f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });

					// Enable start game button
					play_button->isActive = true;
					play_button->tLabel->isActive = true;
					play_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.f, 70.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
				}
				else
				{
					// Increment connection tries
					connection_tries++;

					// Init timer to try again
					timer->timer->reset();
					timer->setFlag(FAILED_WAITING_FOR_CLIENT);

					// Set timer flag to false
					time_over = false;
				}

				// Check for pair disconnect
				auto packets = network_server->recvPackets();
			}
		}
	}
}

void MainMenu::onDestroy()
{
	if (flag_mutex)
	{
		SDL_DestroyMutex(flag_mutex);
		flag_mutex = nullptr;
	}
}

void MainMenu::OnHandleEvent(const SDL_Event& event)
{
	if (event.type == SDL_MOUSEBUTTONUP)
	{
		for (auto go_p : gameObjectMap)
		{
			GameObject* go = go_p.second;
			if (TextInput* tInput = dynamic_cast<TextInput*>(go))
			{
				tInput->setSelected(false);
			}
		}
	}
}

// Timer

void MainMenu::handleTimer(Uint32 flag)
{
	time_over = true;

	if (flag == FAILED_CONNECTION_TO_SERVER)
	{
		if (!disconnected) 
		{
			// Update connecting message
			std::string connect_msg = "Connecting";

			// Get amount of dots to set
			int dots = connection_tries % 4;
			for (int i = 0; i < dots; i++)
				connect_msg += '.';

			connecting_label->setText(connect_msg);
		}
	}
	else if (flag == FAILED_WAITING_FOR_CLIENT)
	{
		// Update connecting message
		std::string connect_msg = "Waiting for players";

		// Get amount of dots to set
		int dots = connection_tries % 4;
		for (int i = 0; i < dots; i++)
			connect_msg += '.';

		connecting_label->setText(connect_msg);
	}

}

// Own methods

void MainMenu::enableLayer(Uint8 layer)
{
	for (auto widget : widgets)
	{
		if (widget->getLayer() & layer)
			widget->isActive = true;
		else
			widget->isActive = false;
	}

	current_layer = layer;

	// Disable graphics options layer
	if(current_layer != OPTIONS_MENU_LAYER)
		graphics_options_title->isActive = false;
}

void MainMenu::exitGame()
{
	SceneManager::quitGame();
}

void MainMenu::loadLevelOne(SceneMode mode, Uint32 player_amount)
{
	LevelOne* level_one = new LevelOne(mode, player_amount);
	level_one->setSceneMode(mode);

	// Set config
	level_one->friendly_fire = friendly_fire;
	level_one->shared_powerups = team_powerups;

	NetworkAgent* nAgent = nullptr;
	if (mode == ONLINE_CLIENT)
	{
		network_client->HandleNAEvent = std::function<void(NetworkAgent::NetworkAgentEvent)>();
		nAgent = network_client;
	}
	else if (mode == ONLINE_SERVER)
	{
		network_server->HandleNAEvent = std::function<void(NetworkAgent::NetworkAgentEvent)>();
		nAgent = network_server;

		// Send start packet
		GameStartPacket gsp;
		nAgent->sendPacket(&gsp, false);
	}

	level_one->networkAgent = nAgent;
	SceneManager::loadNextScene(level_one);
}

void MainMenu::playButtonHandler()
{
	switch(current_layer)
	{
	case MAIN_MENU_LAYER:
		// Set next mode
		next_mode = SINGLE_PLAYER;

		// Move player amount input 
		player_amount_input->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 7.5f, 24.5f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
		friendly_fire_checkbox->setCenteredWithinParent({ 30, 20 });
		shared_power_up_checkbox->setCenteredWithinParent({ 30, 40 });
		play_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.f, 50.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
		back_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.f, 63.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });

		enableLayer(PLAYER_MENU_LAYER);
		break;
	case PLAYER_MENU_LAYER:
	{
		Uint32 player_amount = std::stoi(player_amount_input->getText());

		// Set game config values
		friendly_fire = friendly_fire_checkbox->isSelected();
		team_powerups = shared_power_up_checkbox->isSelected();

		loadLevelOne(SINGLE_PLAYER, player_amount);
		break;
	}
	case CLIENT_CONNECTION_MENU_LAYER:
		loadLevelOne(ONLINE_CLIENT, network_client->player_amount);
		break;
	case SERVER_CONNECTION_MENU_LAYER:

		// Set game config values
		friendly_fire = friendly_fire_checkbox->isSelected();
		team_powerups = shared_power_up_checkbox->isSelected();

		loadLevelOne(ONLINE_SERVER, network_server->player_amount);
		break;
	}
}

void MainMenu::onlineButtonHandler()
{
	enableLayer(ONLINE_MENU_LAYER);
}

void MainMenu::serverButtonHandler()
{
	// Read server config
	readServerConfig();

	// Set next mode
	next_mode = ONLINE_SERVER;

	// Set text
	ip_label->setText("Server port");
	ip_label->setTextColor({ 0, 0, 0 });
	ip_label->setCenteredWithinParent({ 0, -15.f });

	// Set config data
	ip_input->setText(server_port);
	frame_amount_input->setText(server_frame_buffer);
	player_amount_input->setText(server_player_amount);
	friendly_fire_checkbox->setSelected(server_friendly_fire);
	shared_power_up_checkbox->setSelected(server_team_powerups);

	// Move connect and back buttons
	player_amount_input->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 7.5f, 35.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	connect_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.f, 70.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	back_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.f, 82.5f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	friendly_fire_checkbox->setCenteredWithinParent({ 30, 50 });
	shared_power_up_checkbox->setCenteredWithinParent({ 30, 70 });

	enableLayer(SERVER_MENU_LAYER);
}

void MainMenu::clientButtonHandler() 
{
	// Read Config file
	readClientConfig();

	// Set next mode
	next_mode = ONLINE_CLIENT;

	// Set text
	ip_label->setText("Server Address");
	ip_label->setTextColor({ 0, 0, 0 });
	ip_input->setText(client_ip + ":" + client_port);
	ip_label->setCenteredWithinParent({ 0, -15.f });

	// Move connect and back buttons
	connect_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.f, 70.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	back_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.f, 82.5f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });

	enableLayer(CLIENT_MENU_LAYER);
}

void MainMenu::connectButtonHandler()
{
	// Check if address is valid
	if (current_layer == CLIENT_MENU_LAYER)
	{
		std::string input_address = ip_input->getText();
		if (auto optional = getAddressIfValid(input_address))
		{
			auto[ip, port_opt] = optional.value();
			auto port = port_opt.value_or(1338);

			// Create client agent to be used for connection
			if(!network_client)
				network_client = new NetworkClient();
			network_client->state = NetworkClient::CLIENT_OPENING_SOCKET;
			network_client->serverIP = ip;
			network_client->serverPort = port;
			network_client->isBlocking = false;
			network_client->HandleNAEvent = std::bind(&MainMenu::HandleClientAgentEvent, this, std::placeholders::_1);

			// Enable connection layer
			enableLayer(CLIENT_CONNECTION_MENU_LAYER);

			// Create mutex
			if(!flag_mutex)
				flag_mutex = SDL_CreateMutex();

			// Connecting label
			connecting_label->setText("Connecting...");
			connecting_label->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 11.25f, 24.5f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
			connecting_label->setTextColor({ 0, 0, 0 });

			// Reset flag
			disconnected = false;

			// Save config
			client_ip = ip;
			client_port = std::to_string(port);

			saveClientConfig();
		}
		else
		{
			// Set color red, input is not valid
			ip_input->tLabel->setTextColor({ 255, 0, 0 });
		}
	}
	else if (current_layer == SERVER_MENU_LAYER)
	{
		std::string str_port = ip_input->getText();
		std::regex is_number{ R"(\d+)" };

		if (std::regex_match(str_port, is_number))
		{
			if (auto port = std::stoi(str_port); port < 65536)
			{
				auto frame_amount = std::stoi(frame_amount_input->getText());
				auto player_amount = std::stoi(player_amount_input->getText());

				// Create client agent to be used for connection
				if(!network_server)
					network_server = new NetworkServer();
				network_server->state = NetworkServer::SERVER_STATE_OPENING_SOCKET;
				network_server->serverPort = port;
				network_server->max_buffer_size = frame_amount;
				network_server->player_amount = player_amount;
				network_server->socket_set = SDLNet_AllocSocketSet(network_server->player_amount);
				network_server->isBlocking = false;
				network_server->HandleNAEvent = std::bind(&MainMenu::HandleServerAgentEvent, this, std::placeholders::_1);

				// Enable server connection layer
				enableLayer(SERVER_CONNECTION_MENU_LAYER);

				// Enabling connecting label
				connecting_label->setText("Waiting for players");
				connecting_label->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.0f, 24.5f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
				connecting_label->setTextColor({ 0, 0, 0 });

				// Reset player color label
				player_color_info_label->setText("red");
				player_color_info_label->setTextColor({ 255, 0, 0 });

				// Enabling labels
				player_info_label->isActive = true;
				player_color_info_label->isActive = true;
				connected_players_label->isActive = true;
				setConnectedPlayers(1, network_server->player_amount);

				// Set timer flag
				timer->setFlag(FAILED_WAITING_FOR_CLIENT);

				// Save config
				server_port = str_port;
				server_player_amount = player_amount_input->getText();
				server_frame_buffer = frame_amount_input->getText();
				server_friendly_fire = friendly_fire_checkbox->isSelected();
				server_team_powerups = shared_power_up_checkbox->isSelected();

				saveServerConfig();

				return;
			}

		}	
		// Set color red, input is not valid
		ip_input->tLabel->setTextColor({ 255, 0, 0 });
	}
}

void MainMenu::optionsButtonHandler()
{
	enableLayer(OPTIONS_MENU_LAYER);

	back_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.f, 82.5f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });

	// Read Window data

	// Set fullscreen
	fullscreen_checkbox->setSelected(SDL_GetWindowFlags(RendererManager::window) & SDL_WINDOW_FULLSCREEN);

	// Get current window size
	int w = 0;
	int h = 0;
	SDL_GetWindowSize(RendererManager::window, &w, &h);

	window_width_input->setText(std::to_string(w));
	window_height_input->setText(std::to_string(h));

	// Sound
	fullscreen_checkbox->setSelected(AudioManager::sound_enabled);

	int volume = AudioManager::volume;
	sound_volume_input->setText(std::to_string(volume));
}

void MainMenu::backButtonHandler()
{
	// Reset pos
	setDefaultLayout();

	switch (current_layer)
	{
	case CLIENT_CONNECTION_MENU_LAYER:
		if (network_client)
		{
			// Destroy net agent
			delete network_client;
			network_client = nullptr;

			// Disable flag
			connection_established = false;
		}
		break;
	case SERVER_CONNECTION_MENU_LAYER:
		if (network_server)
		{
			delete network_server;
			network_server = nullptr;
		}
		break;
	case OPTIONS_MENU_LAYER:
		{
			// Get winodw size
			int w = std::stoi(window_width_input->getText());
			int h = std::stoi(window_height_input->getText());

			// Set fullscren
			bool fullscreen = fullscreen_checkbox->isSelected();
			if (fullscreen)
			{
				if (!(SDL_GetWindowFlags(RendererManager::window) & SDL_WINDOW_FULLSCREEN))
				{
					// Get size of display
					SDL_DisplayMode dm;
					SDL_GetCurrentDisplayMode(0, &dm);
					w = dm.w;
					h = dm.h;

					// Set window size
					SDL_SetWindowSize(RendererManager::window, w, h);

					// Set fullscreen
					SDL_SetWindowFullscreen(RendererManager::window, SDL_WINDOW_FULLSCREEN);
				}
			}
			// Set window mode
			else
			{
				// Get old size
				int prev_w = 0;
				int prev_h = 0;

				SDL_GetWindowSize(RendererManager::window, &prev_w, &prev_h);

				// Change only if was different before
				if (prev_w != w || prev_h != h)
					SDL_SetWindowSize(RendererManager::window, w, h);

				// Get size of display
				SDL_DisplayMode dm;
				SDL_GetCurrentDisplayMode(0, &dm);

				// Set borderless if same size as display
				if (dm.w == w && dm.h == h)
				{
					SDL_SetWindowFullscreen(RendererManager::window, SDL_WINDOW_BORDERLESS);
					SDL_SetWindowPosition(RendererManager::window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
				}
				else
				{
					// Set to window
					SDL_SetWindowFullscreen(RendererManager::window, 0);
					SDL_SetWindowPosition(RendererManager::window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
				}
			}

			// Get new size
			SDL_GetWindowSize(RendererManager::window, &w, &h);
			WINDOW_HEIGHT = h;
			WINDOW_WIDTH = w;

			// Set audio enabled
			bool sound_enabled = sound_checkbox->isSelected();
			AudioManager::sound_enabled = sound_enabled;

			// Set volume
			int volume = std::stoi(sound_volume_input->getText());
			volume = volume <= 100 ? volume : 100;
			AudioManager::volume = volume;

			// Save config file
			saveGeneralConfig();

			break;
		}
	}

	// Enable main menu
	enableLayer(MAIN_MENU_LAYER);
}

// TextInput methods

void MainMenu::onSelectIPInput()
{
	ip_input->tLabel->setTextColor({ 255, 255, 255 });
}

void MainMenu::setConnectedPlayers(Uint32 connected_players, Uint32 max_players)
{
	std::string default_str = "Connected players: ";
	std::string player_balance = std::to_string(connected_players) + "/" + std::to_string(max_players);

	connected_players_label->setText(default_str + player_balance);
}

// Network agent handle

void MainMenu::HandleServerAgentEvent(NetworkAgent::NetworkAgentEvent event)
{
	switch (event)
	{
	case NetworkAgent::EVENT_PAIR_CONNECTED:
	{
		// Increase player connected amount label
		Uint32 max_players = network_server->player_amount;
		Uint32 connected_players = network_server->client_sockets.size() + 1;

		setConnectedPlayers(connected_players, max_players);

		// Send Game data
		GameConfig config{ friendly_fire_checkbox->isSelected(), shared_power_up_checkbox->isSelected(), connected_players , max_players};
		DataPacket data{ config };
		network_server->sendPacket(&data, false);
	}
		break;
	case NetworkAgent::EVENT_PAIR_DISCONNECTED:
	{
		// Decrease player connected amount label
		Uint32 max_players = network_server->player_amount;
		Uint32 connected_players = network_server->client_sockets.size();

		setConnectedPlayers(connected_players, max_players);

		// Info other clients
		GameConfig config{ friendly_fire_checkbox->isSelected(), shared_power_up_checkbox->isSelected(), connected_players , max_players };
		DataPacket data{ config };
		network_server->sendPacket(&data, false);

		// If game was ready to start, prevent the game from starting
		if (play_button->isActive)
		{
			play_button->isActive = false;
			play_button->tLabel->isActive = false;
		}

		// Enabling connecting label
		connecting_label->setText("Waiting for players");
		connecting_label->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.0f, 24.5f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
		connecting_label->setTextColor({ 0, 0, 0 });

		// Reset to pairin
		network_server->state = NetworkServer::SERVER_STATE_PAIRING;

		break;
	}
	default:
		break;
	}
}

void MainMenu::HandleClientAgentEvent(NetworkAgent::NetworkAgentEvent event)
{
	switch (event)
	{
	case NetworkAgent::EVENT_PAIR_CONNECTED:
	{
	}
	break;
	case NetworkAgent::EVENT_PAIR_DISCONNECTED:
	{
		// Set failure
		connecting_label->setText("Disconnected from server");
		connecting_label->setTextColor({ 255, 0, 0 });
		connecting_label->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 8.f, 24.5f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });

		// Deactivate label
		player_info_label->isActive = false;
		player_color_info_label->isActive = false;
		connection_action_label->isActive = false;

		// Stop connection
		disconnected = true;
		connection_established = false;

		break;
	}
	default:
		break;
	}
}

// Validation methods

std::optional<std::pair<std::string, std::optional<int>>> MainMenu::getAddressIfValid(std::string address)
{
	std::regex ip_port{R"(^((?:(?:25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])\.){3}(?:25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9]))(?::(\d{1,5}))?$)"};

	std::smatch matches;
	if (std::regex_match(address, matches, ip_port)) 
	{
		for (int i = 0; i < matches.size(); i++)
		{
			std::string match = matches[i];

			// If there's port
			if (i == 2)
			{
				if (!match.empty())
				{
					if (int port = std::stoi(match); port < 65536)
					{
						// Return IP + Port
						return  std::make_pair(matches[1], port);
					}
					else
						// Return nothing, port is not valid
						return {};
				}
			}
		}

		// Return IP + nullopt
		return std::make_pair(matches[1], std::nullopt);
	}
	
	// Return nothing, IP is not valid
	return {};
}

// Config files methods

void MainMenu::saveGeneralConfig()
{
	// Open stream
	std::ofstream output_file{ general_config.c_str() };

	if (!output_file)
	{
		return;
	}

	// Get values to save
	// Graphics options
	bool fullscreen = fullscreen_checkbox->isSelected();
	std::string win_w = window_width_input->getText();
	std::string win_h = window_height_input->getText();

	// Sound options
	bool sound_enabled = sound_checkbox->isSelected();
	std::string volume = sound_volume_input->getText();

	// Write values to file
	output_file << fullscreen << "\n";
	output_file << win_w << "\n";
	output_file << win_h << "\n";

	output_file << sound_enabled << "\n";
	output_file << volume << "\n";

	// Close file
	output_file.close();
}

void MainMenu::readClientConfig()
{
	std::ifstream client_file(client_config);

	if (client_file)
	{
		std::getline(client_file, client_ip);
		std::getline(client_file, client_port);
	}
}

void MainMenu::saveClientConfig()
{
	std::ofstream client_file(client_config);

	if (client_file)
	{
		client_file << client_ip << "\n";
		client_file << client_port << "\n";;

		client_file.close();
	}
}

void MainMenu::readServerConfig()
{
	std::ifstream server_file(server_config);

	if (server_file)
	{
		std::getline(server_file, server_port);
		std::getline(server_file, server_player_amount);
		std::getline(server_file, server_frame_buffer);
		
		std::string ff;
		std::getline(server_file, ff);
		std::istringstream(ff) >> server_friendly_fire;
		
		std::string tpu;
		std::getline(server_file, tpu);
		std::istringstream(tpu) >> server_team_powerups;

		server_file.close();
	}
}

void MainMenu::saveServerConfig()
{
	std::ofstream server_file(server_config);

	if (server_file)
	{
		server_file << server_port << "\n";;
		server_file << server_player_amount << "\n";;
		server_file << server_frame_buffer << "\n";;
		server_file << server_friendly_fire << "\n";;
		server_file << server_team_powerups << "\n";;

		server_file.close();
	}
}

// Convenience

void MainMenu::setDefaultLayout()
{
	// Game title 1
	game_title_1->setTextScale(Vector2<float>(4, 4));

	game_title_1->setText("Archers");
	game_title_1->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 5.5f, 3.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	game_title_1->setLayer(EVERY_LAYER);

	// Game title 2
	game_title_2->transform.parent = &game_title_1->transform;
	game_title_2->setTextScale(Vector2<float>(4, 4));

	game_title_2->setText("Duel");
	game_title_2->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 11.5f, 11.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	game_title_2->setLayer(EVERY_LAYER);

	// Play Button
	play_button->setScale(Vector2<float>(2, 2));
	play_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.f, 20.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	play_button->tLabel->setText("Play");
	play_button->tLabel->setTextScale(Vector2<float>(2.f, 2.f));
	play_button->tLabel->setCenteredWithinParent();
	play_button->setOnClickListener(std::bind(&MainMenu::playButtonHandler, this));
	play_button->setLayer(MAIN_MENU_LAYER | PLAYER_MENU_LAYER);

	// Online button
	online_button->setScale(Vector2<float>(2, 2));
	online_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.f, 32.5f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	online_button->tLabel->setText("Online");
	online_button->tLabel->setTextScale(Vector2<float>(2.f, 2.f));
	online_button->tLabel->setCenteredWithinParent();
	online_button->setOnClickListener(std::bind(&MainMenu::onlineButtonHandler, this));
	online_button->setLayer(MAIN_MENU_LAYER);

	// Online Sub-menu
		// Server button
	server_button->setScale(Vector2<float>(2, 2));
	server_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.f, 20.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	server_button->tLabel->setText("Server");
	server_button->tLabel->setTextScale(Vector2<float>(2.f, 2.f));
	server_button->tLabel->setCenteredWithinParent();
	server_button->setOnClickListener(std::bind(&MainMenu::serverButtonHandler, this));
	server_button->isActive = false;
	server_button->setLayer(ONLINE_MENU_LAYER);

	// Server Address / Server Port
	ip_input->setScale(Vector2<float>(2.5f, 1));
	ip_input->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 7.5f, 24.5f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	ip_input->tLabel->setTextScale(Vector2<float>(1.f, 1.f));
	ip_input->tLabel->setCenteredWithinParent();
	ip_input->isActive = false;
	ip_input->onSelect = std::bind(&MainMenu::onSelectIPInput, this);
	ip_input->valid_inputs = { R"(\d|\.|:|[a-z])" };
	ip_input->setLayer(CLIENT_MENU_LAYER | SERVER_MENU_LAYER);

	// Server address label
	ip_label->setScale(Vector2<float>(1, 1));
	ip_label->setText("Server Address");
	ip_label->transform.parent = &ip_input->transform;
	ip_label->setCenteredWithinParent({ 0, -15.f });
	ip_label->setTextColor({ 0, 0, 0 });

	// Connect button
	connect_button->setScale(Vector2<float>(2, 2));
	connect_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.f, 32.5f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	connect_button->tLabel->setText("Connect");
	connect_button->tLabel->setTextScale(Vector2<float>(2.f, 2.f));
	connect_button->tLabel->setCenteredWithinParent();
	connect_button->setOnClickListener(std::bind(&MainMenu::connectButtonHandler, this));
	connect_button->isActive = false;
	connect_button->setLayer(CLIENT_MENU_LAYER | SERVER_MENU_LAYER);

	// Player amount input
	player_amount_input->setScale(Vector2<float>(2.5f, 1));
	player_amount_input->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 7.5f, 35.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	player_amount_input->tLabel->setTextScale(Vector2<float>(1.f, 1.f));
	player_amount_input->tLabel->setCenteredWithinParent();
	player_amount_input->isActive = false;
	player_amount_input->valid_inputs = { R"(\d)" };
	player_amount_input->setLayer(SERVER_MENU_LAYER | PLAYER_MENU_LAYER);

	// Player amount label
	player_amount_label->setScale(Vector2<float>(1, 1));
	player_amount_label->setText("Player Amount");
	player_amount_label->transform.parent = &player_amount_input->transform;
	player_amount_label->setCenteredWithinParent({ 0, -15.f });
	player_amount_label->setTextColor({ 0, 0, 0 });

	// Friendly fire check box
	friendly_fire_checkbox->transform.parent = &player_amount_input->transform;
	friendly_fire_checkbox->setCenteredWithinParent({ 30, 20 });
	friendly_fire_checkbox->isActive = false;
	friendly_fire_checkbox->tLabel->isActive = false;
	friendly_fire_checkbox->tLabel->setText("Friendly Fire");
	friendly_fire_checkbox->tLabel->setTextColor({ 0,0,0 });
	friendly_fire_checkbox->tLabel->setCenteredWithinParent({ -45, 0 });
	friendly_fire_checkbox->setLayer(SERVER_MENU_LAYER | PLAYER_MENU_LAYER);

	// Friendly fire check box
	shared_power_up_checkbox->transform.parent = &player_amount_input->transform;
	shared_power_up_checkbox->setCenteredWithinParent({ 30, 40 });
	shared_power_up_checkbox->isActive = false;
	shared_power_up_checkbox->tLabel->isActive = false;
	shared_power_up_checkbox->tLabel->setText("Team PowerUps");
	shared_power_up_checkbox->tLabel->setTextColor({ 0,0,0 });
	shared_power_up_checkbox->tLabel->setCenteredWithinParent({ -45, 0 });
	shared_power_up_checkbox->setLayer(SERVER_MENU_LAYER | PLAYER_MENU_LAYER);

	// Frame amount input
	frame_amount_input->setScale(Vector2<float>(2.5f, 1));
	frame_amount_input->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 7.5f, 45.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	frame_amount_input->tLabel->setTextScale(Vector2<float>(1.f, 1.f));
	frame_amount_input->tLabel->setCenteredWithinParent();
	frame_amount_input->isActive = false;
	frame_amount_input->valid_inputs = { R"(\d)" };
	frame_amount_input->setLayer(SERVER_MENU_LAYER);

	// Frmae amount lable
	frame_amount_label->setScale(Vector2<float>(1, 1));
	frame_amount_label->setText("Frame Buffer Size");
	frame_amount_label->transform.parent = &frame_amount_input->transform;
	frame_amount_label->setCenteredWithinParent({ 0, -15.f });
	frame_amount_label->setTextColor({ 0, 0, 0 });

	// Client button
	client_button->setScale(Vector2<float>(2, 2));
	client_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.f, 32.5f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	client_button->tLabel->setText("Client");
	client_button->tLabel->setTextScale(Vector2<float>(2.f, 2.f));
	client_button->tLabel->setCenteredWithinParent();
	client_button->setOnClickListener(std::bind(&MainMenu::clientButtonHandler, this));
	client_button->isActive = false;
	client_button->setLayer(ONLINE_MENU_LAYER);

	// Connection label
	connecting_label->setScale(Vector2<float>(1, 1));
	connecting_label->setText("Connecting");
	connecting_label->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 11.25f, 24.5f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	connecting_label->setTextColor({ 0, 0, 0 });
	connecting_label->setLayer(CLIENT_CONNECTION_MENU_LAYER | SERVER_CONNECTION_MENU_LAYER);
	connecting_label->isActive = false;

	// Player info label
	player_info_label->setScale(Vector2<float>(1, 1));
	player_info_label->setTextColor({ 0, 0, 0 });
	player_info_label->setText("You are player");
	player_info_label->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 12.5f, 30.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	player_info_label->isActive = false;

	// Player color report label
	player_color_info_label->setScale({ 1, 1 });
	player_color_info_label->setTextColor({ 255, 0, 0 });
	player_color_info_label->setText("red");
	player_color_info_label->transform.parent = &player_info_label->transform;
	player_color_info_label->setCenteredWithinParent({ 0, 15.f });

	// Connected players label
	connected_players_label->setScale(Vector2<float>(1, 1));
	connected_players_label->setText("Connected players: 0/0");
	connected_players_label->setTextColor({ 0, 0, 0 });
	connected_players_label->transform.parent = &player_color_info_label->transform;
	connected_players_label->setCenteredWithinParent({ 0, 15.f });

	// Connection info label
	connection_action_label->setScale({ 1, 1 });
	connection_action_label->setText("Waiting for server");
	connection_action_label->transform.parent = &connected_players_label->transform;
	connection_action_label->setCenteredWithinParent({ 0, 15.f });
	connection_action_label->isActive = false;

	// Back button
	back_button->setScale(Vector2<float>(2, 2));
	back_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.f, 45.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	back_button->tLabel->setText("Back");
	back_button->tLabel->setTextScale(Vector2<float>(2.f, 2.f));
	back_button->tLabel->setCenteredWithinParent();
	back_button->setOnClickListener(std::bind(&MainMenu::backButtonHandler, this));
	back_button->isActive = false;
	back_button->setLayer(EVERY_LAYER - 1);

	// Exit button
	exit_button->setScale(Vector2<float>(0.25f, 0.25f));
	exit_button->transform.position = Vector2<float>(5, 5);
	exit_button->setOnClickListener(std::bind(&MainMenu::exitGame, this));
	exit_button->tLabel->isActive = false;
	exit_button->tLabel->setText("");
	exit_button->setLayer(EVERY_LAYER);

	// Options
	options_button->setScale(Vector2<float>(2, 2));
	options_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.f, 45.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	options_button->tLabel->setText("Settings");
	options_button->tLabel->setTextScale(Vector2<float>(2.f, 2.f));
	options_button->tLabel->setCenteredWithinParent();
	options_button->setOnClickListener(std::bind(&MainMenu::optionsButtonHandler, this));
	options_button->setLayer(MAIN_MENU_LAYER);

	// Graphics TextLabel
	graphics_options_title->setScale(Vector2<float>(1.5f, 1.5f));
	graphics_options_title->setText("Graphics");
	graphics_options_title->setTextColor({ 0, 0, 0 });
	graphics_options_title->transform.parent = &game_title_2->transform;
	graphics_options_title->setCenteredWithinParent({ 0, 20.f });
	graphics_options_title->setLayer(OPTIONS_MENU_LAYER);
	graphics_options_title->isActive = false;

	// Fullscreen text label
	fullscreen_label->setScale(Vector2<float>(1.f, 1.f));
	fullscreen_label->setText("Fullscreen");
	fullscreen_label->setTextColor({ 0, 0, 0 });
	fullscreen_label->transform.parent = &graphics_options_title->transform;
	fullscreen_label->setCenteredWithinParent({ -20, 20.f });
	fullscreen_label->setLayer(OPTIONS_MENU_LAYER);
	fullscreen_label->isActive = false;

	// Fullscreen check box
	fullscreen_checkbox->transform.parent = &fullscreen_label->transform;
	fullscreen_checkbox->setCenteredWithinParent({ 40, 0 });
	fullscreen_checkbox->setLayer(OPTIONS_MENU_LAYER);
	fullscreen_checkbox->isActive = false;
	fullscreen_checkbox->tLabel->isActive = false;
	fullscreen_checkbox->tLabel->setText("");

	// Window  width label
	window_width_label->setScale(Vector2<float>(1.f, 1.f));
	window_width_label->setText("Window Width");
	window_width_label->setTextColor({ 0, 0, 0 });
	window_width_label->transform.parent = &fullscreen_label->transform;
	window_width_label->setCenteredWithinParent({ -20, 20 });
	window_width_label->setLayer(OPTIONS_MENU_LAYER);
	window_width_label->isActive = false;

	// Window size width
	window_width_input->setScale(Vector2<float>(1, 1));
	window_width_input->transform.parent = &window_width_label->transform;
	window_width_input->setCenteredWithinParent({ 0, 20 });
	window_width_input->tLabel->setTextScale(Vector2<float>(1.f, 1.f));
	window_width_input->tLabel->setCenteredWithinParent();
	window_width_input->isActive = false;
	window_width_input->valid_inputs = { R"(\d)" };
	window_width_input->setLayer(OPTIONS_MENU_LAYER);
	window_width_input->isActive = false;

	// Window height label
	window_height_label->setScale(Vector2<float>(1.f, 1.f));
	window_height_label->setText("Window Height");
	window_height_label->setTextColor({ 0, 0, 0 });
	window_height_label->transform.parent = &fullscreen_label->transform;
	window_height_label->setCenteredWithinParent({ 60, 20 });
	window_height_label->setLayer(OPTIONS_MENU_LAYER);
	window_height_label->isActive = false;

	// Window size height
	window_height_input->setScale(Vector2<float>(1, 1));
	window_height_input->transform.parent = &window_height_label->transform;
	window_height_input->setCenteredWithinParent({ 0, 20 });
	window_height_input->tLabel->setTextScale(Vector2<float>(1.f, 1.f));
	window_height_input->tLabel->setCenteredWithinParent();
	window_height_input->isActive = false;
	window_height_input->valid_inputs = { R"(\d)" };
	window_height_input->setLayer(OPTIONS_MENU_LAYER);

	// Sound TextLabel title
	sound_options_title->setScale(Vector2<float>(1.5f, 1.5f));
	sound_options_title->setText("Sound");
	sound_options_title->setTextColor({ 0, 0, 0 });
	sound_options_title->transform.parent = &graphics_options_title->transform;
	sound_options_title->setCenteredWithinParent({ 0, 80.f });
	sound_options_title->setLayer(OPTIONS_MENU_LAYER);
	sound_options_title->isActive = false;

	// Sound text label
	sound_label->setScale(Vector2<float>(1.f, 1.f));
	sound_label->setText("Sound");
	sound_label->setTextColor({ 0, 0, 0 });
	sound_label->transform.parent = &sound_options_title->transform;
	sound_label->setCenteredWithinParent({ -34, 20.f });
	sound_label->setLayer(OPTIONS_MENU_LAYER);
	sound_label->isActive = false;

	// Sound checkbox
	sound_checkbox->transform.parent = &fullscreen_checkbox->transform;
	sound_checkbox->setCenteredWithinParent({ 0, 80 });
	sound_checkbox->setLayer(OPTIONS_MENU_LAYER);
	sound_checkbox->isActive = false;
	sound_checkbox->select();
	sound_checkbox->tLabel->isActive = false;
	sound_checkbox->tLabel->setText("");

	// Sound volume label
	sound_volume_label->setScale(Vector2<float>(1.f, 1.f));
	sound_volume_label->setText("Volume");
	sound_volume_label->setTextColor({ 0, 0, 0 });
	sound_volume_label->transform.parent = &sound_options_title->transform;
	sound_volume_label->setCenteredWithinParent({ 0, 40.f });
	sound_volume_label->setLayer(OPTIONS_MENU_LAYER);
	sound_volume_label->isActive = false;

	// Sound volume input
	sound_volume_input->setScale(Vector2<float>(1, 1));
	sound_volume_input->transform.parent = &sound_volume_label->transform;
	sound_volume_input->setCenteredWithinParent({ 0, 20 });
	sound_volume_input->tLabel->setTextScale(Vector2<float>(1.f, 1.f));
	sound_volume_input->tLabel->setCenteredWithinParent();
	sound_volume_input->isActive = false;
	sound_volume_input->valid_inputs = { R"(\d)" };
	sound_volume_input->setLayer(OPTIONS_MENU_LAYER);
}