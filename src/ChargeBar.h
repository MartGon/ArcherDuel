#include "GameObject.h"

class ChargeBar : public GameObject
{
public:
	ChargeBar();

	// Components
	TextureRenderer* tRenderer = nullptr;

	// GameObjects
	GameObject *indicator = nullptr;

	// Own methods
	void updateChargeValue();
	int getChargeValue();
	void resetChargeValue();

private:

	// Variables
	int charge = 0;

	// Indicator
	bool decreasing = false;
	int frameCounter = 0;
	int framesToMove = 1;
};
