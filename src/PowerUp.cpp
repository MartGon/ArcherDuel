#include "PowerUp.h"
#include "Player.h"
#include "TextLabel.h"

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
	preventDefaultAction();
}

void PowerUpHaste::onShoot(float& charge)
{
	charge = 6;
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