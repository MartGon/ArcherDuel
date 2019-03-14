#include "GameObject.h"

class HealthBar : public GameObject
{
public:
	HealthBar();

	// Components
	TextureRenderer* leftEndingTRenderer = nullptr;
	TextureRenderer* rightEndingTRenderer = nullptr;
	TextureRenderer* emptyBarTRenderer = nullptr;
	TextureRenderer* filledBarTRenderer = nullptr;

	// Own methods
	void setScale(Vector2<float> scale);
	Vector2<float> getScale();

	void setHealthPercentage(float percent);

private:

	// HealthBar Textures
	Texture healthBarEndingEmpty;
	Texture healthBarEndingFilled;
	Texture healthBarFragEmpty;
	Texture healthBarFragFilled;
};