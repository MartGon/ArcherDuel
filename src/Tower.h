#include "GameObject.h"

class Tower : public GameObject
{
public:
	enum RoofColor 
	{
		ROOF_COLOR_RED,
		ROOF_COLOR_BLUE
	};

	Tower(RoofColor roofColor = ROOF_COLOR_RED);

	// Components
	TextureRenderer* tRenderer;
};