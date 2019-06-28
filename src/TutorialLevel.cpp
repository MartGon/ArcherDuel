#include "TutorialLevel.h"
#include "LevelOne.h"
#include "MainMenu.h"

#include "AudioManager.h"
#include "SceneManager.h"

#include "Cannon.h"
#include "Tower.h"
#include "HealthBar.h"

#include "Random.h"

#include <functional>


// Class Bullseye

// Constructor

Bullseye::Bullseye()
{
	// Init components

	// Texture Renderer
	MapRGB colorKey = { 0, 255, 0 };
	tRenderer = setComponent(new TextureRenderer("Bullseye.png", &colorKey, 255));

	// BoxCollider
	bCollider = setComponent(new BoxCollider(tRenderer->texture));
}

// Overrided methdos

void Bullseye::onColliderEnter(Collider* col)
{
	if (GameObject* go = col->gameObject)
	{
		if (Arrow* arrow = dynamic_cast<Arrow*>(go))
		{
			// Disable arrow
			arrow->isActive = false;

			// Increase quest progress
			if (tLevel)
			{
				tLevel->onPracticeTargetHit();

				// Destroy
				this->isActive = false;
				this->destroy();
			}
		}
	}
}

// TutorialLevel class 

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
	tower->tLevel = this;

	// Blue tower
	tower2 = new Tower(Tower::ROOF_COLOR_BLUE);
	Vector2<float> tower2_pos(LEVEL_WIDTH - 128, LEVEL_HEIGHT - 160 - 31);
	tower2->transform.position = tower2_pos;
	tower2->isActive = false;
	tower2->tLevel = this;

	// Create practice targets
	for (int i = 0; i < 5; i++)
	{
		// Creat target and set ref
		Bullseye* target = new Bullseye();
		target->tLevel = this;
		target->isActive = false;

		// Add to vector
		targets.push_back(target);
	}

	// Set targets pos
	targets.at(0)->transform.position = { 50.f, 40.f };
	targets.at(1)->transform.position = { 220.f, 120.f };
	targets.at(2)->transform.position = { 170.f, 210.f };
	targets.at(3)->transform.position = { 340.f, 40.f };
	targets.at(4)->transform.position = { 420.f, 140.f };

	// Create power up objects
	power_up_objects.push_back(new PowerUpObject(PowerUpType::POWER_UP_FIRE));
	power_up_objects.push_back(new PowerUpObject(PowerUpType::POWER_UP_HASTE));
	power_up_objects.push_back(new PowerUpObject(PowerUpType::POWER_UP_MIRROR));
	power_up_objects.push_back(new PowerUpObject(PowerUpType::POWER_UP_SHIELD));
	power_up_objects.push_back(new PowerUpObject(PowerUpType::POWER_UP_THUNDERSTRIKE));
	power_up_objects.push_back(new PowerUpObject(PowerUpType::POWER_UP_TRIPLE));

	for (int i = 0; i < power_up_objects.size(); i++)
	{
		// Get powerup i
		auto pou = power_up_objects.at(i);
		pou->setScale({ 1.5f, 1.5f });
		pou->vanish_timer->isStopped = true;

		// Set position
		float offset_y = (i / 3) * 38 + 10;
		float offset_x = i % 3 * 40 + 10;
		pou->transform.position = { offset_x, offset_y };

		// Set inactive
		pou->isActive = false;

		// Bind callback
		pou->onHit = std::bind(&TutorialLevel::onPowerUpHit, this, std::placeholders::_1);
	}

	// Create player
	player = new Player(PlayerNumber::PLAYER_ONE);
	players.push_back(player);
	player->tower = tower;
	player->tLevel = this;
	player->transform.position = Vector2<float> (64, LEVEL_HEIGHT - 143);
	player->isActive = false;
	player->canGainSkill = false;

	TimerObject* player_timer = new TimerObject(3 * 1000, player->id);
	player_timer->timer->isOver = true;
	player_timer->callback = std::bind(&TutorialLevel::onPlayerTimerFinish, this, std::placeholders::_1);

	// Insert timer
	player_timers.insert({ player->id,  player_timer });

	// Set player callbacks
	player->onStrafe = std::bind(&TutorialLevel::onPlayerStrafe, this);
	player->onJump = std::bind(&TutorialLevel::onPlayerJump, this);
	player->onFastFall = std::bind(&TutorialLevel::onPlayerFastFall, this);
	player->cannon->onCannonShoot = std::bind(&TutorialLevel::onPlayerShootCannon, this);

	// Disable bow
	player->bow->isActive = false;
	player->rHand->isActive = false;
	player->pHand->isActive = false;

	// Create player cannon bar
	StatusBar* skill_bar = new StatusBar();
	skill_bar->setScale({ 48, 1 });
	skill_bar->setHealthPercentage(0);
	skill_bar->tLabel->setText("Cannon");
	skill_bar->tLabel->setTextColor(LevelOne::getColorMod(player->player_number));
	skill_bar->transform.position = LevelOne::getCanonnBarPosByPlayer(player);
	player->skill_bar = skill_bar;
	player->skill_bar->isActive = false;

	// Create AI player
	player2 = new PlayerAI(PlayerNumber::PLAYER_TWO);
	player2->player_team = Player::PlayerTeam::BLUE_TEAM;
	players.push_back(player2);
	player2->tower = tower2;
	player2->tLevel = this;
	player2->transform.position = Vector2<float>(LEVEL_WIDTH - 84, LEVEL_HEIGHT - 111 - 32);
	player2->isActive = false;

	if (PlayerAI* playerAI = dynamic_cast<PlayerAI*>(player2))
	{
		// Set movement boundaries
		BoxCollider* floor_collider = playerAI->tower->getComponent<BoxCollider>();
		int height = floor_collider->cHeight + floor_collider->offset.y + playerAI->tower->getAbsolutePosition().y;
		int lLimit = floor_collider->offset.x + playerAI->tower->getAbsolutePosition().x;
		int rLimit = floor_collider->cWidth + floor_collider->offset.x + playerAI->tower->getAbsolutePosition().x;

		playerAI->setBoundaries(Vector2<float>(lLimit, height), Vector2<float>(rLimit, height));

		// Set enemy player and tower
		playerAI->enemy = player;
		playerAI->enemy_tower = playerAI->enemy->tower;

		// Set stopped
		playerAI->isStopped = true; 
	}

	player_timer = new TimerObject(3 * 1000, player2->id);
	player_timer->timer->isOver = true;
	player_timer->callback = std::bind(&TutorialLevel::onPlayerTimerFinish, this, std::placeholders::_1);

	// Insert timer
	player_timers.insert({ player2->id,  player_timer });

	// Create player2 cannon bar
	skill_bar = new StatusBar();
	skill_bar->setScale({ 48, 1 });
	skill_bar->setHealthPercentage(0);
	skill_bar->tLabel->setText("Cannon");
	skill_bar->tLabel->setTextColor(LevelOne::getColorMod(player2->player_number));
	skill_bar->transform.position = LevelOne::getCanonnBarPosByPlayer(player2);
	player2->skill_bar = skill_bar;
	player2->skill_bar->isActive = false;

	// GUI
	Button* button = new Button(Texture("ExitButton.png", RendererManager::renderer));
	button->setScale(Vector2<float>(0.25f, 0.25f));
	button->transform.position = Vector2<float>(5, 5);
	button->setOnClickListener(std::bind(&TutorialLevel::exitButtonHandler, this));
	button->tLabel->isActive = false;

	// Winner banner
	winner_banner = new TextLabel();
	winner_banner->setTextScale(Vector2<float>(3, 3));
	winner_banner->setText("red team wins");
	winner_banner->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 30.f, 20.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	winner_banner->isActive = false;

	// DialogBox
	dialog_box = new DialogBox("elcome to Archers Duel! \n Complete this short tutorial \n Or whatever dude \n 1 \n 2 \n 3 \n 4");
	dialog_box->line_offset_y = 10.f;
	dialog_box->text_speed = 2;
	dialog_box->setText("This tutorial will teach what you need to know to play the game. \n On Archers Duel you play as this Archer.");
	dialog_box->hideText();
	dialog_box->transform.position = { LEVEL_WIDTH / 3, LEVEL_HEIGHT / 32 };
	dialog_box->onDisplayEnd = std::bind(&TutorialLevel::onDisplayDialog, this);

	// Continue button
	continue_button = new Button("ArrowButton.png");
	continue_button->setScale(Vector2<float>(1.f, 1.f));
	continue_button->setRelativePosition(Vector2<float>(0, 0), Vector2<float>(LEVEL_WIDTH, LEVEL_HEIGHT), { 2.5f, 12.f }, { ALIGN_FROM_RIGHT, ALIGN_FROM_TOP });
	continue_button->setOnClickListener(std::bind(&TutorialLevel::continueButtonHandler, this));
	continue_button->tLabel->isActive = false;
	continue_button->isActive = false;

	// Quest tracker
	quest_tracker = new QuestTracker("Quest", QuestTracker::QuestTrackerType::QUEST_TRACKER_UNIQUE_OBJECTIVE);
	quest_tracker->transform.parent = &dialog_box->transform;
	quest_tracker->transform.position = { 0.f, 20.f };
	//quest_tracker->setType(QuestTracker::QUEST_TRACKER_MULTI_OBJECTIVE, 5);
	quest_tracker->onComplete = std::bind(&TutorialLevel::onCompleteQuest, this);
	quest_tracker->isActive = false;

	// Insert dialogs
	phase_dialog_map.insert({ TUTORIAL_PHASE_INTRO, {"Welcome to Archers Duel!\n", "This tutorial will teach what you need to know to play the game. \nOn Archers Duel you play as one Archer. Like this one."} });
	phase_dialog_map.insert({ TUTORIAL_PHASE_STRAFE, {"We should start with some movement basics.\nYou can strafe by pressing A and D keys."} });
	phase_dialog_map.insert({ TUTORIAL_PHASE_JUMP, {"You can also jump by pressing W."} });
	phase_dialog_map.insert({ TUTORIAL_PHASE_FASTFALL, {"You can increase your falling speed.\nThis can be done by pressing S key while on the air."} });
	phase_dialog_map.insert({ TUTORIAL_PHASE_BOW, {"As any other archer, you have a bow and some arrows.\nYou can use them to shoot at your targets.", "Aim using your mouse. Press its left button to charge a shot.\nRelease close to the red zone so the arrow travels faster.", "Practice by shooting at these targets."} });
	phase_dialog_map.insert({ TUTORIAL_PHASE_POWERUP, {"In archers duel you win the game by destroying the enemy tower.\nAs long as you do it before they destroy yours.", "You can deal damage to the enemy tower just with your arrows.\nBut there exist ways to increase your damage output.", "These are Power ups.\nYou can use them by hitting them with one of your arrows.", "Once you get one of them, information about it will be displayed."} });
	phase_dialog_map.insert({ TUTORIAL_PHASE_CANNON, {"By doing damage to the enemy tower or hitting enemies\nwith your arrows your cannon bar is filled up.", "Once it is is full you can take out your cannon by\npressing the E key. Use your mouse to aim.",  "It can be placed wherever in your tower and once you press\nthe E key again, you will begin to charge a shot.", "While charging your cannon you cannot move. If you are stunned\nwhile charging a cannon you will not be able to use it again", "Practice by using it against the enemy tower.\nPress the E key to use your cannon."} });
	phase_dialog_map.insert({ TUTORIAL_PHASE_FINALE, {"This tutorial is now over. You should now be able to finish\nthe game by yourself. Good Luck!" } });
}

