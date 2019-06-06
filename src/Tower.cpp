#include "Tower.h"
#include "HealthBar.h"
#include "Arrow.h"
#include "LevelOne.h"

#include "SceneManager.h"
#include "MainMenu.h"

// TODO - Hacer que la torre se destruya por partes, al destruir una se baja un nivel


Tower::Tower(RoofColor roofColor) : GameObject()
{
	// Texture Renderer
	MapRGB *colorKey = new MapRGB();
	colorKey->red = 48;
	colorKey->green = 96;
	colorKey->blue = 130;
	
	team = roofColor;

	if(roofColor)
		tRenderer = setComponent(new TextureRenderer("SymmetricTowerWRoof2Bluew.png", colorKey, 2));
	else
		tRenderer = setComponent(new TextureRenderer("SymmetricTowerWRoof2.png", colorKey, 2));

	// Colliders
	BoxCollider* collider = nullptr;

	// Floor Collider 
	collider = setComponent(new BoxCollider(112 - 16 + 1, 9));
	collider->offset = Vector2<float>(16, 68);
	//collider->debug = true;

	// Upper block Collider
	collider = setComponent(new BoxCollider(95 - 33 + 1, 99 - 77));
	collider->offset = Vector2<float>(33, 77);
	//collider->debug = true;

	// Second floor collider
	collider = setComponent(new BoxCollider(98 - 30 + 1, 106 - 98));
	collider->offset = Vector2<float>(30, 99);
	//collider->debug = true;

	// Middle Block collider
	collider = setComponent(new BoxCollider(95 - 33 + 1, 135 - 106));
	collider->offset = Vector2<float>(33, 107);
	//collider->debug = true;

	// Down block collider
	collider = setComponent(new BoxCollider(103 - 25 + 1, 159 - 135));
	collider->offset = Vector2<float>(25, 136);
	//collider->debug = true;

	// HealthBar
	healthBar = new StatusBar();
	healthBar->setScale(Vector2<float>(110, 1));
	healthBar->transform.parent = &this->transform;

	// HealtBar offset
	Vector2<float> hb_offset(7, 185);
	healthBar->transform.position = hb_offset;
	
	// HealthBar Text
	std::string text = roofColor ? "blue tower" : "red tower";
	healthBar->tLabel->setText(text);
}

Tower::Tower(LevelOne* level_one, RoofColor roofColor) : Tower(roofColor)
{
	this->level_one = level_one;
}

// Overrided Methods
void Tower::onColliderEnter(Collider * collider)
{
	//GameObject* owner = collider->gameObject;
}

// Own Methods
void Tower::takeDamage(float dmg)
{
	if (dynamic_cast<LevelOne*>(SceneManager::scene))
	{
		health = (health - dmg > 0) ? health - dmg : 0;

		// Set winner
		if (!health)
		{
			Player::PlayerTeam winner = team == ROOF_COLOR_RED ? Player::PlayerTeam::BLUE_TEAM : Player::PlayerTeam::RED_TEAM;
			level_one->setWinnerTeam(winner);
		}

		// Update healthbar
		float health_percent = health / max_health * 100;
		healthBar->setHealthPercentage(health_percent, true);
	}
}