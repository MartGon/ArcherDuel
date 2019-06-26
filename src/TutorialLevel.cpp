#include "TutorialLevel.h"
#include "LevelOne.h"
#include "MainMenu.h"

#include "AudioManager.h"
#include "SceneManager.h"

#include "Tower.h"

const int TutorialLevel::LEVEL_WIDTH = 480;
const int TutorialLevel::LEVEL_HEIGHT = 270;

// Constructor

TutorialLevel::TutorialLevel()
{

}

// Overrided methods

void TutorialLevel::loadMedia()
{
	// Setting managers
	setManager(new CollisionManager());
	AudioManager* aManager = new AudioManager();
	aManager->enable();
	setManager(aManager);
	setManager(new RendererManager());

	// GameObjects

	// Background
	GameObject *background = new GameObject();
	background->setComponent(new TextureRenderer("ShittyBackground.png", nullptr, 1));
	background->transform.scale = Vector2<float>(2, 2);

	// Floor
	LevelOne::placeFloorBlocks();

	// Red Tower
	tower = new Tower();
	Vector2<float> tower_pos(0, LEVEL_HEIGHT - 160 - 31);
	tower->transform.position = tower_pos;

	// GUI
	Button* button = new Button(Texture("ExitButton.png", RendererManager::renderer));
	button->setScale(Vector2<float>(0.25f, 0.25f));
	button->transform.position = Vector2<float>(5, 5);
	button->setOnClickListener(std::bind(&TutorialLevel::exitButtonHandler, this));
	button->tLabel->isActive = false;

	// DialogBox
	DialogBox* dialog = new DialogBox("elcome to Archers Duel! \n Complete this short tutorial \n Or whatever dude \n 1 \n 2 \n 3 \n 4");
	dialog->line_offset_y = 10.f;
	dialog->text_speed = 2;
	dialog->setText("This tutorial will teach what you need to know to play the game. \n On Archers Duel you play as this Archer.");
	dialog->transform.position = { LEVEL_WIDTH / 3, LEVEL_HEIGHT / 32 };

	// Continue button
	continue_button = new Button("ArrowButton.png");
	continue_button->setScale(Vector2<float>(1.f, 1.f));
	continue_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 2.5f, 12.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	continue_button->setOnClickListener(std::bind(&TutorialLevel::continueButtonHandler, this));
	continue_button->tLabel->isActive = false;
}

void TutorialLevel::onUpdate()
{
}

// GUI methods

void TutorialLevel::exitButtonHandler()
{
	SceneManager::loadNextScene(new MainMenu());
}

void TutorialLevel::continueButtonHandler()
{

}