void TutorialLevel::onUpdate()
{
	switch (phase)
	{
	case TutorialLevel::TUTORIAL_PHASE_NULL:
	{		
		// Set Intro phase
		phase = TUTORIAL_PHASE_INTRO;
		
		// Temporary
		/*
		player->isActive = true;
		player->bow->isActive = true;
		player->rHand->isActive = true;
		player->pHand->isActive = true;
		player->canGainSkill = true;
		player->skill_bar->isActive = true;

		tower2->isActive = true;
		player2->isActive = true;
		player2->skill_bar->isActive = true;

		onChangeTutorialPhase(phase);
		*/
		// End Temporary


		// Get text to display
		std::string text = getNextText(phase, dialog_index);

		// Set next text
		dialog_box->setText(text);

		// Start displaying text
		dialog_box->hideText();
		dialog_box->displayText();

		// Activate button
		continue_button->isActive = true;
		continue_button->setEnabled(false);
		break;
	}
	case TutorialLevel::TUTORIAL_PHASE_INTRO:
		break;
	case TutorialLevel::TUTORIAL_PHASE_STRAFE:
		break;
	case TutorialLevel::TUTORIAL_PHASE_JUMP:
		break;
	default:
		break;
	}

	// Check if players are in a valid position
	for (auto player : players)
	{
		if (player)
		{
			if (!LevelOne::isPlayerPosValid(player))
			{
				// Reset player pos and deactivate
				LevelOne::resetPlayerPosition(player);
				player->jump_nav->speed = 1;
				player->isActive = false;

				// Remove player buffs
				while (!player->power_ups.empty())
				{
					PowerUp* power_up = player->power_ups.begin()->second;
					power_up->remove();
					player->removePowerUp(power_up);
				}

				// Activate timer
				player_timers.at(player->id)->timer->reset();
			}
		}
	}

}

