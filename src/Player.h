#pragma once
#include "GameObject.h"

class Player : public GameObject
{
public:
	Player();

    // Attributes
    float health_points = 100;

	// Components
	TextureRenderer* tRenderer = nullptr;
	BoxCollider* bCollider = nullptr;

    // Hooks
    void handleEvent(const SDL_Event &event) override;
};