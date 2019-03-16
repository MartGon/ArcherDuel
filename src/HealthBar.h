#include "GameObject.h"
#include "TextLabel.h"

class HealthBar : public GameObject
{
public:
	HealthBar();

	// Own GameObjects
	TextLabel* tLabel = nullptr;

	// Components

	// Texture Renderers
	TextureRenderer* leftEndingTRenderer = nullptr;
	TextureRenderer* rightEndingTRenderer = nullptr;
	TextureRenderer* emptyBarTRenderer = nullptr;
	TextureRenderer* filledBarTRenderer = nullptr;

	// Animator
	Animator* animator = nullptr;

	// Animation
	enum HealtBarAnimations 
	{
		ANIMATION_HEALTH_BAR_BLINK
	};

	Animation* blink = nullptr;

	// Overrided Methods
	void beforeAnimationFrame(Animation* anim, int frameNumber) override;

	// Own methods
	void setScale(Vector2<float> scale);
	Vector2<float> getScale();

	void setHealthPercentage(float percent, bool blink_animation = false);
	void reduceHealthByPercent(float percent);

private:

	// HealthBar Textures
	Texture healthBarEndingEmpty;
	Texture healthBarEndingFilled;
	Texture healthBarFragEmpty;
	Texture healthBarFragFilled;

	// Animation
	float blinkFilledBarScaleBefore = 0.0f;
	float blinkFilledBarScaleAfter = 0.0f;
	int duration = 8;
	int duration_counter = 0;
};