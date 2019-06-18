#include "MainMenu.h"
#include "LevelOne.h"

#include "AudioManager.h"
#include "RendererManager.h"
#include "SceneManager.h"

#include "Tower.h"
#include "HealthBar.h"
#include "Cannon.h"
#include "PowerUp.h"

// Const values
const int MainMenu::LEVEL_WIDTH = 480;
const int MainMenu::LEVEL_HEIGHT = 320;

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
	Vector2<float> tower_pos(0, LEVEL_HEIGHT - 160 - 32);
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

	// Game title 2
	game_title_2 = new TextLabel();
	game_title_2->transform.parent = &game_title_1->transform;
	game_title_2->setTextScale(Vector2<float>(4, 4));

	game_title_2->setText("Duel");
	game_title_2->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 11.5f, 11.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });

	// Play Button
	play_button = new Button(Texture("button_background4.png"));
	play_button->setScale(Vector2<float>(2, 2));
	play_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.f, 20.f }, {ALIGN_FROM_RIGHT, ALIGN_FROM_TOP});
	play_button->tLabel->setText("Play");
	play_button->tLabel->setTextScale(Vector2<float>(2.f, 2.f));
	play_button->tLabel->setCenteredWithinParent();
	play_button->setOnClickListener(std::bind(&MainMenu::playButtonHandler, this));
	play_button->layer = MAIN_MENU_LAYER | PLAYER_MENU_LAYER;

	// Online button
	online_button = new Button(Texture("button_background4.png"));
	online_button->setScale(Vector2<float>(2, 2));
	online_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.f, 32.5f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	online_button->tLabel->setText("Online");
	online_button->tLabel->setTextScale(Vector2<float>(2.f, 2.f));
	online_button->tLabel->setCenteredWithinParent();
	online_button->setOnClickListener(std::bind(&MainMenu::onlineButtonHandler, this));
	online_button->layer = MAIN_MENU_LAYER;

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
	server_button->layer = ONLINE_MENU_LAYER;

			// Server Address / Server Port
	ip_input = new TextInput("button_background4.png", "127.0.0.1:1338");
	ip_input->setScale(Vector2<float>(2.5f, 1));
	ip_input->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 7.5f, 24.5f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	ip_input->tLabel->setTextScale(Vector2<float>(1.f, 1.f));
	ip_input->tLabel->setCenteredWithinParent();
	ip_input->isActive = false;
	ip_input->layer = CLIENT_MENU_LAYER | SERVER_MENU_LAYER;
		
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
	connect_button->layer = CLIENT_MENU_LAYER | SERVER_MENU_LAYER;

		// Player amount input
	player_amount_input = new TextInput("button_background4.png", "4");
	player_amount_input->setScale(Vector2<float>(2.5f, 1));
	player_amount_input->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 7.5f, 35.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	player_amount_input->tLabel->setTextScale(Vector2<float>(1.f, 1.f));
	player_amount_input->tLabel->setCenteredWithinParent();
	player_amount_input->isActive = false;
	player_amount_input->layer = SERVER_MENU_LAYER | PLAYER_MENU_LAYER;

		// Player amount label
	player_amount_label = new TextLabel();
	player_amount_label->setScale(Vector2<float>(1, 1));
	player_amount_label->setText("Player Amount");
	player_amount_label->transform.parent = &player_amount_input->transform;
	player_amount_label->setCenteredWithinParent({ 0, -15.f });
	player_amount_label->setTextColor({ 0, 0, 0 });

		// Frame amount input
	frame_amount_input = new TextInput("button_background4.png", "8");
	frame_amount_input->setScale(Vector2<float>(2.5f, 1));
	frame_amount_input->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 7.5f, 45.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	frame_amount_input->tLabel->setTextScale(Vector2<float>(1.f, 1.f));
	frame_amount_input->tLabel->setCenteredWithinParent();
	frame_amount_input->isActive = false;
	frame_amount_input->layer = SERVER_MENU_LAYER;

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
	client_button->layer = ONLINE_MENU_LAYER;

		// Back button
	back_button = new Button(Texture("button_background4.png"));
	back_button->setScale(Vector2<float>(2, 2));
	back_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.f, 45.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	back_button->tLabel->setText("Back");
	back_button->tLabel->setTextScale(Vector2<float>(2.f, 2.f));
	back_button->tLabel->setCenteredWithinParent();
	back_button->setOnClickListener(std::bind(&MainMenu::backButtonHandler, this));
	back_button->isActive = false;
	back_button->layer = EVERY_LAYER - 1;

	// Exit button
	exit_button = new Button(Texture("ExitButton.png"));
	exit_button->setScale(Vector2<float>(0.25f, 0.25f));
	exit_button->transform.position = Vector2<float>(5, 5);
	exit_button->setOnClickListener(std::bind(&MainMenu::exitGame, this));
	exit_button->tLabel->isActive = false;
	exit_button->layer = EVERY_LAYER;

	// Player
	player = new Player(PlayerNumber::PLAYER_ONE);
	player->tower = tower;
	player->mov_enabled = false;
	Vector2<float> player_pos(64, LEVEL_HEIGHT - 143);
	player->transform.position = player_pos;

	// Renderer Manager setup
	RendererManager::setCameraPosition(Vector2<int>(0, 0), Vector2<int>(LEVEL_WIDTH, LEVEL_HEIGHT));
}

