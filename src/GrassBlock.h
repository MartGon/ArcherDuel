#include "GameObject.h"

class GrassBlock : public GameObject
{
public:
	GrassBlock(bool flip);

	// Components
	TextureRenderer* tRenderer = nullptr;
	BoxCollider* bCollider = nullptr;
};