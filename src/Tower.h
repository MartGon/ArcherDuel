#include "GameObject.h"

class LevelOne;
class StatusBar;

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

	// Overrided methods
	void onColliderEnter(Collider* collider) override;

	// Own Methods
	void takeDamage(float dmg);

private:
	// Attributes
	RoofColor team = ROOF_COLOR_RED;
	float max_health = 500.0f;
	float health = 500.0f;
};