#pragma once
#include "GameObject.h"
#include "ChargeBar.h"
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

	// Misc
	bool isAI = false;

    // Game
    LevelOne* level = nullptr;

    // Gameobjects
    Bow* bow = nullptr;
	GameObject* pHand = nullptr;
	GameObject* rHand = nullptr;
	ChargeBar* chargeBar = nullptr;

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

	// AI Hooks

	// Own Methods
	virtual void onPlayerUpdate() {};

		// Movement
	enum MovDirection 
	{
		MOV_NONE,
		MOV_DIR_LEFT,
		MOV_DIR_RIGHT
	};

	void jump();
	void fast_fall();
	void strafe(MovDirection dir);
};