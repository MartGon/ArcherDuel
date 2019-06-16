#include "PowerUp.h"
#include "Player.h"
#include "Bow.h"
#include "Arrow.h"
#include "TextLabel.h"
#include "LevelOne.h"

#include <sstream>
#include <iomanip>

// PowerUpUtil Namespace

std::string PowerUpUtil::getIconTexturePathByPowerUpType(PowerUpType type)
{
	// Get texture path
	std::string path;

	switch (type)
	{
	case POWER_UP_SHIELD:
		path = "PowerUp_Shield.png";
		break;
	case POWER_UP_HASTE:
		path = "PowerUp_Haste.png";
		break;
	case POWER_UP_FIRE:
		path = "PowerUp_Fire.png";
		break;
	case POWER_UP_TRIPLE:
		path = "PowerUp_DoubleArrow.png";
		break;
	case POWER_UP_THUNDERSTRIKE:
		path = "PowerUp_AirStrike.png";
		break;
	case POWER_UP_MIRROR:
		path = "PowerUp_Mirror.png";
		break;
	case POWER_UP_DUMMY:
	default:
		path = "UndefinedPowerUp";
		break;
	}

	return path;
}

// PowerUpObject Class

// Constructors

PowerUpObject::PowerUpObject(PowerUpType type)
{
	// Set type
	this->type = type;

	// Background renderer
	MapRGB *colorKey = new MapRGB();
	colorKey->green = 255;
	background_renderer = setComponent(new TextureRenderer("PowerUp_Background_yellow.png", colorKey, 254));

	// Icon
	icon = new GameObject();
	std::string icon_path = PowerUpUtil::getIconTexturePathByPowerUpType(type);
	icon_renderer = icon->setComponent(new TextureRenderer(icon_path.c_str(), colorKey, 254));
	icon->transform.parent = &this->transform;
	icon->setScale({ 1.5f, 1.5f });

	// BoxCollider
	bCollider = setComponent(new BoxCollider(16, 16));
	bCollider->offset = { 3, 3 };
	//bCollider->debug = true;

	// RotatableBoxCollider
	rCollider = setComponent(new RotatableBoxCollider(bCollider));
	rCollider->offset = { 3, 3 };
	rCollider->rotation_offset = 45;
	//rCollider->debug = true;

	// Set rotation timer
	rotation_timer = setComponent(new TimerComponent(0.5 * 1000, TIMER_EVENT_ROTATION));

	// Set vanish timer
	vanish_timer = setComponent(new TimerComponent(15 * 1000, TIMER_EVENT_VANISH));
}

// Methods

PowerUp* PowerUpObject::getPowerUp(Player* target)
{
	PowerUp* power_up = nullptr;

	switch (type)
	{
	case POWER_UP_SHIELD:
		power_up = new PowerUpShield(target);
		break;
	case POWER_UP_HASTE:
		power_up = new PowerUpHaste(target);
		break;
	case POWER_UP_FIRE:
		power_up = new PowerUpFire(target);
		break;
	case POWER_UP_TRIPLE:
		power_up = new PowerUpTriple(target);
		break;
	case POWER_UP_MIRROR:
		power_up = new PowerUpMirror(target);
		break;
	case POWER_UP_THUNDERSTRIKE:
		power_up = new PowerUpThunderStrike(target);
		break;
	case POWER_UP_DUMMY:
	default:
		break;
	}

	return power_up;
}

// Overrided Methods

void PowerUpObject::onTimerEnd(Uint8 flag)
{
	if (flag == TIMER_EVENT_ROTATION)
	{
		// Update rotation
		if (transform.zRotation == 0)
		{
			// Keep rCollider in same rotation
			rCollider->rotation_offset = 0;

			// Rotate 45
			transform.zRotation = 45;
		}
		else
		{
			// Revert
			rCollider->rotation_offset = 45;
			transform.zRotation = 0;
		}

		// Reset Timer
		rotation_timer->reset();
	}
	else if (flag == TIMER_EVENT_VANISH)
	{
		// Start to Vanish
		background_renderer->isVanishing = true;
		icon_renderer->isVanishing = true;
	}
}

void PowerUpObject::onVanish()
{
	this->destroy();
	icon->destroy();
}

// PowerUpTimeDisplay

// Constructor

