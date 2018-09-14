#pragma once
#include "GameObject.h"
#include "Bow.h"

class LevelOne;
class Player : public GameObject
{
public:
	Player();

    // Attributes
    float health_points = 100;

    // Movement 
    float movement_speed = 5;
    bool mov_enabled = true;

    // Game
    LevelOne* level = nullptr;

    // Gameobjects
    Bow* bow = nullptr;

	// Components
	TextureRenderer* tRenderer = nullptr;
	BoxCollider* bCollider = nullptr;

    // Hooks
    void handleEvent(const SDL_Event &event) override;
};