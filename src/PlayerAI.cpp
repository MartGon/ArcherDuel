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
		// Check target state
		if (!isTargetStillValid())
		{
			target_object = chooseTarget();
			target = getAimingTargetPoint(target_collider);
		}
		onAttacking();
		break;
	case PLAYERAI_STATE_AFTERATTACK:
		afterAttack();
		break;
	case PLAYERAI_STATE_AIRBORNE:
		break;
	}

	// Alawys aim to target
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
	target = getAimingTargetPoint(target_collider);

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
		if (placeCannon())
		{
			cannon->aim(target);
		}
		else
		{
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
			height_offset = Random::getRandomUniformInteger(offset - 2, offset) * - 10;
		}
		// If fully charged, don't compensate
		else
			height_offset = 0;

		// Set target
		target = getAimingTargetPoint(target_collider);

		if (isChargingCannon)
		{
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

Vector2<float> PlayerAI::getAimingTargetPoint(BoxCollider* collider)
{
	// Get  target center
	Vector2<float> target_pos = target_object->getAbsolutePosition() + collider->offset + Vector2<float>(collider->cWidth / 2, collider->cHeight / 2);
	if(!isPlacingCannon)
		target_pos.y += height_offset;
	return target_pos;
}

GameObject* PlayerAI::chooseTarget()
{
	GameObject* target_object = nullptr;

	// Target tower only if using cannon or have fire power up
	if (isPlacingCannon || isChargingCannon || power_ups.find(POWER_UP_FIRE) != power_ups.end())
	{
		target_object = enemy_tower;
	}
	// Choose randomly otherwise
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
				int target_enemy = Random::getBoolWithChance(0.5f);

				// Set target object
				if (target_enemy)
					target_object = enemy;
				else
					target_object = enemy_tower;
			}

		}
		else
			target_object = getCloserPowerUp(active_power_ups);
	}

	// Get target collider
	target_collider = nullptr;

	// Target is player
	if (target_object == enemy)
	{
		target_collider = enemy->bCollider;
	}
	// Target is enemy tower
	else if (target_object == enemy_tower)
	{
		// Get colliders
		auto colliders = enemy_tower->getComponents<BoxCollider>();

		// Remove third collider option
		colliders.erase(colliders.begin() + 2);

		
		int index = 0;
		// Is placing cannon or doesn't have fire
		if (isPlacingCannon || power_ups.find(POWER_UP_FIRE) == power_ups.end())
		{
			index = Random::getRandomUniformInteger(0, 3);
		}
		// If has powerup fire
		else
		{
			for (auto fire : enemy_tower->fires)
			{
				if (fire.second->isActive)
					index++;
				else
					break;
			}
		}

		target_collider = colliders.at(index);
	}
	// Target is powerup
	else
	{
		// Create a collider based on texture
		target_collider = target_object->getComponent<BoxCollider>();
	}

	return target_object;
}

bool PlayerAI::isTargetStillValid()
{
	bool result = true;
	// Player
	if (target_object == enemy)
	{
		// Keep player if is active and not inmunne
		result = enemy->isActive && !enemy->isInmunne;
	}
	// Enemy tower
	else if (target_object == enemy_tower)
	{
		if (!isPlacingCannon)
		{
			// If has power up fire, should target a different one
			if (power_ups.find(POWER_UP_FIRE) != power_ups.end())
			{
				if (target_collider)
				{
					unsigned int id = target_collider->id;

					if (Fire* fire = enemy_tower->fires.at(id))
					{
						if (fire->isActive)
							result = false;
					}
				}
			}
		}
	}
	else
	{
		// If power up is no longer in the active list, is not valid
		auto active_power_ups = getActivePowerUpObjects();

		for (auto pou : active_power_ups)
		{
			if (target_object == pou)
				break;
		}

		result = false;
	}

	return result;
}