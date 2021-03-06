#include "GameObject.h"

#include "AudioPlayer.h"

#include <unordered_map>

class TimerComponent;
class LevelOne;
class TutorialLevel;
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
	TutorialLevel* tLevel = nullptr;

	// Components
	AudioPlayer* aPlayer = nullptr;
	AudioPlayer* fire_aPlayer = nullptr;
	TextureRenderer* tRenderer = nullptr;

	// Audios
	int fire_combo = 0;

	// Inner GameObjects
	StatusBar* healthBar = nullptr;
	std::unordered_map<unsigned int, Fire*> fires;

	// Overrided methods
	void onUpdate() override;
	void onColliderEnter(Collider* collider) override;

	// Own Methods
	void takeDamage(float dmg);
	void createFires();
	bool isCompletelyOnFire();
	void disableFires();

	double max_health = 500.0f;
	double health = 500.0f;

private:
	// Attributes
	RoofColor team = ROOF_COLOR_RED;
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
	void onTimerEnd(Uint32 flag);
};