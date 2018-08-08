#include "Button.h"


// Constructors

Button::Button()
{
}

Button::Button(Texture texture) : GameObject(texture)
{
	setComponent(new Collider(texture));
}

Button::~Button()
{
}

// Methods

void Button::handleEvent(SDL_Event *e)
{
	switch (e->type)
	{
		case SDL_MOUSEMOTION:
			break;
		case SDL_MOUSEBUTTONDOWN:
			break;
		case SDL_MOUSEBUTTONUP:
			break;
		default:
			break;
	}

}

const bool Button::isWithinBoundaries(int mousePosX, int mousePosY)
{
	bool inside = false;

	int x = mousePosX;
	int y = mousePosY;

	if (x < collider->cLeft)
		return false;
	else if (x > collider->cRight)
		return false;
	else if (y < collider->cTop)
		return false;
	else if (y > collider->cBottom)
		return false;
	else
		return true;
}

void Button::setOnClickListener(std::function<void()> callback)
{
	onClick = callback;
}

void Button::onStart()
{
	collider = getComponent<Collider>();
}