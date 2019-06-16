#pragma once
#include "GameObject.h"
#include "ChargeBar.h"
#include "Bow.h"
#include "PowerUp.h"

#include <unordered_map>

class StatusBar;
class Cannon;
class Tower;
class LevelOne;

enum class PlayerNumber
{
	PLAYER_ONE = 1,
	PLAYER_TWO,
	PLAYER_THREE,
	PLAYER_FOUR
};

class Player : public GameObject
{
public:
	Player(PlayerNumber number);

	enum class PlayerTeam
	{
		RED_TEAM,
		BLUE_TEAM
	};

    // Attributes
	float skill_points = 0;
	bool isStunned = false;
	bool isInmunne = false;
	bool isSkillReady = false;
	bool isPlacingCannon = false;
	bool isChargingCannon = false;
	int stun_duration = 0;	// Duration in frames
	PlayerNumber player_number = PlayerNumber::PLAYER_ONE;
	PlayerTeam player_team = PlayerTeam::RED_TEAM;
	std::unordered_map<PowerUpType, PowerUp*> power_ups;

    // Movement
	bool airborne = true;
    float movement_speed = 1.f;
    bool mov_enabled = true;

	// Misc
	bool isAI = false;

    // Game
    LevelOne* level = nullptr;

    // Gameobjects
	Tower* tower = nullptr;
    Bow* bow = nullptr;
	Cannon* cannon = nullptr;
	ChargeBar* chargeBar = nullptr;
	StatusBar* skill_bar = nullptr;
	GameObject* pHand = nullptr;
	GameObject* rHand = nullptr;
	GameObject* dizzy_effect = nullptr;

	// Components
	AudioPlayer* aPlayer = nullptr;
	TextureRenderer* tRenderer = nullptr;
	BoxCollider* bCollider = nullptr;
	Animator* animator = nullptr;
	Navigator* mov_nav = nullptr;
	Navigator* knock_nav = nullptr;
	Navigator* jump_nav = nullptr;

	// Audio
	int power_up_sf = 0;

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
    bool OnHandleEvent(const SDL_Event &event) override;
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

	typedef void (PowerUp::*PowerUpHook)();

	std::string getPathByPlayerNumber(PlayerNumber number);
	void jump();
	void fast_fall();
	void strafe(MovDirection dir);
	void stop();
	void increase_skill_points(float skill_points);
	void addPowerUp(PowerUp* power_up);
	void removePowerUp(PowerUp* power_up);
	bool powerUpHook(PowerUpHook hook);

	template<typename MemFn, typename ... Params>
	bool powerUpHookTemplate(MemFn hook, Params ... params)
	{
		bool interrupt = false;
		for (auto power_up_pair : power_ups)
		{
			PowerUp* power_up = power_up_pair.second;

			std::invoke(hook, *power_up, params...);

			// Check interrupt
			if (!interrupt)
				interrupt = power_up->interruptDefaultAction();
		}

		return interrupt;
	}

		// Cannon
	bool withdrawCannon();
	bool placeCannon();
	bool shootCannon();

		// Status effects
	void stun(int duration);
	void knockback(Vector2<float> dir, float strength);
	void recover();
};