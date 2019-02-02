#include "ChargeBar.h"

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
	indicator->transform.position = Vector2<float>(0.5f, 0.5f);
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
			indicator->transform.position = iPos + Vector2<float>(0.5f, 0);
		else
			indicator->transform.position = iPos - Vector2<float>(0.5f, 0);

		// Check if direction is going to change
		if (iPos.x >= 30.f)
			decreasing = true;
		else if (iPos.x <= 1.f)
			decreasing = false;

		// Reset Counter
		frameCounter = 0;
	}
	// Increase counter
	else
		frameCounter++;
}