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
	MapRGB *colorKey = new MapRGB();
	colorKey->blue = 0xFF;
	tRenderer = setComponent(new TextureRenderer());
	tRenderer->setLayer(253);
	SceneManager::scene->addComponentToManager(tRenderer);

	// Set scale
	transform.scale = Vector2<float>(0.5f, 0.5f);

	// Setup Animator
	animator = setComponent(new Animator());
	pull = animator->addAnimation("MyBow", colorKey, tRenderer, 5, 1, 1);
	animator->setCurrentAnimation(pull);

	for (auto &frame : pull->frames)
	{
		frame->duration = 10;
	}

	rel = animator->addAnimation("MyBow", colorKey, tRenderer, 9, 5, 2);

	for (auto &frame : rel->frames)
	{
		frame->duration = 2;
	}

	animator->isEnabled = false;

	// Audio Player
	aPlayer = setComponent(new AudioPlayer());
	rel_index = aPlayer->addAudioToList(aPlayer->loadAudioFile("bow-release-16.wav"));
	pull_index = aPlayer->addAudioToList(aPlayer->loadAudioFile("bow-pull.wav"));
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
			aPlayer->setAudioToPlay(pull_index);
			aPlayer->loop = true;
			aPlayer->play();
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
			aPlayer->setAudioToPlay(rel_index);
			aPlayer->play();
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

		// Setting up arrow navigator
		Navigator* nav = arrow->nav;
		nav->setDirection(dir);
		nav->speed = 2 * charge;
		nav->isEnabled = true;

		// Disable acceleration if max charge
		if(charge == 6.0)
			nav->isKinematic = false;

		// Set bow state
		state = BOW_STATE_ARROW_LAUNCHED;

        // Change arrow position
        arrow->transform.position = Utilities::rotatePointFromCenter(arrow->transform.position + (Vector2<float>)*(arrow->transform.rotationCenter), arrow->transform.zRotation, arrow->transform.position);
        arrow->transform.rotationCenter = nullptr;

		arrow->transform.position = arrow->getAbsolutePosition();
		arrow->transform.parent = nullptr;

		// Enabling arrow collider
		arrow->rotCollider->isEnabled = true;

		// Remove arrow
		arrow = nullptr;

		// Reset pHand position and center
		owner->pHand->transform.position = Vector2<float>(13, 11);
		owner->pHand->setAbsoluteRotationCenter(getAbsoluteRotationCenter());
	}
}

void Bow::onStart()
{
	// Load an arrow
	loadArrow();
}

void Bow::onUpdate()
{
	// Always point to mouse
	if (owner->level && owner->level->isOnline())
	{
		if (owner->level->mode == Scene::ONLINE_SERVER)
		{
			if (owner->player_number == Player::PlayerNumber::PLAYER_ONE)
				pointBowToMouse();
		}
		else if (owner->level->mode == Scene::ONLINE_CLIENT)
		{
			if (owner->player_number == Player::PlayerNumber::PLAYER_TWO)
				pointBowToMouse();
		}
	}
	else
		pointBowToMouse();

    switch (state)
    {
    case Bow::BOW_STATE_IDLE:
		// Nothing
        break;
    case Bow::BOW_STATE_PULLING:
		// Nothing
		owner->chargeBar->enable();
        break;
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
		loadArrow();
		state = BOW_STATE_IDLE;
        break;
    default:
        break;
    }
}

bool Bow::handleEvent(const SDL_Event &event)
{
	// Check if the player is an AI
	if (Player* player = this->owner)
		if (player->isAI)
			return false;

	if (event.type == SDL_KEYDOWN)
	{
		switch (event.key.keysym.sym)
		{
		case SDLK_p:
			draw();
			break;
		case SDLK_r:
			shoot();
			break;
		case SDLK_l:
			if (state != BOW_STATE_ARROW_LAUNCHED)
				return false;

			loadArrow();
			state = BOW_STATE_IDLE;
			break;
		case SDLK_u:
			if (state != BOW_STATE_IDLE)
				return false;

			//if (angle < 90)
			angle += angle_inc;
			break;
		case SDLK_j:
			if (state != BOW_STATE_IDLE)
				return false;

			if (angle >= angle_inc)
				angle -= angle_inc;

			break;
		default:
			break;
		}
	}
	else if (event.type == SDL_MOUSEBUTTONDOWN)
	{
		switch (event.button.button)
		{
		case SDL_BUTTON_LEFT:
			if (state != BOW_STATE_IDLE)
				return false;

			if (owner->isStopped)
				return false;

			// Pull Bow
			animator->setCurrentAnimation(pull);
			animator->isEnabled = true;
			state = BOW_STATE_PULLING;
			break;
		}
	}
	else if (event.type == SDL_MOUSEBUTTONUP)
	{
		switch (event.button.button)
		{
		case SDL_BUTTON_LEFT:
			// If released during pull animation, it will be an instant cast
			if (state == BOW_STATE_PULLING)
				instant_cast = true;
			if (state == BOW_STATE_PULLED)
			{
				// Release bow
				shoot();
				break;
			}
		}
	}
	else
		return false;

	return true;
}

// Own methods

Vector2<float> Bow::getArrowInitialPosition(bool reversed)
{
	// With original scale is Vector2<float>(6, 13)

    if(reversed)
        return Vector2<float>(-3, 7.f);
    else
        return Vector2<float>(3, 7.f);
}

void Bow::reset()
{
	// Arrow position
	arrow->transform.position = getArrowInitialPosition();
	arrow->setAbsoluteRotationCenter(getAbsoluteRotationCenter());

	// Animation
	animator->setCurrentAnimation(pull);
	animator->isEnabled = false;
	
	// State
	state = BOW_STATE_IDLE;
	
	// Onwer pHand
	owner->pHand->transform.position = Vector2<float>(13, 11);
	owner->pHand->setAbsoluteRotationCenter(getAbsoluteRotationCenter());
}

void Bow::loadArrow()
{
	// Create new arrow
    arrow = new Arrow();

	// Set references
	arrow->bow = this;
	arrow->transform.parent = &this->transform;

	// Adjust transform
	Vector2<int> absCenter = this->getAbsoluteRotationCenter();
    arrow->transform.position = getArrowInitialPosition();
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
}

void Bow::pointBowToMouse()
{
	// Check if the player is an AI
	if (Player* player = this->owner)
		if (player->isAI)
			return;

    Vector2<float> dest;

	// Get Mouse position
    int x, y;
    SDL_GetMouseState(&x, &y);

    dest.x = x;
    dest.y = y;

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