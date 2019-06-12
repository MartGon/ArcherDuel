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
	float getChargeValue();

	// Hiding
	void enable(int framesToMove = 0);
	void disable();

private:

	// Variables
	int charge = 0;

	// Indicator
	bool decreasing = false;
	int frameCounter = 0;
	int framesToMove = 0;

	// Indicator limits
	static const float lLimit;
	static const float rLimit;

	// Charge zone limit
	static const float limit1;
	static const float limit2;
	static const float limit3;
	static const float limit4;
	static const float limit5;
};
