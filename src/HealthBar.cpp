#include "HealthBar.h"

StatusBar::StatusBar()
{
	// Create RGB Map
	MapRGB *colorKey = new MapRGB();
	colorKey->green = 255;

	// Preparing texture renderers
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

	// Animator
	animator = setComponent(new Animator());
	
	// Animation
	blink = new Animation();
	Frame* emptyBarFrame = new Frame();
	Frame* filledBarFrame = new Frame();

	emptyBarFrame->duration = 5;
	filledBarFrame->duration = 5;

	emptyBarFrame->texture = healthBarFragFilled;
	filledBarFrame->texture = healthBarFragFilled;

	blink->frames.push_back(emptyBarFrame);
	blink->frames.push_back(filledBarFrame);

	blink->id = ANIMATION_HEALTH_BAR_BLINK;
	blink->loop = true;
	blink->name = "blink";
	blink->tRenderer = filledBarTRenderer;

	animator->animations.push_back(blink);
	animator->setCurrentAnimation(blink);
	animator->isEnabled = false;

	// Text Label test
	tLabel = new TextLabel();
	tLabel->transform.parent = &this->transform;
	tLabel->transform.position = Vector2<float>(0, -5);
}

// Overrided Methods
void StatusBar::beforeAnimationFrame(Animation *anim, int frameNumber)
{
	// Change scale during blink animation
	if (anim->id == ANIMATION_HEALTH_BAR_BLINK)
	{
		if (frameNumber)
		{
			filledBarTRenderer->scale.x = blinkFilledBarScaleBefore;
			if (blinkFilledBarScaleBefore == emptyBarTRenderer->scale.x)
			{
				rightEndingTRenderer->texture = healthBarEndingFilled;
			}
			if (blinkFilledBarScaleAfter == 0.0f)
			{
				leftEndingTRenderer->texture = healthBarEndingFilled;
			}
		}
		else
		{
			filledBarTRenderer->scale.x = blinkFilledBarScaleAfter;
			if (blinkFilledBarScaleBefore == emptyBarTRenderer->scale.x)
			{
				rightEndingTRenderer->texture = healthBarEndingEmpty;
			}
			if (blinkFilledBarScaleAfter == 0.0f)
			{
				leftEndingTRenderer->texture = healthBarEndingEmpty;
			}
		}
	}
	
	// Check if the animation should stop
	if (duration_counter == duration)
	{
		filledBarTRenderer->scale.x = blinkFilledBarScaleAfter;

		// Check if it was full health
		if (blinkFilledBarScaleBefore == emptyBarTRenderer->scale.x)
		{
			rightEndingTRenderer->texture = healthBarEndingEmpty;
		}

		// Check if it has no health left
		if (blinkFilledBarScaleAfter <= 0.0f)
		{
			leftEndingTRenderer->texture = healthBarEndingEmpty;
		}

		duration_counter = 0;
		animator->isEnabled = false;
	}
	else
		duration_counter++;
}

// Own methods

void StatusBar::setScale(Vector2<float> scale)
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

	// Set animation vars
	blinkFilledBarScaleBefore = emptyBarTRenderer->scale.x;
	blinkFilledBarScaleAfter = blinkFilledBarScaleBefore;
}

void StatusBar::setHealthPercentage(float percent, bool blink)
{
	// Store old value
	blinkFilledBarScaleBefore = blinkFilledBarScaleAfter;
	
	// Calculate new value
	blinkFilledBarScaleAfter = emptyBarTRenderer->scale.x * percent / 100.0f;

	if (percent == 100.0f)
		rightEndingTRenderer->texture = healthBarEndingFilled;
	else
		rightEndingTRenderer->texture = healthBarEndingEmpty;

	if (percent == 0.0f)
		leftEndingTRenderer->texture = healthBarEndingEmpty;

	if (blink)
	{
		// Enable animation
		animator->isEnabled = true;
	}

	filledBarTRenderer->scale.x = blinkFilledBarScaleAfter;
}