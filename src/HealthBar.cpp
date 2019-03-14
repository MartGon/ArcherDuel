#include "HealthBar.h"

HealthBar::HealthBar()
{
	// Create RGB Map
	MapRGB *colorKey = new MapRGB();
	colorKey->green = 255;

	// Load Textures
	healthBarEndingEmpty = Texture("HealtBarEndingEmpty.png", RendererManager::renderer, colorKey);
	healthBarEndingFilled = Texture("HealtBarEndingFilled.png", RendererManager::renderer, colorKey);
	healthBarFragEmpty = Texture("HealthBarFragEmpty.png", RendererManager::renderer, colorKey);
	healthBarFragFilled = Texture("HealthBarFragFilled.png", RendererManager::renderer, colorKey);

	// Setting textureRenderers
	leftEndingTRenderer = setComponent(new TextureRenderer(healthBarEndingFilled, 255));
	rightEndingTRenderer = setComponent(new TextureRenderer(healthBarEndingFilled, 255));
	emptyBarTRenderer = setComponent(new TextureRenderer(healthBarFragEmpty, 254));
	filledBarTRenderer = setComponent(new TextureRenderer(healthBarFragFilled, 255));

	// Setting offsets
	leftEndingTRenderer->render_offset = Vector2<float>(0, 0);
	emptyBarTRenderer->render_offset = Vector2<float>(healthBarEndingEmpty.mWidth, 0);
	filledBarTRenderer->render_offset = Vector2<float>(healthBarEndingEmpty.mWidth, 0);
	rightEndingTRenderer->render_offset = Vector2<float>(healthBarEndingEmpty.mWidth + healthBarFragEmpty.mWidth, 0);

	// Flip right ending
	rightEndingTRenderer->flip = SDL_FLIP_HORIZONTAL;
}

// Own methods

void HealthBar::setScale(Vector2<float> scale)
{
	// Change scales
	leftEndingTRenderer->scale = Vector2<float>(scale.y, scale.y);
	emptyBarTRenderer->scale = scale;
	filledBarTRenderer->scale = scale;
	rightEndingTRenderer-> scale = Vector2<float>(scale.y, scale.y);

	// Change positions
	emptyBarTRenderer->render_offset = Vector2<float>(healthBarEndingEmpty.mWidth * scale.y, 0);
	filledBarTRenderer->render_offset = Vector2<float>(healthBarEndingEmpty.mWidth * scale.y, 0);
	rightEndingTRenderer->render_offset = Vector2<float>(healthBarEndingEmpty.mWidth * scale.y + healthBarFragEmpty.mWidth * scale.x, 0);
}

void HealthBar::setHealthPercentage(float percent)
{
	float filledBarScale = emptyBarTRenderer->scale.x * percent / 100.0f;

	if (percent == 100.0f)
		rightEndingTRenderer->texture = healthBarEndingFilled;
	else
		rightEndingTRenderer->texture = healthBarEndingEmpty;

	filledBarTRenderer->scale.x = filledBarScale;
}