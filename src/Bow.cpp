#include "Bow.h"
#include "Navigator.h"

Bow::Bow() : GameObject()
{
	MapRGB *colorKey = new MapRGB();
	colorKey->blue = 0xFF;
	tRenderer = setComponent(new TextureRenderer());

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

		if (frameNumber > 1)
		{
			pos.x = pos.x - 1;
		}
		else
			pos.x = pos.x - 3;

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
	if (anim->id == rel->id)
	{
		Navigator* nav = arrow->getComponent<Navigator>();
		nav->setDirection(Vector2<float>(1, 0));
		nav->speed = 3;
	}
}

void Bow::onUpdate()
{

}

void Bow::handleEvent(const SDL_Event &event)
{
	if (event.type != SDL_KEYDOWN)
		return;

	Navigator * nav = nullptr;
	switch (event.key.keysym.sym)
	{
	case SDLK_p:
		animator->setCurrentAnimation(pull);
		animator->isEnabled = true;
		break;
	case SDLK_r:
		animator->setCurrentAnimation(rel);
		animator->isEnabled = true;
		break;
	default:
		break;
	}
}