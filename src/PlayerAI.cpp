#include "PlayerAI.h"
#include "Random.h"

// Constructor
 
PlayerAI::PlayerAI() : Player()
{
	isAI = true;
}

// Overrided Methods
void PlayerAI::onPlayerUpdate()
{
	switch (state)
	{
	case PLAYERAI_STATE_IDLE:
		onIdle();
		break;
	case PLAYERAI_STATE_MOVING_TO_WAYPOINT:
		onMoveToWayPoint();
		break;
	case PLAYERAI_STATE_ATTACK:
		onAttack();
		break;
	case PLAYERAI_STATE_ATTACKING:
		onAttacking();
		break;
	case PLAYERAI_STATE_AIRBORNE:
		break;
	}

	// Point to player
	Vector2<float> target_pos = enemy->transform.position;
	target_pos.y += height_offset;
	setTarget(target_pos);
	aimBowToTarget();
}

// Own methods
void PlayerAI::setBoundaries(Vector2<float> lBoundary, Vector2<float> rBoundary)
{
	this->lBoundary = lBoundary;
	this->rBoundary = rBoundary;
}

// Private methods

// Movement

Vector2<float> PlayerAI::getRandomValidMovementPoint()
{
	float y = lBoundary.y;
	float x = Random::getRandomUniformFloat(lBoundary.x, rBoundary.x);

	return Vector2<float>(x, y);
}

void PlayerAI::setWaypoint(Vector2<float> waypoint)
{
	this->waypoint = waypoint;
}

Player::MovDirection PlayerAI::getDirToMove()
{
	Vector2<float> cur_pos = transform.position;
	int xDir = waypoint.x - cur_pos.x;
	MovDirection dir = MOV_NONE;

	if (xDir > 0)
		dir = MOV_DIR_RIGHT;
	else if (xDir < 0)
		dir = MOV_DIR_LEFT;

	return dir;
}

// Attacking

void PlayerAI::setTarget(Vector2<float> target)
{
	this->target = target;
}

void PlayerAI::aimBowToTarget()
{
	bow->aimBow(target);
}

// State

void PlayerAI::onIdle()
{
	// Get next waypoint
	Vector2<float> waypoint;
	waypoint.x = Random::getRandomUniformFloat(lBoundary.x, rBoundary.x);
	waypoint.y = lBoundary.y;

	// Set next waypoint
	setWaypoint(waypoint);

	// Notify
	//std::cout << "Waypoint set to " << waypoint << '\n';

	// Change state
	state = PLAYERAI_STATE_MOVING_TO_WAYPOINT;
}

void PlayerAI::onMoveToWayPoint()
{
	MovDirection dir = getDirToMove();

	// Check if we reached the waypoint
	if (dir == MOV_NONE)
	{
		// Stop
		stop();

		// Check for doing a jump
		bool doJump = Random::getRandomUniformInteger(0, 1);

		if (doJump)
			jump();

		// Notify
		//std::cout << "Waypoint reached !!" << std::endl;

		// Change state to idle
		state = PLAYERAI_STATE_ATTACK;

		return;
	}

	// Strafe otherwise
	strafe(dir);
}

void PlayerAI::onAttack() 
{
	// Get some offset for height
	height_offset = Random::getRandomUniformInteger(2, 7) * -10;

	// Pull Bow
	bow->draw();
	state = PLAYERAI_STATE_ATTACKING;

	// Get frames to wait
	draw_frames = Random::getRandomUniformInteger(20, 120);
}

void PlayerAI::onAttacking()
{
	// Decrease wait frames
	draw_frames--;

	// Shoot bow after waiting the draw_frames
	if (!draw_frames)
	{
		bow->shoot();
		state = PLAYERAI_STATE_IDLE;
	}
}