PowerUpTimeDisplay::PowerUpTimeDisplay(PowerUp* power_up)
{
	// Set type
	this->type = power_up->type;

	// Set references
	this->power_up = power_up;

	// Create icon renderer
	MapRGB *colorKey = new MapRGB();
	colorKey->green = 255;
	std::string icon_path = PowerUpUtil::getIconTexturePathByPowerUpType(type);
	icon_renderer = setComponent(new TextureRenderer(icon_path.c_str(), colorKey, 254));
	icon_renderer->render_offset = { -4, -2};

	// Create TextLabel and set this as parent
	time_label = new TextLabel();
	time_label->setText("0.0");
	time_label->transform.parent = &this->transform;
	time_label->setScale({ 0.5f, 0.5f });
	time_label->transform.position = { -3, 9 };

	// Change scale
	setScale({ 0.5f, 0.5f });
}

// Overrided Methods

void PowerUpTimeDisplay::onUpdate()
{
	// Set time_display text to buff duration
	Uint32 remaining_ms = power_up->timer->getTimeRemaining();
	double remaining_sec = remaining_ms / (double)1000;

	// Limit to two decimal values
	std::ostringstream stream; 
	stream << std::setprecision(2) << std::fixed << remaining_sec;
	std::string num_text = stream.str();

	// Output
	time_label->setText(num_text);
}

// PowerUp class

// Constructor

PowerUp::PowerUp(Player* owner, PowerUpType type, Uint32 duration)
{
	// Set attributes
	this->owner = owner;
	this->type = type;
	this->duration = duration;

	// Create timer
	timer = setComponent(new TimerComponent(duration));

	// Create TimerDisplay
	time_display = new PowerUpTimeDisplay(this);
}

void PowerUp::remove()
{
	// Destroy and display
	this->destroy();
	time_display->destroy();
	time_display->time_label->destroy();
}

void PowerUp::preventDefaultAction()
{
	prevent_default_action = true;
}

bool PowerUp::interruptDefaultAction()
{
	bool action = prevent_default_action;
	prevent_default_action = false;
	return action;
}

// Overrided Methods

void PowerUp::onTimerEnd(Uint8 flag)
{
	// Remove from owner
	owner->removePowerUp(this);

	// Destroy
	remove();
}

// PowerUpShield class

PowerUpShield::PowerUpShield(Player* owner) : PowerUp(owner, POWER_UP_SHIELD, 15 * 1000)
{

}

// Overrided methods

void PowerUpShield::onStun()
{
	preventDefaultAction();
}

void PowerUpShield::onKnockback()
{
	preventDefaultAction();
}

// PowerUpHaste class

void PowerUpHaste::beforePull()
{
	if (Bow* bow = owner->bow)
	{
		// Set to instant_cast
		bow->instant_cast = true;

		// Prevent enable charge bar
		preventDefaultAction();
	}
}

void PowerUpHaste::onApply()
{
	// Increase shoot speed
	if (Bow* bow = owner->bow)
	{
		// Increase animation speed
		bow->animator->frame_speed = 2.5f;
	}

	// AI measure
	if (PlayerAI* ai = dynamic_cast<PlayerAI*>(owner))
	{
		if (!ai->isChargingCannon)
			ai->draw_frames = 1;
	}
}

void PowerUpHaste::onRemove()
{
	// Revert shoot speed
	if (Bow* bow = owner->bow)
	{
		bow->animator->frame_speed = 1;
	}
}

void PowerUpHaste::onBowPull()
{
	// Prevent Pull sound effect
	preventDefaultAction();
}

void PowerUpHaste::onShoot(float& charge)
{
	charge = 6;
}

void PowerUpHaste::onShootInputCheck(bool* result)
{
	InputManager* iMgr = InputManager::get();
	*result = iMgr->isMousePressed(SDL_BUTTON_LEFT, owner->network_owner);
}

void PowerUpHaste::onBowRelease()
{
	if (Bow* bow = owner->bow)
	{
		if (AudioPlayer* aPlayer = bow->aPlayer)
		{
			// Play special audio
			aPlayer->setAudioToPlay(bow->rel_special_index);
			aPlayer->play();

			// Prevent original audio to play
			preventDefaultAction();
		}
	}
}

// PowerUp Triple Class

