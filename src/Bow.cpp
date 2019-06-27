#include "Bow.h"
#include "Player.h"
#include "LevelOne.h"
#include "Navigator.h"
#include "RendererManager.h"
#include "RotatableBoxCollider.h"
#include "Utilities.h"
#include "SceneManager.h"

Bow::Bow() : GameObject()
{
	// Texture Renderer
	MapRGB colorKey;
	colorKey.blue = 0xFF;
	tRenderer = setComponent(new TextureRenderer());
	tRenderer->setLayer(253);
	SceneManager::scene->addComponentToManager(tRenderer);
	tRenderer->hasTrailEffect = true;

	// Set scale
	transform.scale = Vector2<float>(0.5f, 0.5f);

	// Setup Animator
	animator = setComponent(new Animator());
	pull = animator->addAnimation("MyBow", &colorKey, tRenderer, 5, 1, 1);
	animator->setCurrentAnimation(pull);

	for (auto &frame : pull->frames)
	{
		frame->duration = 10;
	}

	rel = animator->addAnimation("MyBow", &colorKey, tRenderer, 9, 5, 2);

	for (auto &frame : rel->frames)
	{
		frame->duration = 2;
	}

	animator->isEnabled = false;

	// Audio Player
	aPlayer = setComponent(new AudioPlayer());
	rel_index = aPlayer->addAudioToList(aPlayer->loadAudioFile("bow-release-16.wav"));
	rel_special_index = aPlayer->addAudioToList(aPlayer->loadAudioFile("bow-release-special-trim.wav"));
	pull_index = aPlayer->addAudioToList(aPlayer->loadAudioFile("bow-pull.wav"));

	// Network
	isNetworkStatic = false;
}

void Bow::beforeAnimationFrame(Animation* anim, int frameNumber)
{
	Vector2<float> pos = arrow->transform.position;
	Vector2<float> hPos = owner->pHand->transform.position;
	Vector2<float> scale = transform.scale;

	if (anim->id == pull->id)
	{
		// Play Sound effect
		if (frameNumber == 1)
		{
			if (!owner->powerUpHookTemplate(&PowerUp::onBowPull))
			{
				aPlayer->setAudioToPlay(pull_index);
				aPlayer->loop = true;
				aPlayer->play();
			}
		}
		// V1: In order to bow and arrow rotate algon, abs rotation center has to be the same
		// V2: Modifying the rotation center along the arrow position does the trick
		// V3: The center is not modified along the pull animation, just the position
		// V4: Modifying the rotation center along the arrow position does the trick, or when pulled the arrow doesn't follow the rotation
		// TODO - Need a set pos method that also modifies the center
		if (frameNumber > 1)
		{
			pos.x = pos.x - 1 * scale.x;
			hPos.x = hPos.x - 1 * scale.x;
		}
		else
		{
			pos.x = pos.x - 3 * scale.x;
			hPos.x = hPos.x - 3 * scale.x;
		}
	}
	else if (anim->id == rel->id)
	{   
		// Play Sound effect
		if (frameNumber == 1)
		{
			// AfterShoot hook
			if (!owner->powerUpHookTemplate(&PowerUp::onBowRelease))
			{
				aPlayer->setAudioToPlay(rel_index);
				aPlayer->play();
			}
		}

        int lastFrame = (anim->frames.size() - 1);
        if (frameNumber < lastFrame)
        {
            pos.x = pos.x + 1;
        }
        else
        {
            pos.x = pos.x + 3;
        }
	}

	// Set arrow new pos and rotation center
	arrow->transform.position = pos;
	arrow->setAbsoluteRotationCenter(getAbsoluteRotationCenter());

	// Set archer new pos and rotation center
	owner->pHand->transform.position = hPos;
	owner->pHand->setAbsoluteRotationCenter(getAbsoluteRotationCenter());
}