void MainMenu::onUpdate()
{
	// Fill vector
	if(widgets.empty())
		widgets = getGameObjects<Button*>();
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

// Own methods

void MainMenu::enableLayer(Uint8 layer)
{
	for (auto widget : widgets)
	{
		if (widget->layer & layer)
			widget->isActive = true;
		else
			widget->isActive = false;
	}

	current_layer = layer;
}

void MainMenu::exitGame()
{
	SceneManager::quitGame();
}

void MainMenu::loadLevelOne(SceneMode mode)
{
	Scene* level_one = new LevelOne();
	level_one->setSceneMode(mode);
	SceneManager::loadNextScene(level_one);
}

void MainMenu::playButtonHandler()
{
	//loadLevelOne();
	if (current_layer == MAIN_MENU_LAYER)
	{
		// Set next mode
		next_mode = SINGLE_PLAYER;

		// Move player amount input 
		player_amount_input->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 7.5f, 24.5f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
		play_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.f, 37.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
		back_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.f, 50.5f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });

		enableLayer(PLAYER_MENU_LAYER);
	}
	else
		loadLevelOne();
}

void MainMenu::onlineButtonHandler()
{
	enableLayer(ONLINE_MENU_LAYER);
}

void MainMenu::serverButtonHandler()
{
	// Set next mode
	next_mode = ONLINE_SERVER;

	// Set text
	ip_label->setText("Server port");
	ip_label->setTextColor({ 0, 0, 0 });
	ip_input->setText("65534");
	ip_label->setCenteredWithinParent({ 0, -15.f });

	// Move connect and back buttons
	player_amount_input->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 7.5f, 35.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	connect_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.f, 57.5f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	back_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.f, 70.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });

	enableLayer(SERVER_MENU_LAYER);
}

void MainMenu::clientButtonHandler() 
{
	// Set next mode
	next_mode = ONLINE_CLIENT;

	// Set text
	ip_label->setText("Server Address");
	ip_label->setTextColor({ 0, 0, 0 });
	ip_input->setText("127.0.0.1:65534");
	ip_label->setCenteredWithinParent({ 0, -15.f });

	// Move connect and back buttons
	connect_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.f, 37.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	back_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.f, 50.5f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });

	enableLayer(CLIENT_MENU_LAYER);
}

void MainMenu::connectButtonHandler()
{
	// Network connection stuff
	std::cout << "Connecting \n";
}

void MainMenu::backButtonHandler()
{
	// Reset pos
	play_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.f, 20.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	back_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 10.f, 45.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });

	enableLayer(MAIN_MENU_LAYER);
}