// Methods
Arrow* PowerUpTriple::createExtraArrow(Arrow* arrow, Bow* bow)
{
	// Create extra arrow
	Arrow* extra_arrow = nullptr;

	// Check arrow type
	if (dynamic_cast<FireArrow*>(arrow))
		extra_arrow = new FireArrow();
	else
		extra_arrow = new Arrow();

	// Copy refs
	extra_arrow->bow = bow;
	extra_arrow->owner = bow->owner;

	// Copy transform
	extra_arrow->transform.position = arrow->transform.position;
	extra_arrow->transform.rotationCenter = new Vector2<int>();
	extra_arrow->transform.parent = &bow->transform;

	return extra_arrow;
}

void PowerUpTriple::loadTwoExtraArrows(Bow* bow, Arrow* main_arrow)
{
	// AbsCenter
	Vector2<int> absCenter;

	// Create upper arrow and set position
	uArrow = createExtraArrow(main_arrow, bow);
	uArrow->transform.position.y += 3;

	// Set rotation Center
	absCenter = bow->getAbsoluteRotationCenter();
	uArrow->setAbsoluteRotationCenter(absCenter);

	// Create downer arrow and set position
	dArrow = createExtraArrow(main_arrow, bow);
	dArrow->transform.position.y -= 3;
	
	// Set rotation Center
	absCenter = bow->getAbsoluteRotationCenter();
	dArrow->setAbsoluteRotationCenter(absCenter);
}

// Overrided Methods
void PowerUpTriple::onApply()
{
	if (Bow* bow = owner->bow)
			if(Arrow* arrow = bow->arrow)
				loadTwoExtraArrows(bow, arrow);
}

void PowerUpTriple::onLoadArrow(Arrow* arrow)
{
	if (Bow* bow = owner->bow)
	{
		loadTwoExtraArrows(bow, arrow);
	}
}

void PowerUpTriple::afterShoot(float charge)
{
	if (Bow* bow = owner->bow)
	{
		// Return if arrows don't exist
		if (!uArrow)
			return;
		if (!dArrow)
			return;

		// Launch both arrows
		bow->launchArrow(uArrow, charge);
		bow->launchArrow(dArrow, charge);
		
		// Set to nullptr
		uArrow = nullptr;
		dArrow = nullptr;
	}
}

void PowerUpTriple::onRemove()
{
	if (Bow* bow = owner->bow)
	{
		// Remove loaded arrows
		if (uArrow) 
		{
			//uArrow->isActive = false;
			uArrow->onVanish();
			uArrow = nullptr;
		}

		if (dArrow)
		{
			//dArrow->isActive = false;
			dArrow->onVanish();
			dArrow = nullptr;
		}
	}
}

void PowerUpTriple::afterAimBow()
{
	if (Bow* bow = owner->bow)
	{
		if (uArrow)
		{
			uArrow->transform.zRotation = bow->transform.zRotation;

			if (bow->arrow)
			{
				uArrow->transform.position.x = bow->arrow->transform.position.x;
				uArrow->setAbsoluteRotationCenter(bow->getAbsoluteRotationCenter());
			}
		}
		if (dArrow)
		{
			dArrow->transform.zRotation = bow->transform.zRotation;

			if (bow->arrow)
			{
				dArrow->transform.position.x = bow->arrow->transform.position.x;
				dArrow->setAbsoluteRotationCenter(bow->getAbsoluteRotationCenter());
			}
		}

	}
}

// PowerUpFire Class

// Overrided Methods

void PowerUpFire::beforeLoadArrow(Arrow*& arrow_to_load)
{
	// Arrow_to_load should be nulltpr
	if (!arrow_to_load)
	{
		// Create fire Arrow instead
		arrow_to_load = new FireArrow();
	}
}

void PowerUpFire::onApply()
{
	if (Bow* bow = owner->bow)
	{
		if (Arrow* arrow = bow->arrow)
		{
			// Do nothing if the arrow is already on fire
			if (dynamic_cast<FireArrow*>(arrow))
				return;

			// Create FireArrow
			FireArrow* fire_arrow = new FireArrow();

			// Destroy previous arrow
			bow->arrow->onVanish();

			// Set bow's arrow pointing to new fire_arrow
			bow->arrow = fire_arrow;

			// Prepare arrow
			bow->prepareArrow(fire_arrow);
		}
	}

	// Check for triple bonus
	if (owner->power_ups.find(POWER_UP_TRIPLE) != owner->power_ups.end())
	{
		if (PowerUpTriple* triple = dynamic_cast<PowerUpTriple*>(owner->power_ups[POWER_UP_TRIPLE]))
		{
			// Destroy previous arrows
			if (triple->uArrow)
			{
				triple->uArrow->onVanish();
				triple->uArrow = nullptr;
			}
			if (triple->dArrow)
			{
				triple->dArrow->onVanish();
				triple->uArrow = nullptr;
			}

			// Create new arrows
			if (Bow* bow = owner->bow)
			{
				if (Arrow* arrow = bow->arrow)
					triple->loadTwoExtraArrows(bow, arrow);
			}
		}
	}
}

