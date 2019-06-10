#include "GameObject.h"

#include <unordered_map>

class TimerComponent;
class LevelOne;
class StatusBar;
class Fire;

class Tower : public GameObject
{
public:
	enum RoofColor 
	{
		ROOF_COLOR_RED,
		ROOF_COLOR_BLUE
	};

	Tower(RoofColor roofColor = ROOF_COLOR_RED);
	Tower(LevelOne* level, RoofColor roofColor = ROOF_COLOR_RED);

	// Game
	LevelOne* level_one = nullptr;

	// Components
	TextureRenderer* tRenderer;

	// Inner GameObjects
	StatusBar* healthBar = nullptr;
	std::unordered_map<unsigned int, Fire*> fires;

	// Overrided methods
	void onColliderEnter(Collider* collider) override;

	// Own Methods
	void takeDamage(float dmg);
	void createFires();
	bool isCompletelyOnFire();
	void disableFires();

private:
	// Attributes
	RoofColor team = ROOF_COLOR_RED;
	float max_health = 500.0f;
	float health = 500.0f;
};

class Fire : public GameObject
{
public:
	Fire();

	// Components
	TextureRenderer* tRenderer = nullptr;
	TimerComponent* timer = nullptr;
	Animator* animator = nullptr;
	Animation* idle = nullptr;

	// Overrided methods
	void onTimerEnd(Uint8 flag);
};