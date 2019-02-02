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
}

void Bow::beforeAnimationFrame(Animation* anim, int frameNumber)
{
	Vector2<float> pos = arrow->transform.position;
	Vector2<float> hPos = owner->pHand->transform.position;
	Vector2<float> scale = transform.scale;

	if (anim->id == pull->id)
	{
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
        int lastFrame = (anim->frames.size() - 1);
        if (frameNumber < lastFrame)
        {
            pos.x = pos.x + 1;
			//hPos.x = hPos.x + 1 * scale.x;
        }
        else
        {
            pos.x = pos.x + 3;
			//hPos.x = hPos.x + 3 * scale.x;
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
		// Setting up navigator
		Navigator* nav = arrow->nav;
		nav->setDirection(dir);
		nav->speed = 2 + (charge * 0.1);
		nav->isEnabled = true;
		nav->isKinematic = true;
		nav->acceleration.y = -0.005f;
		state = BOW_STATE_ARROW_LAUNCHED;

        // Change position
        arrow->transform.position = Utilities::rotatePointFromCenter(arrow->transform.position + (Vector2<float>)*(arrow->transform.rotationCenter), arrow->transform.zRotation, arrow->transform.position);
        arrow->transform.rotationCenter = nullptr;

		arrow->transform.position = arrow->getAbsolutePosition();
		arrow->transform.parent = nullptr;

		// Enabling collider
		arrow->rotCollider->isEnabled = true;

		// Remove arrow
		arrow = nullptr;

        // Reset charge
        charge = 0;

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
    switch (state)
    {
    case Bow::BOW_STATE_IDLE:
		pointBowToMouse();
        break;
    case Bow::BOW_STATE_PULLING:
		pointBowToMouse();
        break;
    case Bow::BOW_STATE_PULLED:
        // Charge Bar
		pointBowToMouse();
        charge = charge++ % 101;
        //printf("La carga es %i\n", charge);
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

void Bow::handleEvent(const SDL_Event &event)
{

	if (event.type != SDL_KEYDOWN)
		return;

	Navigator *nav = nullptr;
    TextureRenderer *aRenderer = nullptr;

	switch (event.key.keysym.sym)
	{
	case SDLK_p:
		if (state != BOW_STATE_IDLE)
			return;

		animator->setCurrentAnimation(pull);
		animator->isEnabled = true;
		state = BOW_STATE_PULLING;
		break;
	case SDLK_r:
		if (state != BOW_STATE_PULLED)
			return;

		animator->setCurrentAnimation(rel);
		animator->isEnabled = true;
		state = BOW_STATE_RELEASING;
		break;
	case SDLK_l:
        if (state != BOW_STATE_ARROW_LAUNCHED)
            return;

		loadArrow();
		state = BOW_STATE_IDLE;
        break;
    case SDLK_u:
        if (state != BOW_STATE_IDLE)
            return;

        //if (angle < 90)
            angle += angle_inc;
        break;
    case SDLK_j:
        if (state != BOW_STATE_IDLE)
            return;

        if (angle >= angle_inc)
            angle -= angle_inc;

        break;
	default:
		break;
	}
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

void Bow::pointBowToMouse()
{
    Vector2<float> dest;

	// Get Mouse position
    int x, y;
    SDL_GetMouseState(&x, &y);

    dest.x = x;
    dest.y = y;

    // Destination point
    dest = dest / RendererManager::getScaler();

    //printf("Dest es %f, %f\n", dest.x, dest.y);

    // Calculate Direction vector
	Vector2<float> center = getAbsoluteRotationCenter();
    dir = (dest - center);
	dir.normalize();

    //printf("Dir es %f, %f\n", dir.x, dir.y);

	// Rotate Bow
	transform.zRotation = dir.getAngle();

	// Rotate arrow
	if (arrow)
		arrow->transform.zRotation = dir.getAngle();
}

void Bow::rotateArrow()
{
    dir = Vector2<float>(-angle);
    //transform.zRotation = dir.getAngle();
    if (arrow)
        arrow->transform.zRotation = dir.getAngle();
}