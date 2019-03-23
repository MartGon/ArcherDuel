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
		PLAYERAI_STATE_AIRBORNE
	};

	// Constructor
	PlayerAI();

	// Attributes
	PlayerAIState state = PLAYERAI_STATE_IDLE;

	// References
	Player* enemy = nullptr;

	// Overrided methods
	void onPlayerUpdate() override;
	
	// Own methods
	void setBoundaries(Vector2<float> lBoundary, Vector2<float> rBoundary);
private:
	// Movement
	// Extracted by mouse l = 368 r = 464
	Vector2<float> lBoundary;
	Vector2<float> rBoundary;
	Vector2<float> waypoint;

	Vector2<float> getRandomValidMovementPoint();
	void setWaypoint(Vector2<float> waypoint);
	MovDirection getDirToMove();

	// Attacking
	Vector2<float> target;
	float height_offset = 0;
	int draw_frames;

	void setTarget(Vector2<float> target);
	void aimBowToTarget();

	// State
	void onIdle();
	void onMoveToWayPoint();
	void onAttack();
	void onAttacking();
};