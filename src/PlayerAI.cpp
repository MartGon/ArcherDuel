#include "PlayerAI.h"
#include "Random.h"
#include "Tower.h"
#include "Cannon.h"
#include "Scene.h"
#include "LevelOne.h"
#include "TextureRenderer.h"

// Constructor
 
PlayerAI::PlayerAI() : Player(PlayerNumber::PLAYER_TWO)
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
	case PLAYERAI_STATE_AFTERATTACK:
		afterAttack();
		break;
	case PLAYERAI_STATE_AIRBORNE:
		break;
	}

	// Alawys aim to target
	if (isChargingCannon)
		cannon->aim(target);
	else
		aimBowToTarget();

	// Update waypoints
	onMoveToWayPoint();
}

// Own methods
void PlayerAI::setBoundaries(Vector2<float> lBoundary, Vector2<float> rBoundary)
{
	this->lBoundary = lBoundary;
	this->rBoundary = rBoundary;
}

// Private methods

// Movement

Vector2<float> PlayerAI::getRandomMovementPoint(Vector2<float> lBoundary, Vector2<float> rBoundary)
{
	float y = lBoundary.y;
	float x = Random::getRandomUniformFloat(lBoundary.x, rBoundary.x);

	return Vector2<float>(x, y);
}

Vector2<float> PlayerAI::getRandomValidMovementPoint()
{
	return getRandomMovementPoint(lBoundary.x, rBoundary.x);
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
	// Choose temporary target
	target_object = chooseTarget();
	target = getAimingTargetPoint();

	// Change state
	state = PLAYERAI_STATE_ATTACK;
}

void PlayerAI::onMoveToWayPoint()
{
	MovDirection dir = getDirToMove();

	// Check if we reached the waypoint
	if (dir == MOV_NONE)
	{
		// Stop
		stop();

		// Check for doing a jump, don't jump when carrying cannon
		bool doJump = isPlacingCannon ? 0 : Random::getRandomUniformInteger(0, 7);

		if (!doJump)
			jump();

		// Notify
		//std::cout << "Waypoint reached !!" << std::endl;

		// Get next waypoint
		Vector2<float> rLimit;

		// Depending on target move closer to the ledge or anywhere
		if (target_object == enemy_tower)
			rLimit = (rBoundary - lBoundary) / 4 + lBoundary;
		else
			rLimit = (rBoundary - lBoundary) / 2 + lBoundary;

		// Get a random point wihtin boundaries
		Vector2<float> waypoint = getRandomMovementPoint(lBoundary, rLimit);

		// Set next waypoint
		setWaypoint(waypoint);

		return;
	}

	// Strafe otherwise
	if(mov_enabled)
		strafe(dir);
}

void PlayerAI::onAttack() 
{
	// Don't attack if bow is not ready, bow should be idle when using cannon
	if (bow->state != Bow::BOW_STATE_IDLE)
		return;

	// Get frames to wait
	draw_frames = Random::getRandomUniformInteger(100, 121);

	// Place cannon if doing so
	if (isPlacingCannon)
	{
		// If cannon cannot be placed, look for other position
		if (!placeCannon())
		{
			cannon->aim(target);
			state = PLAYERAI_STATE_IDLE;
			return;
		}

		// Set double draw frames
		draw_frames = draw_frames * 1.75f;
	}
	else
	{
		// Pull Bow
		bow->draw();
	}

	// Reset draw frames if powerUpHaste is present
	if (power_ups.find(POWER_UP_HASTE) != power_ups.end())
		draw_frames = 1;

	// Set next state
	state = PLAYERAI_STATE_ATTACKING;
}

void PlayerAI::onAttacking()
{
	// Decrease wait frames
	draw_frames--;

	// Shoot bow after waiting the draw_frames
	if (!draw_frames)
	{
		// Get some offset for height
		int charge = isChargingCannon ? 6 : chargeBar->getChargeValue();

		// If charge is not full, compensate projectile fall
		if (charge == 3)
		{
			float distance = transform.position.x - target.x;
			int offset = distance / 50;
			height_offset = Random::getRandomUniformInteger(offset, offset) * - 10;
		}
		// If fully charged, don't compensate
		else
			height_offset = 0;

		// Set target
		target_object = chooseTarget();
		target = getAimingTargetPoint();
		target.y += height_offset;

		if (isChargingCannon)
		{
			// Aim to target
			cannon->aim(target);

			// Shoot cannon
			shootCannon();
		}
		else
			// Shoot bow
			bow->shoot();

		// Reset state
		state = PLAYERAI_STATE_AFTERATTACK;
	}
}

