#include "Arrow.h"
#include "Player.h"
#include "LevelOne.h"
#include "Tower.h"
#include "Random.h"
#include "Timer.h"
#include "Button.h"
#include "PowerUp.h"
#include "SceneManager.h"

Arrow::Arrow()
{
	// Setting components
	name = "arrow";
	template_id = 1;

	// Texture Renderer
	MapRGB *colorKey = new MapRGB();
	colorKey->blue = 0xFF;
	tRenderer = setComponent(new TextureRenderer("ArrowTrim.png", colorKey, 254));

	// Set scale
	transform.scale = Vector2<float>(0.5f, 0.5f);

	// Navigator
	nav = setComponent(new Navigator());
	nav->isEnabled = false;
	nav->isKinematic = true;
	nav->acceleration.y = -0.03f;

	// Rotatable Box Collider
	rotCollider = setComponent(new RotatableBoxCollider(Vector2<int>(0, 0), Vector2<int>(0, 3 * transform.scale.y), 
		Vector2<int>(14 * transform.scale.x, 0), Vector2<int>(14 * transform.scale.x, 3 * transform.scale.y)));
	rotCollider->isEnabled = false;
    //rotCollider->debug = true;
	
	// Damage text label
	dmg_label = new TextLabel();
	dmg_label->transform.parent = &transform;
	dmg_label->transform.position = Vector2<float>(0, -3);
	dmg_label->setText("0");
	dmg_label->setScale(Vector2<float>(0.5, 0.5));

	// Get Texture Renderer
	dmg_tRenderer = dmg_label->getComponent<TextureRenderer>();

	// Disable until the arrow is pinned
	dmg_label->isActive = false;

	// Audio player
	aPlayer = setComponent(new AudioPlayer("arrow-impact-building1.wav"));
	audio_impact_building_2 = aPlayer->addAudioToList(aPlayer->loadAudioFile("arrow-impact-building2.wav"));
	audio_impact_player = aPlayer->addAudioToList(aPlayer->loadAudioFile("arrow-impact-player.wav"));
	aPlayer->pause();

	// Network
	isNetworkStatic = false;
}

// Hooks

void Arrow::onColliderEnter(Collider* collider)
{
	// Disable nav and collisions
	nav->isEnabled = false;
	rotCollider->isEnabled = false;

	// Create timer flag
	bool wait_timer = true;

	if (Player* player = dynamic_cast<Player*>(collider->gameObject))
	{
		// Set player to new parent
		transform.position = getAbsolutePosition() - player->getAbsolutePosition();
		transform.parent = &player->transform;
	
		// Set to vanish
		tRenderer->isVanishing = true;

		// Check inmunnity
		if (player->isInmunne)
			return;

		// Stop mov nav
		player->mov_nav->setDirection(Vector2<float>(0, 0));

		// Stun player hit
		player->stun(120);

		// Knock player back
		player->knockback(nav->getDirection(), nav->speed * 0.65);

		// Play sound effect
		aPlayer->setAudioToPlay(audio_impact_player);
		aPlayer->play();

		// Increase owner skill points
		if (owner)
		{
			if (owner != player)
				owner->increase_skill_points(25);
		}

		// We don't wait for timer
		wait_timer = false;
	}
	else if (Tower* tower = dynamic_cast<Tower*>(collider->gameObject))
	{
		// OnImpact hook
		onImpactGameObject(collider);

		// Deal dmg to the tower
		tower->takeDamage(nav->speed);

		// Set dmg label
		dmg_label->setText(std::to_string((int)std::round(nav->speed)));

		// Enable dmg label
		dmg_label->isActive = true;

		// Set label to vanish
		for(auto dmg_tRenderer : dmg_label->getComponents<TextureRenderer>())
			dmg_tRenderer->isVanishing = true;

		// Notify
		//std::cout << "This arrow did " << nav->speed << "dmg \n";

		// Increase owner skill points
		if(owner)
			owner->increase_skill_points(nav->speed / 2);

		// Play random building sound
		int index = Random::getRandomUniformInteger(audio_impact_building_1, audio_impact_building_2);
		aPlayer->setAudioToPlay(index);
		aPlayer->play();
	}
	else if (Arrow* arrow = dynamic_cast<Arrow*>(collider->gameObject))
	{
		// Increase owner skill points
		if (arrow->owner != owner)
		{
			// Set both arrows to vanish
			tRenderer->isVanishing = true;
			arrow->tRenderer->isVanishing = true;

			owner->increase_skill_points(30);
		}
		else
		{
			// We don't wait for timer
			wait_timer = false;

			// Re-enable components
			nav->isEnabled = true;
			rotCollider->isEnabled = true;
		}
	}
	else if (Button* button = dynamic_cast<Button*>(collider->gameObject))
	{
		// Only during Menu
		if(!SceneManager::scene->isOnline())
			button->click();

		// We don't wait for timer
		wait_timer = false;
		
		// Start vanishing
		tRenderer->isVanishing = true;
	}
	else if (PowerUpObject* power_up_object = dynamic_cast<PowerUpObject*>(collider->gameObject))
	{
		// Disable this arrow
		this->isActive = false;
		this->onVanish();

		// Disable power up and destroy
		power_up_object->onVanish();

		// Play sound effect

		// Give effect to owner
		if(owner)
			owner->addPowerUp(power_up_object->getPowerUp(owner));
	}

	// Return if we dont have to create a timer
	if (!wait_timer)
		return;

	// Set timer to vanish arrow
	if (!timer)
	{
		timer = setComponent(new TimerComponent(15 * 1000));
	}

}