// GUI methods

void TutorialLevel::exitButtonHandler()
{
	SceneManager::loadNextScene(new MainMenu());
}

void TutorialLevel::continueButtonHandler()
{
	// Get next text
	dialog_index = dialog_index + 1;
	std::string text = getNextText(phase, dialog_index);

	// Check if reached the final text
	if (!text.empty())
	{
		// Set next text
		dialog_box->setText(text);

		// Hide and display
		dialog_box->hideText();
		dialog_box->displayText();
	}
	// Final reached
	else
	{
		// Change phase and reset index
		phase = static_cast<TutorialLevel::TutorialLevelPhase>(phase + 1);
		dialog_index = 0;
		onChangeTutorialPhase(phase);

		// Get text with new phase
		text = getNextText(phase, dialog_index);

		// Set next text
		dialog_box->setText(text);

		// Hide and display
		dialog_box->hideText();
		dialog_box->displayText();
	}

	// Disable button
	continue_button->setEnabled(false);

	// Disable quest tracker
	quest_tracker->isActive = false;
}

void TutorialLevel::onDisplayDialog()
{
	// Enable button
	continue_button->setEnabled(true);

	switch (phase)
	{
	case TutorialLevel::TUTORIAL_PHASE_NULL:
		break;
	case TutorialLevel::TUTORIAL_PHASE_INTRO:
	{
		break;
	}
	case TutorialLevel::TUTORIAL_PHASE_STRAFE:
		// Enable movement
		player->mov_enabled = true;

		// Enable quest
		quest_tracker->setQuestText("Strafe");
		quest_tracker->setType(QuestTracker::QUEST_TRACKER_UNIQUE_OBJECTIVE);
		quest_tracker->isActive = true;

		// Disable button until quest completed
		continue_button->setEnabled(false);

		break;
	case TutorialLevel::TUTORIAL_PHASE_JUMP:
		// Enable quest
		quest_tracker->setQuestText("Jump");
		quest_tracker->setType(QuestTracker::QUEST_TRACKER_UNIQUE_OBJECTIVE);
		quest_tracker->isActive = true;

		// Disable button until quest completed
		continue_button->setEnabled(false);
		break;
	case TutorialLevel::TUTORIAL_PHASE_FASTFALL:
		// Enable quest
		quest_tracker->setQuestText("FastFall");
		quest_tracker->setType(QuestTracker::QUEST_TRACKER_UNIQUE_OBJECTIVE);
		quest_tracker->isActive = true;

		// Disable button until quest completed
		continue_button->setEnabled(false);
		break;
	case TutorialLevel::TUTORIAL_PHASE_BOW:
		switch(dialog_index)
		{
		case 0:
			// Set bow and hands active 
			player->bow->isActive = true;
			player->rHand->isActive = true;
			player->pHand->isActive = true;
			break;
		case 1:
			break;
		case 2:
			// Enable quest
			quest_tracker->setQuestText("Practice Targets");
			quest_tracker->setType(QuestTracker::QUEST_TRACKER_MULTI_OBJECTIVE, 5);
			quest_tracker->isActive = true;

			// Enable targets
			for (auto target : targets)
				target->isActive = true;

			// Disable button until quest completed
			continue_button->setEnabled(false);
			break;
		}
		break;
	case TutorialLevel::TUTORIAL_PHASE_POWERUP:
		switch (dialog_index)
		{
		case 1:
			// Enable powerUps
			for (auto pou : power_up_objects)
				pou->isActive = true;

			break;
		case 3:
			// First init
			if (!quest_tracker->isActive)
			{
				quest_tracker->setQuestText("Power Ups used");
				quest_tracker->setType(QuestTracker::QUEST_TRACKER_MULTI_OBJECTIVE, 6);
				quest_tracker->isActive = true;

				player->bow->isEnabled = true;

				// Disable button until quest completed
				continue_button->setEnabled(false);
			}
			// Display text of powerup
			else
			{
				// Enable powerUps
				for (auto pou : power_up_objects)
					pou->isActive = true;

				// Disable button until quest completed
				if (!quest_tracker->isComplete)
					continue_button->setEnabled(false);
			}
			break;
		default:
			break;
		}

		break;
	case TutorialLevel::TUTORIAL_PHASE_CANNON:

		if (dialog_index == 4)
		{
			player->increase_skill_points(100);

			quest_tracker->setQuestText("Shoot cannon");
			quest_tracker->setType(QuestTracker::QUEST_TRACKER_UNIQUE_OBJECTIVE);
			quest_tracker->isActive = true;

			// Disable until quest is completed
			continue_button->setEnabled(false);
		}

		break;
	case TutorialLevel::TUTORIAL_PHASE_FINALE:

		// Enable quest tracker
		quest_tracker->setQuestText("Finish the game");
		quest_tracker->setType(QuestTracker::QUEST_TRACKER_UNIQUE_OBJECTIVE);
		quest_tracker->isActive = true;

		// Enable enemy player
		if (PlayerAI* playerAI = dynamic_cast<PlayerAI*>(player2))
			playerAI->isStopped = false;

		// Disable until quest is completed
		continue_button->setEnabled(false);

		break;
	default:
		break;
	}
}

