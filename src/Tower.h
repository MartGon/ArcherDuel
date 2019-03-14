#include "GameObject.h"

class HealthBar;

class Tower : public GameObject
{
public:
	enum RoofColor 
	{
		ROOF_COLOR_RED,
		ROOF_COLOR_BLUE
	};

	Tower(RoofColor roofColor = ROOF_COLOR_RED);

	// Components
	TextureRenderer* tRenderer;

	// Inner GameObjects
	HealthBar* healthBar = nullptr;

	// Overrided methods
	void onColliderEnter(Collider* collider) override;

	// Own Methods
	void takeDamage(float dmg);

private:
	// Attributes
	float max_health = 100.0f;
	float health = 100.0f;
};