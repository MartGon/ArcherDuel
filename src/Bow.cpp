#include "Bow.h"
#include "Navigator.h"
#include "RendererManager.h"

Bow::Bow() : GameObject()
{
	MapRGB *colorKey = new MapRGB();
	colorKey->blue = 0xFF;

	// Collider
	collider = setComponent(new Collider(16, 30));

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
		frame->duration = 1;
	}

	animator->isEnabled = false;
}

void Bow::beforeAnimationFrame(Animation* anim, int frameNumber)
{
	if (anim->id == pull->id)
	{
		Vector2<float> pos = arrow->transform.position;
		Vector2<float> aDir = Vector2<float>(1, 0);
		TextureRenderer* aRenderer = arrow->getComponent<TextureRenderer>();

		// V1: In order to bow and arrow rotate algon, abs rotation center has to be the same
		// V2: Modifying the rotation center along the arrow position does the trick
		// TODO - Need a set pos method that also modifies the center
		if (frameNumber > 1)
		{
			pos.x = pos.x - (1 * aDir.x);
			pos.y = pos.y - (1 * aDir.y);
			aRenderer->center->x += 1;
		}
		else
		{
			pos.x = pos.x - (3 * aDir.x);
			pos.y = pos.y - (3 * aDir.y);
			aRenderer->center->x += 3;
		}
		arrow->transform.position = pos;
		
	}
	else if (anim->id == rel->id)
	{
		Vector2<float> pos = arrow->transform.position;

		int lastFrame = (anim->frames.size() - 1);
		if (frameNumber ==  lastFrame)
		{
			pos.x = pos.x + 3;
		}
		else
			pos.x = pos.x + 1;

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
		nav->speed = 3;
		nav->isEnabled = true;
		state = BOW_STATE_IDLE;

		aRenderer->center = new Vector2<int>(2, 2);
	}
}

void Bow::onUpdate()
{

	Vector2<float> dest;

	int x, y;
	SDL_GetMouseState(&x, &y);

	dest.x = x;
	dest.y = y;

	dest = dest / RendererManager::getScaler();

	dir = dest - getArrowInitialPosition();

	transform.zRotation = dir.getAngle();
		
	if (arrow)
	{
		arrow->transform.zRotation = dir.getAngle();
	}
}

void Bow::handleEvent(const SDL_Event &event)
{
	if (event.type != SDL_KEYDOWN)
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
		arrow->transform.position = getArrowInitialPosition();
		arrow->getComponent<Navigator>()->isEnabled = false;
	default:
		break;
	}
}

Vector2<float> Bow::getArrowInitialPosition()
{
	return Vector2<float>(transform.position.x + 6, transform.position.y + 13);
}