// Dialog
std::string TutorialLevel::getNextText(TutorialLevel::TutorialLevelPhase phase, Uint32 index)
{
	std::string text;

	// Check if dialog list exists
	if (phase_dialog_map.find(phase) != phase_dialog_map.end())
	{
		DialogList dList = phase_dialog_map[phase];

		// Check if index is valid
		if (index < dList.size())
		{
			text = dList[index];
		}
	}

	return text;
}

// Tutorial

void TutorialLevel::onChangeTutorialPhase(TutorialLevel::TutorialLevelPhase phase)
{
	switch (phase)
	{
	case TutorialLevel::TUTORIAL_PHASE_NULL:
		break;
	case TutorialLevel::TUTORIAL_PHASE_INTRO:
	{
	}
	case TutorialLevel::TUTORIAL_PHASE_STRAFE:
	{
		// Activate 
		player->isActive = true;
		player->mov_enabled = false;
		break;

		break;
	}
	case TutorialLevel::TUTORIAL_PHASE_JUMP:
		break;
	case TutorialLevel::TUTORIAL_PHASE_POWERUP:
		
		// Disable bow
		player->chargeBar->disable();
		player->bow->reset();
		player->bow->isEnabled = false;

		// Enable tower2 and player2
		tower2->isActive = true;
		player2->isActive = true;

		break;
	case TutorialLevel::TUTORIAL_PHASE_CANNON:

		// Set flag
		player->skill_bar->isActive = true;
		player->canGainSkill = true;

		// Set playr2 flag 
		player2->skill_bar->isActive = true;

		break;
	default:
		break;
	}
}

