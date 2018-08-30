#pragma once
#include "GameObject.h"

class Player : public GameObject
{
public:
	Player();

	// Components
	TextureRenderer* tRenderer = nullptr;
	BoxCollider* bCollider = nullptr;
};