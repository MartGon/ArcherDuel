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
	bool airborne = false;
    float movement_speed = 1.f;
    bool mov_enabled = true;

    // Game
    LevelOne* level = nullptr;

    // Gameobjects
    Bow* bow = nullptr;

	// Components
	TextureRenderer* tRenderer = nullptr;
	BoxCollider* bCollider = nullptr;
	Animator* animator = nullptr;
	Navigator* nav = nullptr;

	// Animation
	enum PlayerAnimation
	{
		PLAYER_ANIMATION_IDLE,
		PLAYER_ANIMATION_WALK
	};

	Animation* move = nullptr;

    // Hooks
		// General
    void handleEvent(const SDL_Event &event) override;
	void onStart() override;
	void onUpdate() override;

		// Navigator
	void afterMove() override;

		// Collider
	void onColliderEnter(Collider *collider) override;
};