void Bow::onAnimationFinished(Animation *anim)
{
	TextureRenderer *aRenderer = arrow->getComponent<TextureRenderer>();

	if (anim->id == pull->id)
	{
		state = BOW_STATE_PULLED;
	}
	else if (anim->id == rel->id)
	{
		// Get charge value an disable charge bar
		float charge = owner->chargeBar->getChargeValue();
		owner->chargeBar->disable();

		// OnShoot hook
		owner->powerUpHookTemplate<void(PowerUp::*)(float &), float&>(&PowerUp::onShoot, charge);

		// Launch Arrow
		launchArrow(arrow, charge);

		// Remove arrow
		arrow = nullptr;

		// Change Bow state
		state = BOW_STATE_ARROW_LAUNCHED;

		// Reset pHand position and center
		owner->pHand->transform.position = Vector2<float>(13, 11);
		owner->pHand->setAbsoluteRotationCenter(getAbsoluteRotationCenter());

		// AfterShoot hook
		owner->powerUpHookTemplate(&PowerUp::afterShoot, charge);
	}
}

void Bow::onStart()
{
}

void Bow::onUpdate()
{
	pointBowToMouse();

	if (!isEnabled)
		return;

	if (Player* player = this->owner)
	{
		if (!player->isAI)
		{
			if (shootInput())
			{
				if (state == BOW_STATE_IDLE)
				{
					// Check for arrow availability
					if (arrow)
					{
						// Pull Bow
						animator->setCurrentAnimation(pull);
						animator->isEnabled = true;
						state = BOW_STATE_PULLING;
					}
				}
			}
			if (SceneManager::scene->inputManager->isMouseEvent(SDL_BUTTON_LEFT, player->network_owner, KeyEvent::UP))
			{
				// If released during pull animation, it will be an instant cast
				if (state == BOW_STATE_PULLING)
					instant_cast = true;
				if (state == BOW_STATE_PULLED)
				{
					// Release bow
					shoot();
				}
			}
		}
	}

    switch (state)
    {
    case Bow::BOW_STATE_IDLE:
		// Load arrow if we don't have one
		if (!arrow)
			arrow = loadArrow();
        break;
    case Bow::BOW_STATE_PULLING:
	{
		// Nothing

		// Call onPull hook
		bool interrupt = false;
		for (auto pu_pair : owner->power_ups)
		{
			PowerUp* power_up = pu_pair.second;
			power_up->beforePull();

			if (!interrupt)
				interrupt = power_up->interruptDefaultAction();
		}

		// Don't charge if any buff interrupts
		if (!interrupt)
			owner->chargeBar->enable();

		break;
	}
    case Bow::BOW_STATE_PULLED:
        // Charge Bar
		if (instant_cast)
		{
			// Shoot
			shoot();

			// Reset flag
			instant_cast = false;
		}
		else
			owner->chargeBar->updateChargeValue();
        break;
    case Bow::BOW_STATE_RELEASING:
        // Nothing
        break;
    case Bow::BOW_STATE_ARROW_LAUNCHED:
		// Load next arrow
		if(!arrow)
			arrow = loadArrow();
		state = BOW_STATE_IDLE;
        break;
    default:
        break;
    }
}

bool Bow::OnHandleEvent(const SDL_Event &event)
{
	return false;
}

// Own methods

bool Bow::shootInput()
{
	bool result = false;
	if (Player* player = owner)
	{
		// Check for normal input
		result = SceneManager::scene->inputManager->isMouseEvent(SDL_BUTTON_LEFT, player->network_owner, KeyEvent::DOWN);

		// Check for powerup input
		player->powerUpHookTemplate(&PowerUp::onShootInputCheck, &result);
	}

	return result;
}

Vector2<float> Bow::getArrowInitialPosition(Arrow* arrow)
{
	// With original scale is Vector2<float>(6, 13)

	if (FireArrow* fire_arrow = dynamic_cast<FireArrow*>(arrow))
		return Vector2<float>(3, 5.5f);

	return Vector2<float>(3, 7.f);
}

void Bow::reset()
{
	// Arrow position
	if (arrow)
	{
		arrow->transform.position = getArrowInitialPosition(arrow);
		arrow->setAbsoluteRotationCenter(getAbsoluteRotationCenter());
	}

	// Animation
	animator->setCurrentAnimation(pull);
	animator->isEnabled = false;
	
	// State
	state = BOW_STATE_IDLE;
	
	// Onwer pHand
	owner->pHand->transform.position = Vector2<float>(13, 11);
	owner->pHand->setAbsoluteRotationCenter(getAbsoluteRotationCenter());
}

