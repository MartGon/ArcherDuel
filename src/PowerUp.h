#pragma once

#include "GameObject.h"

#include "Timer.h"

class Player;
class PowerUp;
class TextLabel;

enum PowerUpType
{
	POWER_UP_DUMMY,
	POWER_UP_SHIELD
};

namespace PowerUpUtil
{
	std::string getIconTexturePathByPowerUpType(PowerUpType type);
}

class PowerUpObject : public GameObject
{
public:
	// Constructor
	PowerUpObject(PowerUpType type);

	// Attributes
	PowerUpType type = POWER_UP_DUMMY;

	// Components
	TextureRenderer* background_renderer = nullptr;
	BoxCollider* bCollider = nullptr;
	RotatableBoxCollider* rCollider = nullptr;
	TimerComponent* rotation_timer = nullptr;
	TimerComponent* vanish_timer = nullptr;

	enum TimerEvent
	{
		TIMER_EVENT_ROTATION,
		TIMER_EVENT_VANISH
	};

	// GameObjects
	GameObject* icon = nullptr;
	TextureRenderer* icon_renderer = nullptr;

	// Methods
	PowerUp* getPowerUp(Player* target);

	// Overridede Methods
	void onTimerEnd(Uint8 flag) override;
	void onVanish() override;
};

class PowerUpTimeDisplay : public GameObject
{
public:
	// Constructors
	PowerUpTimeDisplay(PowerUp* power_up);

	// References
	PowerUp* power_up = nullptr;

	// Attributes
	PowerUpType type = POWER_UP_DUMMY;

	// Components
	TextureRenderer* icon_renderer = nullptr;

	// GameObjects
	TextLabel* time_label = nullptr;

	// Overrided Methods
	void onUpdate() override;
};

class PowerUp : public GameObject
{
public:
	// Constructor
	PowerUp(Player* owner, PowerUpType type, Uint32 duration);

	// Attributes
	PowerUpType type = POWER_UP_DUMMY;
	Player* owner = nullptr;
	Uint32 duration = 0;				// Duration in ms

	// Components
	TimerComponent* timer = nullptr;

	// Members
	PowerUpTimeDisplay* time_display = nullptr;

	// Methods 
	bool interruptDefaultAction();

	// Hooks
	virtual void onApply() {};
	virtual void onLoadArrow() {};

	virtual void onStun() {};
	virtual void onKnockback() {};

	// Overrided methods
	void onTimerEnd(Uint8 flag) override;

protected:
	void preventDefaultAction();

private:
	bool prevent_default_action = false;

};

class PowerUpShield : public PowerUp
{
public:
	// Constructor
	PowerUpShield(Player* owner);

	// Overrided methods
	void onStun() override;
	void onKnockback() override;
};