void PowerUpFire::onRemove()
{
	if (Bow* bow = owner->bow)
	{
		if (Arrow* arrow = bow->arrow)
		{
			if (dynamic_cast<FireArrow*>(arrow))
			{
				// Destroy previous arrow
				bow->arrow->onVanish();

				// Set bow's arrow pointing to new fire_arrow
				bow->arrow = new Arrow();

				// Prepare arrow
				bow->prepareArrow(bow->arrow);
			}
		}
	}

	// Check for triple buff
	if (owner->power_ups.find(POWER_UP_TRIPLE) != owner->power_ups.end())
	{
		if (PowerUpTriple* triple = dynamic_cast<PowerUpTriple*>(owner->power_ups[POWER_UP_TRIPLE]))
		{
			// Destroy previous arrows
			if (triple->uArrow)
			{
				triple->uArrow->onVanish();
				triple->uArrow = nullptr;
			}
			if (triple->dArrow)
			{
				triple->dArrow->onVanish();
				triple->uArrow = nullptr;
			}

			// Create new arrows
			if (Bow* bow = owner->bow)
			{
				if (Arrow* arrow = bow->arrow)
					triple->loadTwoExtraArrows(bow, arrow);
			}
		}
	}
}

// Class PowerUpMirror

// Overrided methods

void PowerUpMirror::onArrowOutofBounds(LevelOne* level, Arrow* arrow)
{
	// Prevent arrow vanish
	preventDefaultAction();

	// Get arrow pos
	auto pos = arrow->transform.position;

	// Reposition
	if (pos.x > level->LEVEL_WIDTH)
		arrow->transform.position.x = -15;
	else
		arrow->transform.position.x = level->LEVEL_WIDTH;
}

// Class PowerUpThunderStrike

void PowerUpThunderStrike::onRemove()
{
	cast();
}

void PowerUpThunderStrike::onExtend()
{
	cast();
	timer->due_date = timer->getCurrentTime() + timer->delay;
}

// Own method

void PowerUpThunderStrike::cast()
{
	if (LevelOne* level = owner->level)
	{
		// Find targets
		for (auto player : level->players)
		{
			// Launch arrow against this target if different team
			if (player->player_team != owner->player_team)
			{
				// Create arrow and launch
				Arrow* arrow = getArrow();
				launchArrowAgainstTarget(player, arrow, 0);

				// Check for triple buff
				if (owner->power_ups.find(POWER_UP_TRIPLE) != owner->power_ups.end())
				{
					// Left Arrow
					Arrow* lArrow = getArrow();
					launchArrowAgainstTarget(player, lArrow, -5);

					// Right Arrow
					Arrow* rArrow = getArrow();
					launchArrowAgainstTarget(player, rArrow, 5);
				}

			}
		}
	}
}

void PowerUpThunderStrike::launchArrowAgainstTarget(Player* target, Arrow* arrow, float offset)
{
	// Get target pos
	auto target_pos = target->transform.position;

	// Set postion
	arrow->transform.position = Vector2<float>(target_pos.x + offset, -10);

	// Get dir Vector
	Vector2<float> dir = target_pos - arrow->transform.position;

	// Setting up arrow navigator
	Navigator* nav = arrow->nav;
	nav->setDirection(dir);
	nav->speed = 1.75f * 6;
	nav->isEnabled = true;

	// Enable collider
	arrow->rotCollider->isEnabled = true;

	// Set arrow owner
	arrow->owner = owner;
}

Arrow* PowerUpThunderStrike::getArrow()
{
	Arrow* arrow = nullptr;

	// Create fire arrow if playes has buff
	if (owner->power_ups.find(POWER_UP_FIRE) != owner->power_ups.end())
		arrow = new FireArrow();
	else
		arrow = new Arrow();

	return arrow;
}