// Player Callbacks

void TutorialLevel::onPlayerStrafe()
{
	if (phase == TUTORIAL_PHASE_STRAFE)
	{
		if (dialog_box->isDisplayed)
			quest_tracker->updateProgress(1);
	}

	if (phase != TUTORIAL_PHASE_FINALE)
	{
		// Check boundaries
		if (BoxCollider* col = tower->getComponent<BoxCollider>())
		{
			// Player pos 
			auto player_pos = player->transform.position;
			if (auto player_col = player->getComponent<BoxCollider>())
			{
				auto player_col_w = player_col->getDimensions().x;

				if (player_pos.x < col->cLeft)
				{
					player->transform.position.x = col->cLeft;
				}
				else if (auto player_dim = (player_pos.x + player_col_w + player_col->offset.x); player_dim > col->cRight)
				{
					player->transform.position.x = col->cRight - (player_col_w + player_col->offset.x);
				}
			}
		}
	}
}

void TutorialLevel::onPlayerJump()
{
	if (phase == TUTORIAL_PHASE_JUMP)
	{
		if (dialog_box->isDisplayed)
			quest_tracker->updateProgress(1);
	}	
}

void TutorialLevel::onPlayerFastFall()
{
	if (phase == TUTORIAL_PHASE_FASTFALL)
	{
		if(dialog_box->isDisplayed)
			quest_tracker->updateProgress(1);
	}
}

void TutorialLevel::onPlayerShootCannon()
{
	if (phase == TUTORIAL_PHASE_CANNON)
	{
		if (dialog_box->isDisplayed)
			quest_tracker->updateProgress(1);
	}
}

// Other callbacks