void Arrow::afterMove()
{
 	transform.zRotation = nav->getDirection().getAngle();

	// Check for valid position
	if (!LevelOne::isObjectPositionValid(this))
	{
		if (owner)
		{
			if (LevelOne* level = owner->level)
			{
				bool interrupt = owner->powerUpHookTemplate(&PowerUp::onArrowOutofBounds, level, this);

				if (interrupt)
					return;
			}
		}

		onVanish();
	}
}

void Arrow::onUpdate()
{
	/*
	if (nav)
		if (nav->isEnabled)
			std::cout << "Arrow position is" << transform.position << "in frame " << SceneManager::scene->frame_count++ << "\n";
	*/
}

void Arrow::onVanish()
{
	// Set to destroy
	destroy();
	dmg_label->destroy();
}

void Arrow::onTimerEnd(Uint8 flag)
{
	tRenderer->isVanishing = true;
}
// TODO - Los colliders de la flecha no aparecen ir acordes al movimiento. HACK: Poner el navigator antes del collider

// Class FireArrow

FireArrow::FireArrow() : Arrow()
{
	// Create animator
	MapRGB mapRGB;
	mapRGB.green = 255;
	animator = setComponent(new Animator("FireArrowIdle", &mapRGB, tRenderer, 5));
	
	// Set animation
	idle = animator->currentAnimation;
	idle->loop = true;
	for (auto frame : idle->frames)
	{
		frame->duration = 5;
	}
}

// Overrided Methods

void FireArrow::onImpactGameObject(Collider* col)
{
	// Increase arrow dmg by 50%
	nav->speed = nav->speed * 1.5f;

	// Tower case
	if (GameObject* go = col->gameObject)
	{
		if (Tower* tower = dynamic_cast<Tower*>(go))
		{
			if (tower->fires.find(col->id) == tower->fires.end())
				return;

			// Enable fire
			Fire* fire = tower->fires.at(col->id);
			fire->isActive = true;

			// Activate timer
			fire->timer->reset();

			// Check if it's last fire block
			if (tower->isCompletelyOnFire())
			{
				// Now take bonus dmg
				tower->takeDamage(100);

				// Disable fires
				tower->disableFires();

				// Increase skill points
				if (owner)
					owner->increase_skill_points(25);
			}
		}
	}
}