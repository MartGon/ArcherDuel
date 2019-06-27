#pragma once

#include "GameObject.h"

#include "Timer.h"

#include <functional>

class Player;
class PowerUp;
class TextLabel;
class Bow;
class Arrow;
class FireArrow;
class LevelOne;

enum PowerUpType
{
	POWER_UP_DUMMY,
	POWER_UP_SHIELD,
	POWER_UP_HASTE,
	POWER_UP_FIRE,
	POWER_UP_TRIPLE,
	POWER_UP_MIRROR,
	POWER_UP_THUNDERSTRIKE
};

enum PowerUpColor
{
	POWER_UP_COLOR_YELLOW,
	POWER_UP_COLOR_BROWN
};

namespace PowerUpUtil
{
	std::string getIconTexturePathByPowerUpType(PowerUpType type);
	std::string getPowerUpBackground(PowerUpColor color);
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
	void onTimerEnd(Uint32 flag) override;
	void onVanish() override;

	// Callbacks
	std::function<void(PowerUp*)> onHit;
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
	void remove();

	// Hooks

	// Before Hooks
	virtual void beforeLoadArrow(Arrow*& arrow_to_load) {};
	virtual void beforeShoot() {};
	virtual void beforePull() {};

	// On Hooks
	virtual void onApply() {};

	// Bow
	virtual void onLoadArrow(Arrow* arrow) {};
	virtual void onShoot(float& charge) {};
	virtual void onShootInputCheck(bool* result) {};
	virtual void onBowPull() {};
	virtual void onBowRelease() {};

	// Arrow
	virtual void onArrowOutofBounds(LevelOne* level, Arrow* arrow) {};

	// Status effect
	virtual void onStun() {};
	virtual void onKnockback() {};

	virtual void onRemove() {};
	virtual void onExtend() {};

	// After Hooks
	virtual void afterAimBow() {};
	virtual void afterShoot(float charge) {};

	// Overrided methods
	void onTimerEnd(Uint32 flag) override;

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

class PowerUpHaste : public PowerUp
{
public:
	// Constructor
	PowerUpHaste(Player* owner) : PowerUp(owner, POWER_UP_HASTE, 5 * 1000){};

	// Overrided methods
	void beforePull() override;

	void onApply() override;
	void onRemove() override;
	void onBowPull() override;
	void onShoot(float& charge) override;
	void onShootInputCheck(bool* result);
	void onBowRelease() override;
};

class PowerUpFire : public PowerUp
{
public:
	PowerUpFire(Player* owner) : PowerUp(owner, POWER_UP_FIRE, 15 * 1000) {};

	// Overrided Methods
	void beforeLoadArrow(Arrow*& arrow_to_load) override;

	void onApply() override;
	void onRemove() override;
};

class PowerUpTriple : public PowerUp
{
public:
	PowerUpTriple(Player* owner) : PowerUp(owner, POWER_UP_TRIPLE, 10 * 1000) {};

	// Members
	Arrow* uArrow = nullptr;
	Arrow* dArrow = nullptr;

	// Methods
	Arrow* createExtraArrow(Arrow* main_arrow, Bow* bow);
	void loadTwoExtraArrows(Bow* bow, Arrow* main_arrow);

	// Overrided methods
	void onApply() override;
	void onLoadArrow(Arrow* arrow) override;
	void afterShoot(float charge);
	void onRemove();

	void afterAimBow() override;
};

class PowerUpThunderStrike : public PowerUp
{
public:
	PowerUpThunderStrike(Player* owner) : PowerUp(owner, POWER_UP_THUNDERSTRIKE, 5 * 1000) {};

	// Overrided methods
	void onRemove();
	void onExtend();

	// Own methods
	void cast();
	void launchArrowAgainstTarget(Player* target, Arrow* arrow, float offset);
	Arrow* getArrow();
};

class PowerUpMirror : public PowerUp
{
public:
	PowerUpMirror(Player* owner) : PowerUp(owner, POWER_UP_MIRROR, 30 * 1000) {};

	// Overrided methods
	void onArrowOutofBounds(LevelOne* level, Arrow* arrow) override;
};