void TutorialLevel::onPracticeTargetHit()
{
	if(phase == TUTORIAL_PHASE_BOW)
		quest_tracker->updateProgress(1);
}

void TutorialLevel::onPowerUpHit(PowerUp* pu)
{
	PowerUpType type = pu->type;

	std::string tooltip;
	switch(type)
	{ 
	case PowerUpType::POWER_UP_FIRE:
		tooltip = "Fire arrows deal extra damage to Towers\nIt deals bonus damage if every part of a tower is on fire";
		break;
	case PowerUpType::POWER_UP_HASTE:
		tooltip = "Every arrow you shoot while the power up is active\ndeal maximum damage and is not affected by gravity";
		break;
	case PowerUpType::POWER_UP_MIRROR:
		tooltip = "Every arrow you shoot while the power up is active\nis mirrored when it goes out of bounds";
		break;
	case PowerUpType::POWER_UP_SHIELD:
		tooltip = "While the shield is active you cannot be stunned\nor recieve knockback from enemy projectiles";
		break;
	case PowerUpType::POWER_UP_TRIPLE:
		tooltip = "You shoot three arrows instead of one while this is active";
		break;
	case PowerUpType::POWER_UP_THUNDERSTRIKE:
		tooltip = "When the power up timer ends, an arrow\nis shot from the sky at every enemy";
		break;
	default:
		tooltip = "Missing tooltip";
		break;
		break;
	}

	// Update quest
	quest_tracker->updateProgress(1);

	// Set dialog box text
	dialog_box->setText(tooltip);
	dialog_box->hideText();
	dialog_box->displayText();

	// Stop powerup timer and remove time label
	if (pu->type != POWER_UP_THUNDERSTRIKE)
	{
		pu->timer->isStopped = true;
		pu->time_display->time_label->isActive = false;
	}

	// Remove power up if avialable
	auto owner = pu->owner;
	PowerUpType type_to_remove = POWER_UP_DUMMY;
	for (auto pup : owner->power_ups)
	{
		auto active_type = pup.second->type;
		if (active_type != pu->type)
			type_to_remove = active_type;
	}
	if (type_to_remove != POWER_UP_DUMMY)
	{
		auto active_pu = owner->power_ups.at(type_to_remove);
		owner->removePowerUp(active_pu);

		active_pu->remove();
	}


	// Disable every power up and remove this one
	int index_to_remove = -1;
	for (int i = 0; i < power_up_objects.size(); i++)
	{
		auto pou = power_up_objects[i];

		// Set inactive
		pou->isActive = false;

		if (pou->type == pu->type)
			index_to_remove = i;
	}

	// Remove power up object from list
	if (index_to_remove != -1)
		power_up_objects.erase(power_up_objects.begin() + index_to_remove);
}

// Quest callbacks

void TutorialLevel::onCompleteQuest()
{
	switch (phase)
	{
	case TUTORIAL_PHASE_FASTFALL:
	case TUTORIAL_PHASE_JUMP:
	case TUTORIAL_PHASE_STRAFE:
	case TUTORIAL_PHASE_BOW:
	case TUTORIAL_PHASE_CANNON:
		continue_button->setEnabled(true);
		break;
	default:
		break;
	}
}

// Level One implementations
void TutorialLevel::setWinnerTeam(Player::PlayerTeam team)
{
	// Can only set winner during finale
	if (phase != TUTORIAL_PHASE_FINALE)
		return;

	// Check flag
	if (isGameOver)
		return;

	// Set flag
	isGameOver = true;

	// Complete quest
	if (phase == TUTORIAL_PHASE_FINALE)
		quest_tracker->updateProgress(1);

	// Update winner banner
	std::string winner_str = team == Player::PlayerTeam::RED_TEAM ? "red" : "blu";
	winner_banner->setText(winner_str + " team wins");
	winner_banner->isActive = true;

	// Set timer for finishing
	TimerObject* timer = new TimerObject(10 * 1000, 0);
	timer->callback = std::bind(&TutorialLevel::onEndGameTimerFininsh, this, 0);
}

void TutorialLevel::onPlayerTimerFinish(Uint32 flag)
{
	if (Player* player = dynamic_cast<Player*>(getGameObjectById(flag)))
	{
		player->isActive = true;
		player->isInmunne = true;
		player->tRenderer->setBlink(6, 60 * 3);
	}
}

void TutorialLevel::onEndGameTimerFininsh(Uint32 flag)
{
	SceneManager::loadNextScene(new MainMenu());
}

