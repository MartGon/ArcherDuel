#include "Tower.h"
#include "HealthBar.h"
#include "Arrow.h"
#include "LevelOne.h"

#include "Timer.h"

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

	// Create fires
	createFires();

	// Set fire combo audio player
	aPlayer = setComponent(new AudioPlayer());
	fire_combo = aPlayer->addAudioToList(aPlayer->loadAudioFile("fire_combo.wav"));
	aPlayer->setAudioToPlay(fire_combo);
	aPlayer->pause();

	// Set fire audio player
	fire_aPlayer = setComponent(new AudioPlayer("fire.wav"));
	fire_aPlayer->loop = true;
	fire_aPlayer->volume = 50;
	fire_aPlayer->pause();
}

Tower::Tower(LevelOne* level_one, RoofColor roofColor) : Tower(roofColor)
{
	this->level_one = level_one;
}

// Overrided Methods

void Tower::onUpdate()
{
	for (auto fire_pair : fires)
	{
		Fire* fire = fire_pair.second;
		if (fire->isActive)
		{
			fire_aPlayer->play();
			return;
		}
	}
	
	fire_aPlayer->pause();
}

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

void Tower::createFires()
{
	for (auto collider : getComponents<BoxCollider>())
	{
		if (collider->id == 3)
			continue;

		// Create fire
		Fire* fire = new Fire();
		fire->transform.parent = &this->transform;

		// Get collider center
		Vector2<float> center = collider->getCollisionCenter() + collider->offset;

		// Get Fire texture center
		Vector2<float> tCenter = Vector2<float>( - fire->tRenderer->texture.mWidth / 2,  - fire->tRenderer->texture.mHeight / 2 );

		// Set on center
		fire->transform.position = center + tCenter;

		// In collider one add some offset
		if (collider->id == 1)
			fire->transform.position.y -= 30;

		// Save on map
		fires.insert({ collider->id, fire });

		// Deactivate them
		fire->isActive = false;
	}
}

bool Tower::isCompletelyOnFire()
{
	bool result = false;
	for (auto collider : getComponents<BoxCollider>())
	{
		if (collider->id == 3)
			continue;

		Fire* fire = fires.at(collider->id);

		if (!fire->isActive)
			return result;
	}

	result = true;

	return result;
}

void Tower::disableFires()
{
	for (auto pair : fires)
	{
		Fire* fire = pair.second;
		fire->isActive = false;
	}
}

// Fire Class

Fire::Fire()
{
	// Set TextureRenderer
	MapRGB green;
	green.green = 255;
	tRenderer = setComponent(new TextureRenderer("Fire1.png", &green, 3));

	// Set animator
	animator = setComponent(new Animator("Fire", &green, tRenderer, 5));

	// Set idle animation
	idle = animator->currentAnimation;
	idle->loop = true;
	for (auto frame : idle->frames)
		frame->duration = 5;

	// Change scale
	setScale({ 1.5f, 1.5f });

	// Create timer
	timer = setComponent(new TimerComponent(15 * 1000));
}


void Fire::onTimerEnd(Uint8 flag)
{
	// Deactivate on timer end
	isActive = false;
}