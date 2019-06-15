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
	game_title_1->setRelativePosition(Vector2<float>(LEVEL_WIDTH - LEVEL_WIDTH / 10 * 3, LEVEL_HEIGHT / 10 * 0.5f));

	// Game title 2
	game_title_2 = new TextLabel();
	game_title_2->transform.parent = &game_title_1->transform;
	game_title_2->setTextScale(Vector2<float>(4, 4));

	game_title_2->setText("Duel");
	game_title_2->setRelativePosition(Vector2<float>(LEVEL_WIDTH / 10 * 0.75f, LEVEL_HEIGHT / 10));

	// Game title bow
	//game_title_bow = new GameObject();
	//game_title_bow->setComponent(new TextureRenderer("MyBow1.png", new MapRGB(0, 0, 255), 255));
	//game_title_bow->transform.scale = Vector2<float>(0.6f, 0.4f);
	//ame_title_bow->setRelativePosition(Vector2<float>(LEVEL_WIDTH / 20 * 1.5f , LEVEL_HEIGHT / 20 * 2.05f));

	// Play Button
	play_button = new Button(Texture("button_background4.png"));
	play_button->setScale(Vector2<float>(2, 2));
	play_button->setRelativePosition(Vector2<float>(LEVEL_WIDTH - LEVEL_WIDTH / 10 * 1.75f, LEVEL_HEIGHT / 10 * 2.5f));
	play_button->tLabel->setText("Play");
	play_button->tLabel->setTextScale(Vector2<float>(2.f, 2.f));
	play_button->tLabel->transform.position = (play_button->collider->getDimensions() / 2 ) - Vector2<float>(20, 5);
	play_button->setOnClickListener(std::bind(&MainMenu::playButtonHandler, this));

	// Online button
	online_button = new Button(Texture("button_background4.png"));
	online_button->setScale(Vector2<float>(2, 2));
	online_button->setRelativePosition(Vector2<float>(LEVEL_WIDTH - LEVEL_WIDTH / 10 * 1.75f, LEVEL_HEIGHT / 10 * 3.75f));
	online_button->tLabel->setText("Online");
	online_button->tLabel->setTextScale(Vector2<float>(2.f, 2.f));
	online_button->tLabel->transform.position = (online_button->collider->getDimensions() / 2) - Vector2<float>(30, 5);
	online_button->setOnClickListener(std::bind(&MainMenu::onlineButtonHandler, this));

	// Online Sub-menu
		// Server button
	server_button = new Button(Texture("button_background4.png"));
	server_button->setScale(Vector2<float>(2, 2));
	server_button->setRelativePosition(Vector2<float>(LEVEL_WIDTH - LEVEL_WIDTH / 10 * 1.75f, LEVEL_HEIGHT / 10 * 2.5f));
	server_button->tLabel->setText("Server");
	server_button->tLabel->setTextScale(Vector2<float>(2.f, 2.f));
	server_button->tLabel->transform.position = (server_button->collider->getDimensions() / 2) - Vector2<float>(30, 5);
	server_button->setOnClickListener(std::bind(&MainMenu::serverButtonHandler, this));
	server_button->isActive = false;

		// Client button
	client_button = new Button(Texture("button_background4.png"));
	client_button->setScale(Vector2<float>(2, 2));
	client_button->setRelativePosition(Vector2<float>(LEVEL_WIDTH - LEVEL_WIDTH / 10 * 1.75f, LEVEL_HEIGHT / 10 * 3.75f));
	client_button->tLabel->setText("Client");
	client_button->tLabel->setTextScale(Vector2<float>(2.f, 2.f));
	client_button->tLabel->transform.position = (client_button->collider->getDimensions() / 2) - Vector2<float>(30, 5);
	client_button->setOnClickListener(std::bind(&MainMenu::clientButtonHandler, this));
	client_button->isActive = false;

	// Client button
	back_button = new Button(Texture("button_background4.png"));
	back_button->setScale(Vector2<float>(2, 2));
	back_button->setRelativePosition(Vector2<float>(LEVEL_WIDTH - LEVEL_WIDTH / 10 * 1.75f, LEVEL_HEIGHT / 10 * 5.f));
	back_button->tLabel->setText("Back");
	back_button->tLabel->setTextScale(Vector2<float>(2.f, 2.f));
	back_button->tLabel->transform.position = (back_button->collider->getDimensions() / 2) - Vector2<float>(20, 5);
	back_button->setOnClickListener(std::bind(&MainMenu::backButtonHandler, this));
	back_button->isActive = false;

	// Exit button
	exit_button = new Button(Texture("ExitButton.png"));
	exit_button->setScale(Vector2<float>(0.25f, 0.25f));
	exit_button->transform.position = Vector2<float>(5, 5);
	exit_button->setOnClickListener(std::bind(&MainMenu::exitGame, this));
	exit_button->tLabel->isActive = false;

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

}

void MainMenu::OnHandleEvent(const SDL_Event& event)
{
}

// Own methods

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
	loadLevelOne();
}

void MainMenu::onlineButtonHandler()
{
	// Deactivate old buttons
	play_button->isActive = false;
	online_button->isActive = false;

	// Activate new buttons
	server_button->isActive = true;
	client_button->isActive = true;
	back_button->isActive = true;
}

void MainMenu::serverButtonHandler()
{
	loadLevelOne(ONLINE_SERVER);
}

void MainMenu::clientButtonHandler() 
{
	loadLevelOne(ONLINE_CLIENT);
}

void MainMenu::backButtonHandler()
{
	// Activate old buttons
	play_button->isActive = true;
	online_button->isActive = true;

	// Deactivate new buttons
	server_button->isActive = false;
	client_button->isActive = false;
	back_button->isActive = false;
}