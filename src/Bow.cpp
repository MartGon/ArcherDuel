#include "Bow.h"
#include "Player.h"
#include "LevelOne.h"
#include "Navigator.h"
#include "RendererManager.h"
#include "RotatableBoxCollider.h"
#include "Utilities.h"

Bow::Bow() : GameObject()
{
	MapRGB *colorKey = new MapRGB();
	colorKey->blue = 0xFF;

	// Collider
	//collider = setComponent(new BoxCollider(16, 30));

	// Texture Renderer
	tRenderer = setComponent(new TextureRenderer());
	Vector2<float> iArrow = getArrowInitialPosition();

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
	if (anim->id == pull->id)
	{
		Vector2<float> pos = arrow->transform.position;

		// V1: In order to bow and arrow rotate algon, abs rotation center has to be the same
		// V2: Modifying the rotation center along the arrow position does the trick
		// TODO - Need a set pos method that also modifies the center
		if (frameNumber > 1)
		{
			pos.x = pos.x - 1;
			arrow->transform.rotationCenter->x += 1;
		}
		else
		{
			pos.x = pos.x - 3;
			arrow->transform.rotationCenter->x += 3;
		}
		arrow->transform.position = pos;
		
	}
	else if (anim->id == rel->id)
	{
        Vector2<float> pos = arrow->transform.position;
        
        int lastFrame = (anim->frames.size() - 1);
        if (frameNumber < lastFrame)
        {
            pos.x = pos.x + 1;
            arrow->transform.rotationCenter->x -= 1;
        }
        else
        {
            pos.x = pos.x + 3;
            arrow->transform.rotationCenter->x -= 3;
        }
        arrow->transform.position = pos;
	}
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
		Navigator* nav = arrow->getComponent<Navigator>();
		nav->setDirection(dir);
		nav->speed = 3 + (10 * charge * 0.01);
		nav->isEnabled = true;
		nav->isKinematic = true;
		nav->acceleration.y = -0.005f;
		state = BOW_STATE_ARROW_LAUNCHED;

        // Change position
        arrow->transform.position = Utilities::rotatePointFromCenter(arrow->transform.position + (Vector2<float>)*(arrow->transform.rotationCenter), arrow->transform.zRotation, arrow->transform.position);
        arrow->transform.rotationCenter = nullptr;

        // Reset charge
        charge = 0;
	}
}

void Bow::onUpdate()
{
    switch (state)
    {
    case Bow::BOW_STATE_IDLE:
        dir = Vector2<float>(-angle);
        transform.zRotation = dir.getAngle();
        if(arrow)
            arrow->transform.zRotation = dir.getAngle();
        break;
    case Bow::BOW_STATE_PULLING:
        // Nothing
        break;
    case Bow::BOW_STATE_PULLED:
        // Charge Bar
        charge = charge++ % 101;
        printf("La carga es %i\n", charge);
        break;
    case Bow::BOW_STATE_RELEASING:
        // Nothing
        break;
    case Bow::BOW_STATE_ARROW_LAUNCHED:
        arrow->transform.zRotation = arrow->getComponent<Navigator>()->getDirection().getAngle();
        break;
    default:
        break;
    }
}

void Bow::handleEvent(const SDL_Event &event)
{
	if (event.type != SDL_KEYDOWN)
		return;

    if (!owner->level->canPlayerAct(owner))
        return;

	Navigator * nav = nullptr;
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

        arrow->transform.position = getArrowInitialPosition();
        arrow->setAbsoluteRotationCenter(getAbsoluteRotationCenter());
		arrow->getComponent<Navigator>()->isEnabled = false;
        arrow->getComponent<Collider>()->isEnabled = true;
		state = BOW_STATE_IDLE;
        break;
    case SDLK_u:
        if (state != BOW_STATE_IDLE)
            return;

        if (owner->level->turn == owner->level->PLAYER_ONE_TURN)
        {
            if (angle < 90)
                angle += angle_inc;
        }
        else
        {
            if (angle >= (angle_inc + 90))
                angle -= angle_inc;
        }
        break;
    case SDLK_j:
        if (state != BOW_STATE_IDLE)
            return;
        if (owner->level->turn == owner->level->PLAYER_ONE_TURN)
        {
            if (angle >= angle_inc)
                angle -= angle_inc;
        }
        else
        {
            if (angle < 180)
                angle += angle_inc;
        }
        break;
	default:
		break;
	}
}

// Own methods

Vector2<float> Bow::getArrowInitialPosition(bool reversed)
{
    if(reversed)
        return Vector2<float>(transform.position.x - 6, transform.position.y - 13);
    else
        return Vector2<float>(transform.position.x + 6, transform.position.y + 13);
}

void Bow::loadArrow()
{
    arrow = new Arrow();
    arrow->transform.position = getArrowInitialPosition();
    arrow->setAbsoluteRotationCenter(getAbsoluteRotationCenter());
    arrow->bow = this;
}

void Bow::pointBowToMouse()
{

    Vector2<float> dest;

    int x, y;
    SDL_GetMouseState(&x, &y);

    dest.x = x;
    dest.y = y;

    // Destination point
    dest = dest / RendererManager::getScaler();

    //printf("Dest es %f, %f\n", dest.x, dest.y);

    // Direction vector
    // Taking always the center of the window as reference
    dir = dest - Vector2<float>(RendererManager::getNativeResolution().x / 2, RendererManager::getNativeResolution().y / 2);

    //printf("Dir es %f, %f\n", dir.x, dir.y);
}
