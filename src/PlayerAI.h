#pragma once
#include "Player.h"

class PlayerAI : public Player 
{
public:
	enum PlayerAIState 
	{
		PLAYERAI_STATE_IDLE,
		PLAYERAI_STATE_MOVING_TO_WAYPOINT,
		PLAYERAI_STATE_ATTACK,
		PLAYERAI_STATE_ATTACKING,
		PLAYERAI_STATE_AFTERATTACK,
		PLAYERAI_STATE_AIRBORNE
	};

	// Constructor
	PlayerAI(PlayerNumber player_number);

	// Attributes
	PlayerAIState state = PLAYERAI_STATE_IDLE;
	bool isStopped = false;

	// References
	Tower* enemy_tower = nullptr;
	Player* enemy = nullptr;

	// Overrided methods
	void onPlayerUpdate() override;
	
	// Own methods
	void setBoundaries(Vector2<float> lBoundary, Vector2<float> rBoundary);

	// Attack info
	int draw_frames = 0;
private:
	// Movement
	// Extracted by mouse l = 368 r = 464
	Vector2<float> lBoundary;
	Vector2<float> rBoundary;
	Vector2<float> waypoint = {-1, -1};

	Vector2<float> getRandomMovementPoint(Vector2<float> lBoundary, Vector2<float> rBoundary);
	Vector2<float> getRandomValidMovementPoint();
	void setWaypoint(Vector2<float> waypoint);
	MovDirection getDirToMove();

	// Attacking
	GameObject* target_object = nullptr;
	BoxCollider* target_collider = nullptr;
	Vector2<float> target;
	float height_offset = 0;

	// Targeting
	GameObject* chooseTarget();
	void setTarget(Vector2<float> target);
	void aimBowToTarget();
	Vector2<float> getAimingTargetPoint(BoxCollider* collider);

	// Next state
	PlayerAIState next_state = PLAYERAI_STATE_IDLE;

	// Sensors
	PowerUpObject* getCloserPowerUp(std::vector<PowerUpObject*> power_ups);
	std::vector<PowerUpObject*> getActivePowerUpObjects();
	bool isTargetStillValid();


	// State
	void onIdle();
	void onMoveToWayPoint();
	void onAttack();
	void onAttacking();
	void afterAttack();
};