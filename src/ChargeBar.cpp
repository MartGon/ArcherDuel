#include "ChargeBar.h"

// Indicator limits
const float ChargeBar::lLimit = 0.75f;
const float ChargeBar::rLimit = 30.75f;

// Charge zone limit
const float ChargeBar::limit1 = 7.5f;
const float ChargeBar::limit2 = 15.f;
const float ChargeBar::limit3 = 20.f;
const float ChargeBar::limit4 = 25.f;
const float ChargeBar::limit5 = 30.f;

ChargeBar::ChargeBar()
{
	// Change scale
	transform.scale = Vector2<float>(0.5f, 0.5f);

	// Set Texture Renderer
	tRenderer = setComponent(new TextureRenderer("ChargeBar.png", nullptr, 254));
	tRenderer->texture.setAlpha(255);

	// Create indicator
	indicator = new GameObject();
	indicator->setComponent(new TextureRenderer("ChargeIndicator.png", nullptr, 255));
	indicator->transform.scale = transform.scale;
	indicator->transform.parent = &this->transform;
	indicator->transform.position = Vector2<float>(0.75f, 0.75f);

	// Set isActive to false
	isActive = false;
	indicator->isActive = false;
}

// Own methods

void ChargeBar::updateChargeValue()
{
	Vector2<float> iPos = indicator->transform.position;

	// Move indicator eachs @framesToMove frames
	if (frameCounter == framesToMove)
	{
		// Update indicator position
		if (!decreasing)
			iPos = iPos + Vector2<float>(0.5f, 0);
		else
			iPos = iPos - Vector2<float>(0.5f, 0);

		// Check if direction is going to change
		if (iPos.x >= rLimit)
			decreasing = true;
		else if (iPos.x <= lLimit)
			decreasing = false;

		// Set position
		indicator->transform.position = iPos;

		// Reset Counter
		frameCounter = 0;
	}
	// Increase counter
	else
		frameCounter++;
}

float ChargeBar::getChargeValue() 
{
	float iX = indicator->transform.position.x;

	if (iX < limit1)
		return 1.f;
	else if (iX < limit2)
		return 1.5f;
	else if (iX < limit3)
		return 2.f;
	else if (iX < limit4)
		return 2.5f;
	else if (iX < limit5)
		return 3.f;
	else
		return 6.f;
}

// Hiding

void ChargeBar::enable(int framesToMove)
{
	this->framesToMove = framesToMove;
	this->isActive = true;
	indicator->isActive = true;
}

void ChargeBar::disable()
{
	this->isActive = false;
	indicator->isActive = false;

	// Reset position
	indicator->transform.position.x = lLimit;
	
	// Reset counter
	frameCounter = 0;
}