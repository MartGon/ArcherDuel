#include "Arrow.h"
#include "Player.h"
#include "LevelOne.h"
#include "Tower.h"
#include "Random.h"
#include "Timer.h"
#include "Button.h"

Arrow::Arrow()
{
	// Setting components
	name = "arrow";

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
}

// Hooks

void Arrow::onColliderEnter(Collider* collider)
{
	printf("Arrow Collision\n");

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

		// Stun player hit
		player->stun(120);

		// Knock player back
		player->knockback(nav->getDirection(), nav->speed * 0.65);

		// Play sound effect
		aPlayer->setAudioToPlay(audio_impact_player);
		aPlayer->play();

		// We don't wait for timer
		wait_timer = false;
	}
	else if (Tower* tower = dynamic_cast<Tower*>(collider->gameObject))
	{
		// Deal dmg to the tower
		tower->takeDamage(nav->speed);

		// Set dmg label
		dmg_label->setText(std::to_string((int)nav->speed));

		// Enable dmg label
		dmg_label->isActive = true;

		// Set label to vanish
		dmg_tRenderer->isVanishing = true;

		// Notify
		//std::cout << "This arrow did " << nav->speed << "dmg \n";

		// Play random building sound
		int index = Random::getRandomUniformInteger(audio_impact_building_1, audio_impact_building_2);
		aPlayer->setAudioToPlay(index);
		aPlayer->play();
	}
	else if (Arrow* arrow = dynamic_cast<Arrow*>(collider->gameObject))
	{
		// Set both arrows to vanish
		tRenderer->isVanishing = true;
		arrow->tRenderer->isVanishing = true;

		// We don't wait for timer
		wait_timer = false;
	}
	else if (Button* button = dynamic_cast<Button*>(collider->gameObject))
	{
		button->click();

		// We don't wait for timer
		wait_timer = false;
	}

	if (!wait_timer)
		return;

	// Set timer to vanish arrow
	Timer* timer = new Timer(15 * 1000, this, nullptr);
}

void Arrow::afterMove()
{
	transform.zRotation = nav->getDirection().getAngle();

	// Check for valid position
	if (!LevelOne::isObjectPositionValid(this))
		onVanish();
}

void Arrow::onUpdate()
{
}

void Arrow::onVanish()
{
	// Set to destroy
	destroy();
	dmg_label->destroy();
}

void Arrow::onTimerFinish(void* param)
{
	tRenderer->isVanishing = true;
}
// TODO - Los colliders de la flecha no aparecen ir acordes al movimiento. HACK: Poner el navigator antes del collider