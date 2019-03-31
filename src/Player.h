#pragma once
#include "GameObject.h"
#include "ChargeBar.h"
#include "Bow.h"

class LevelOne;
class Player : public GameObject
{
public:
	Player();

	enum class PlayerNumber
	{
		PLAYER_ONE,
		PLAYER_TWO
	};

	enum class PlayerTeam
	{
		RED_TEAM,
		BLUE_TEAM
	};

    // Attributes
    float health_points = 100;
	bool isStopped = false;
	bool isStunned = false;
	bool isInmunne = false;
	int stun_duration = 0;	// Duration in frames
	PlayerNumber player_number = PlayerNumber::PLAYER_ONE;
	PlayerTeam player_team = PlayerTeam::RED_TEAM;

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
	ChargeBar* chargeBar = nullptr;
	GameObject* pHand = nullptr;
	GameObject* rHand = nullptr;
	GameObject* dizzy_effect = nullptr;

	// Components
	TextureRenderer* tRenderer = nullptr;
	BoxCollider* bCollider = nullptr;
	Animator* animator = nullptr;
	Navigator* knock_nav = nullptr;
	Navigator* nav = nullptr;

	// Animation
	enum PlayerAnimation
	{
		PLAYER_ANIMATION_IDLE,
		PLAYER_ANIMATION_WALK,
		PLAYER_ANIMATION_DIZZY
	};

	Animation* move = nullptr;
	Animation* dizzy = nullptr;

    // Hooks
		// General
    void handleEvent(const SDL_Event &event) override;
	void onStart() override;
	void onUpdate() override;

		// Texture Renderer
	void onBlinkFinish() override;

		// Navigator
	void afterMove() override;

		// Collider
	void onColliderEnter(Collider *collider) override;

	// AI Hooks
	virtual void onPlayerUpdate() {};

	// Own Methods
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

		// Status effects
	void stun(int duration);
	void knockback(Vector2<float> dir, float strength);
	void recover();
};