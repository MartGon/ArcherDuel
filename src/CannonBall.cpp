#include "CannonBall.h"

#include "Navigator.h"

#include "Random.h"

#include "Timer.h"
#include "Tower.h"
#include "Player.h"

CannonBall::CannonBall()
{
	// Texture Renderer
	MapRGB *colorKey = new MapRGB();
	colorKey->green = 255;
	tRenderer = setComponent(new TextureRenderer("Cannon_ball.png", colorKey, 254));

	// Navigator
	nav = setComponent(new Navigator(Vector2<float>(0, 0), 1));

	// Collider
	bCollider = setComponent(new BoxCollider(tRenderer->texture));
	//bCollider->debug = true;

	// Damage text label
	dmg_label = new TextLabel();
	dmg_label->transform.parent = &transform;
	dmg_label->transform.position = Vector2<float>(0, -3);
	dmg_label->setText("0");
	dmg_label->setScale(Vector2<float>(0.5, 0.5));
	dmg_label->isActive = false;
}

void CannonBall::onColliderEnter(Collider* collider)
{
	// Disable nav and collisions
	nav->isEnabled = false;
	bCollider->isEnabled = false;

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

		// We don't wait for timer
		wait_timer = false;
	}
	else if (Tower* tower = dynamic_cast<Tower*>(collider->gameObject))
	{
		// Deal dmg to the tower
		tower->takeDamage(dmg);

		// Set dmg label
		dmg_label->setText(std::to_string((int)dmg));

		// Enable dmg label
		dmg_label->isActive = true;

		// Set label to vanish
		for (auto dmg_tRenderer : dmg_label->getComponents<TextureRenderer>())
			dmg_tRenderer->isVanishing = true;

		// Play sound

	}
	else if (Arrow* arrow = dynamic_cast<Arrow*>(collider->gameObject))
	{
		// Set both arrows to vanish
		tRenderer->isVanishing = true;
		arrow->tRenderer->isVanishing = true;

		// We don't wait for timer
		wait_timer = false;
	}

	// Return if we dont have to create a timer
	if (!wait_timer)
		return;

	// Set timer to vanish arrow
	if (!timer)
		timer = setComponent(new TimerComponent(15 * 1000));
}

void CannonBall::onTimerEnd(Uint8 flag)
{
	tRenderer->isVanishing = true;
}

void CannonBall::onVanish()
{
	destroy();
	dmg_label->destroy();
}