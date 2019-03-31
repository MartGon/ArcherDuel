#include "MainMenu.h"
#include "LevelOne.h"

#include "AudioManager.h"
#include "RendererManager.h"
#include "SceneManager.h"

#include "Tower.h"
#include "HealthBar.h"

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

	// Player
	player = new Player();
	player->mov_enabled = false;
	Vector2<float> player_pos(64, LEVEL_HEIGHT - 143);
	player->transform.position = player_pos;

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
	play_button->setOnClickListener(std::bind(&MainMenu::loadLevelOne, this));

	// Exit button
	exit_button = new Button(Texture("ExitButton.png"));
	exit_button->setScale(Vector2<float>(0.25f, 0.25f));
	exit_button->transform.position = Vector2<float>(5, 5);
	exit_button->setOnClickListener(std::bind(&MainMenu::exitGame, this));
	exit_button->tLabel->isActive = false;

	// Renderer Manager setup
	RendererManager::setCameraPosition(Vector2<int>(0, 0), Vector2<int>(LEVEL_WIDTH, LEVEL_HEIGHT));
}

void MainMenu::onUpdate()
{

}

void MainMenu::handleEvent(const SDL_Event& event)
{
	for (auto go : gameObjectMap)
		go.second->handleEvent(event);
}

// Own methods

void MainMenu::exitGame()
{
	SceneManager::quitGame();
}

void MainMenu::loadLevelOne()
{
	SceneManager::loadNextScene(new LevelOne());
}