Arrow* Bow::loadArrow(Arrow* arrow)
{
	// Before LoadArrowHook
	owner->powerUpHookTemplate<void(PowerUp::*)(Arrow*&), Arrow*&>(&PowerUp::beforeLoadArrow,arrow);

	// Create new arrow
	if(!arrow)
		arrow = new Arrow();

	// Set arrow pos and refs
	prepareArrow(arrow);

	// On LoadArrowHook
	owner->powerUpHookTemplate(&PowerUp::onLoadArrow, arrow);

	return arrow;
}

void Bow::prepareArrow(Arrow* arrow)
{
	// Set owner
	arrow->owner = owner;

	// Set references
	arrow->bow = this;
	arrow->transform.parent = &this->transform;

	// Adjust transform
	Vector2<int> absCenter = this->getAbsoluteRotationCenter();
	arrow->transform.position = getArrowInitialPosition(arrow);
	arrow->setAbsoluteRotationCenter(absCenter);
}

void Bow::aimBow(Vector2<float> target) 
{
	// Calculate vector
	Vector2<float> center = getAbsoluteRotationCenter();
	dir = (target - center);
	dir.normalize();

	// Rotate Bow
	transform.zRotation = dir.getAngle();

	// Rotate arrow
	if (arrow)
		arrow->transform.zRotation = dir.getAngle();

	// After Aim hook
	if (owner->powerUpHookTemplate(&PowerUp::afterAimBow))
		return;
}

void Bow::pointBowToMouse()
{
	// Check if the player is an AI
	if (Player* player = this->owner)
		if (player->isAI)
			return;

    Vector2<float> dest;

	// Get Mouse position
	Vector2<Uint16> mouse_pos = SceneManager::scene->inputManager->getInputStatus(owner->network_owner).mouse_pos;

    dest.x = mouse_pos.x;
    dest.y = mouse_pos.y;

    // Destination point
    dest = dest / RendererManager::getScaler();

	// Aim to point
	aimBow(dest);
}

void Bow::rotateArrow()
{
    dir = Vector2<float>(-angle);
    //transform.zRotation = dir.getAngle();
    if (arrow)
        arrow->transform.zRotation = dir.getAngle();
}

void Bow::draw()
{
	if (state != BOW_STATE_IDLE)
		return;

	// Set pull animation
	animator->setCurrentAnimation(pull);
	animator->isEnabled = true;

	// Change state
	state = BOW_STATE_PULLING;
}

void Bow::shoot()
{
	if (state != BOW_STATE_PULLED)
		return;
	
	// Set rel animation
	animator->setCurrentAnimation(rel);
	animator->isEnabled = true;

	// Change state
	state = BOW_STATE_RELEASING;
}

void Bow::launchArrow(Arrow* arrow, float charge)
{
	// Setting up arrow navigator
	Navigator* nav = arrow->nav;
	nav->setDirection(dir);
	nav->speed = 1.75f * charge;
	nav->isEnabled = true;

	// Disable acceleration if max charge
	if (charge == 6.0)
		nav->isKinematic = false;

	// Change arrow position
	arrow->transform.position = Utilities::rotatePointFromCenter(arrow->transform.position + (Vector2<float>)*(arrow->transform.rotationCenter), arrow->transform.zRotation, arrow->transform.position);
	arrow->transform.rotationCenter = nullptr;

	arrow->transform.position = arrow->getAbsolutePosition();
	arrow->transform.parent = nullptr;

	// Enabling arrow collider
	arrow->rotCollider->isEnabled = true;
	arrow->extCollider->isEnabled = true;

	// Set to disappeart after 15 seconds of flight
	arrow->timer = arrow->setComponent(new TimerComponent(15 * 1000));

	// Set trail effect
	arrow->tRenderer->hasTrailEffect = true;
}