void PlayerAI::afterAttack()
{
	if (bow->state != Bow::BOW_STATE_IDLE)
		return;

	// If skill is ready, try to shoot cannon
	if (isSkillReady)
	{
		if (power_ups.find(POWER_UP_HASTE) == power_ups.end())
		{
			if (power_ups.find(POWER_UP_SHIELD) != power_ups.end())
				withdrawCannon();
			else
			{
				float modifier = power_ups.size() * 2 + 2;
				int use_cannon = power_ups.empty() ? 0 : Random::getBoolWithChance(1.f / modifier);

				if (use_cannon)
					withdrawCannon();
			}
		}
	}
	
	state = PLAYERAI_STATE_IDLE;
}

// Sensors

std::vector<PowerUpObject*> PlayerAI::getActivePowerUpObjects()
{
	std::vector<PowerUpObject*> power_ups;

	for (auto goPair : level->gameObjectMap)
	{
		GameObject* go = goPair.second;

		if (PowerUpObject* puo = dynamic_cast<PowerUpObject*>(go))
		{
			power_ups.push_back(puo);
		}
	}

	return power_ups;
}

PowerUpObject* PlayerAI::getCloserPowerUp(std::vector<PowerUpObject*> power_ups)
{
	float min_distance = 600;
	PowerUpObject* closer_puo = nullptr;

	for (auto puo : power_ups)
	{
		float distance = std::abs(puo->transform.position.x - transform.position.x);

		if (distance < min_distance)
		{
			min_distance = distance;
			closer_puo = puo;
		}
	}

	return closer_puo;
}

Vector2<float> PlayerAI::getAimingTargetPoint()
{
	// Get target collider
	BoxCollider* collider = nullptr;
	BoxCollider bCollider;
	if (target_object == enemy)
	{
		collider = enemy->bCollider;
	}
	else if (target_object == enemy_tower)
	{
		// Get colliders
		auto colliders = enemy_tower->getComponents<BoxCollider>();

		// Remove third collider option
		colliders.erase(colliders.begin() + 2);

		// If has powerup fire
		int index = 0;
		if (power_ups.find(POWER_UP_FIRE) != power_ups.end())
		{
			for (auto fire : enemy_tower->fires)
			{
				if (fire.second->isActive)
					index++;
				else
					break;
			}
		}
		else
		{
			// Choose one of them randomly
			index = Random::getRandomUniformInteger(0, 3);
		}

		collider = colliders.at(index);
	}
	else
	{
		// Target closer power_up
		auto power_ups = getActivePowerUpObjects();
		if (!power_ups.empty())
		{
			// Get closer power up
			target_object = getCloserPowerUp(power_ups);

			// Create a collider based on texture
			auto tRenderer = target_object->getComponent<TextureRenderer>();
			bCollider = BoxCollider(tRenderer->texture);
			collider = &bCollider;
		}
		// If there are not power_ups available, target enemy player and return
		else
		{
			target_object = enemy;
			collider = enemy->bCollider;
		}
	}

	// Get  target center
	Vector2<float> target_pos = target_object->getAbsolutePosition() + collider->offset + Vector2<float>(collider->cWidth / 2, collider->cHeight / 2);

	return target_pos;
}

GameObject* PlayerAI::chooseTarget()
{
	GameObject* target_object = nullptr;

	// Target tower only if using cannon
	if (isPlacingCannon || isChargingCannon)
	{
		target_object = enemy_tower;
	}
	else
	{
		// Get active powerups
		auto active_power_ups = getActivePowerUpObjects();

		// Target powerup or somehting else
		float modifier = 2 + power_ups.size();
		bool target_powerup = active_power_ups.empty() ? 0 : Random::getBoolWithChance(1 / modifier);

		// If we don't target a power up, choose tower or enemy
		if (!target_powerup)
		{
			// Choose tower if enemy is not active or is Inmunne
			if (!enemy->isActive || enemy->isInmunne)
			{
				target_object = enemy_tower;
			}
			// Choose randomly otherwise
			else
			{
				// Select a target, player or tower
				modifier = power_ups.find(POWER_UP_FIRE) != power_ups.end() ? 8 : 2;
				int target_enemy = Random::getBoolWithChance(1.f / modifier);

				// Set target object
				if (target_enemy)
					target_object = enemy;
				else
					target_object = enemy_tower;
			}

		}
		else
			target_object = nullptr;
	}

	return target_object;
}