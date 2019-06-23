#include "Player.h"
#include "LevelOne.h"
#include "Tower.h"
#include "SceneManager.h"
#include "Cannon.h"
#include "HealthBar.h"
#include "PowerUp.h"

#include <functional>

Player::Player(PlayerNumber player_number)
{
	// Texture Renderer
	MapRGB colorKey;
	colorKey.green = 255;
	std::string subPath = getPathByPlayerNumber(player_number);
	std::string path = "Archer" + subPath + ".png";
	tRenderer = setComponent(new TextureRenderer(path.c_str() , &colorKey, 254));

	// Jump Navigator
	jump_nav = setComponent(new Navigator(Vector2<float>(0, 0), 1, true, Vector2<float>(0, -0.15)));
	jump_nav->isEnabled = true;
	//nav->isEnabled = false;

	// Move Navigator
	mov_nav = setComponent(new Navigator(Vector2<float>(0, 0), 1));
	mov_nav->isEnabled = true;

	// Knockback Navigator
	knock_nav = setComponent(new Navigator(Vector2<float>(0, 0), 1, true, Vector2<float>(0, 0)));
	knock_nav->stopAtInflectionPoint = true;
	knock_nav->isEnabled = false;

	// BoxCollider
	bCollider = setComponent(new BoxCollider(5, 15));
	bCollider->offset = Vector2<float>(3, 4);
    //bCollider->debug = true;

	// Animator
	animator = setComponent(new Animator());
	path = "Archer_Walk" + subPath;
	move = animator->addAnimation(path.c_str(), &colorKey, tRenderer, 4, 1, PLAYER_ANIMATION_WALK);
	move->loop = true;
	animator->setCurrentAnimation(move);
	animator->isEnabled = false;

	for (auto &frame : move->frames)
	{
		frame->duration = 6;
	}
	//move->frames.at(0)->duration = 10;
	//move->frames.at(2)->duration = 10;

	// Create Bow and set Player to its parent
	bow = new Bow();
	bow->owner = this;
	bow->transform.parent = &this->transform;
	
	// Set Center
	bow->transform.rotationCenter = new Vector2<int>(-6, 7);
	
	// Offset is 11, 4
	Vector2<float> offset(11, 4);
	bow->transform.position = offset;

	// Add player pHand
	pHand = new GameObject();
	pHand->setComponent(new TextureRenderer("ArcherHand.png", &colorKey, 255));
	pHand->setScale(Vector2<float>(0.65f, 0.65f));
	pHand->transform.parent = &this->transform;
	pHand->transform.position = Vector2<float>(13, 11);
	pHand->setAbsoluteRotationCenter(bow->getAbsoluteRotationCenter());

	// Add player rHand
	rHand = new GameObject();
	rHand->setComponent(new TextureRenderer("ArcherHand.png", &colorKey, 252));
	rHand->setScale(Vector2<float>(0.65f, 0.65f));
	rHand->transform.parent = &this->transform;
	rHand->transform.position = Vector2<float>(16, 11);
	rHand->setAbsoluteRotationCenter(bow->getAbsoluteRotationCenter());

	// Add ChargerBar
	chargeBar = new ChargeBar();
	chargeBar->transform.parent = &this->transform;
	chargeBar->transform.position = Vector2<float>(-10, -10);

	// GameObject - Dizzy Effect
	dizzy_effect = new GameObject();
	dizzy_effect->transform.parent = &this->transform;
	dizzy_effect->transform.position = Vector2<float>(2, -7);
	TextureRenderer* dizzy_tRenderer = dizzy_effect->setComponent(new TextureRenderer("dizzy_effect1.png", &colorKey, 255));
	Animator* dizzy_anim = dizzy_effect->setComponent(new Animator("dizzy_effect", &colorKey, dizzy_tRenderer, 4, 1 ));
	dizzy_anim->currentAnimation->loop = true;
	dizzy_effect->isActive = false;

	// Animation - Dizzy
	path = "Archer_Dizzy" + subPath;
	dizzy = animator->addAnimation(path.c_str(), &colorKey, tRenderer, 1, 1, PLAYER_ANIMATION_DIZZY);

	// Network
	isNetworkStatic = false;

	// Create cannon
	cannon = new Cannon();
	cannon->isActive = false;

	// Setup audio player
	aPlayer = setComponent(new AudioPlayer());
	power_up_sf = aPlayer->addAudioToList(aPlayer->loadAudioFile("PowerUp22.wav"));
	aPlayer->setAudioToPlay(power_up_sf);
	aPlayer->pause();

	// Set attributes
	this->player_number = player_number;
}

// Hooks

	// General
bool Player::OnHandleEvent(const SDL_Event & event)
{
	return false;
}

void Player::onStart() 
{
}

void Player::onUpdate()
{
	// Update stun and return
	if (isStunned)
	{ 
		recover();
		return;
	}

	// Check sprite orientation
	double orientation = bow->transform.zRotation;

	if (orientation > 90 && orientation <= 270)
	{
		tRenderer->flip = SDL_FLIP_HORIZONTAL;
		pHand->getComponent<TextureRenderer>()->flip = SDL_FLIP_VERTICAL;

		// Change power_up position
		int pu_index = 0;
		for (auto power_up_pair : power_ups)
		{
			// Get power up display
			PowerUp* power_up = power_up_pair.second;

			// Find its position
			Vector2<float> pos = Vector2<float>( pu_index * 9 + 16, 0 );

			// It's odd, the second row
			if (pu_index & 1)
			{
				pos.y = 12;
				pos.x = (pu_index - 1) * 9 + 16;
			}

			// Set position
			power_up->time_display->transform.position = pos;

			// Increase index
			++pu_index;
		}
	}
	else
	{
		tRenderer->flip = SDL_FLIP_NONE;
		pHand->getComponent<TextureRenderer>()->flip = SDL_FLIP_NONE;

		// Change power_up position
		int pu_index = 0;
		for (auto power_up_pair : power_ups)
		{
			// Get power up display
			PowerUp* power_up = power_up_pair.second;

			// Find its position
			Vector2<float> pos = Vector2<float>( pu_index * (-9) - 9, 0 );

			// It's odd, then it goes in the second row
			if (pu_index & 1)
			{
				pos.y = 12;
				pos.x = (pu_index - 1) * (-9) - 9;
			}

			// Set position
			power_up->time_display->transform.position = pos;

			// Increase index
			++pu_index;
		}
	}

	// Update hands rotation
	pHand->transform.zRotation = orientation;
	rHand->transform.zRotation = orientation;

	// Get InputManager
	InputManager* input_mgr = InputManager::get();

	// Check cannon skill
	if (isSkillReady)
	{
		if (input_mgr->isKeyEvent(SDL_SCANCODE_E, network_owner, KeyEvent::DOWN))
		{
			withdrawCannon();
		}		
	}
	else if (isPlacingCannon)
	{
		// Flip cannon and player by mouse pos
		Vector2<float> mouse_pos = (Vector2<float>)input_mgr->getInputStatus(network_owner).mouse_pos / RendererManager::getScaler();
		Vector2<float> pos = getAbsolutePosition();

		if (mouse_pos.x > pos.x)
		{
			flipCannon(SDL_FLIP_NONE);
		}
		else
		{
			flipCannon(SDL_FLIP_HORIZONTAL);
		}

		// Check cannon placement
		if (input_mgr->isKeyEvent(SDL_SCANCODE_E, network_owner, KeyEvent::DOWN))
		{
			placeCannon();
		}
	}
	else if (isChargingCannon)
	{
		// Update charge bar value
		chargeBar->updateChargeValue();

		// Check for cannon shot
		if (input_mgr->isKeyEvent(SDL_SCANCODE_E, network_owner, KeyEvent::UP))
		{
			shootCannon();
		}
	}

	// Check if is AI before move
	if (mov_enabled)
	{
		if (!isAI)
		{
			// Check movement
			MovDirection dir = MOV_NONE;
			if (input_mgr->isKeyPressed(SDL_SCANCODE_A, network_owner))
				dir = MOV_DIR_LEFT;
			else if (input_mgr->isKeyPressed(SDL_SCANCODE_D, network_owner))
				dir = MOV_DIR_RIGHT;
			else
				stop();

			strafe(dir);
			
			// Check Jump and fastfall
			if (input_mgr->isKeyEvent(SDL_SCANCODE_W, network_owner, KeyEvent::DOWN))
			{
				jump();
			}
			if (input_mgr->isKeyEvent(SDL_SCANCODE_S, network_owner, KeyEvent::DOWN))
			{
				fast_fall();
			}
		}
	}

	// AI hook
	onPlayerUpdate();
}

	// Texture Renderer
void Player::onBlinkFinish()
{
	isInmunne = false;
}

	// Navigator

void Player::afterMove() 
{
	//airborne = true;
}

	// Collider
void Player::onColliderEnter(Collider *collider) 
{
	GameObject* owner = collider->gameObject;
	Tower* tower = dynamic_cast<Tower*>(collider->gameObject);
	
	if (tower)
	{

		if (collider->id == 1)
		{
			BoxCollider* boxCollider = static_cast<BoxCollider*>(collider);

			// Get diffs
			int over_diff = bCollider->cBottom - boxCollider->cTop;

			if (over_diff >= 0 && over_diff <= 6)
			{
				transform.position = Vector2<float>(transform.position.x, owner->transform.position.y + boxCollider->offset.y - this->tRenderer->texture.mHeight + 1);
				jump_nav->setDirection({ 0,0 });
				airborne = false;
			}
		}
	}
}

// Own methods

std::string Player::getPathByPlayerNumber(PlayerNumber number)
{
	std::string path = "";
	switch ((PlayerNumber)((int)number % 4))
	{
	case PlayerNumber::PLAYER_ONE:
		path = "_red";
		break;
	case PlayerNumber::PLAYER_TWO:
		path = "_blue";
		break;
	case PlayerNumber::PLAYER_THREE:
		path = "_yellow";
		break;
	}

	return path;
}

void Player::jump()
{
	if (!airborne)
	{
		jump_nav->setDirection({ 0,-1 });
		jump_nav->speed = 3;
		animator->isEnabled = false;
		airborne = true;
	}
}

void Player::fast_fall() 
{
	if (airborne)
	{
		jump_nav->setDirection(Vector2<float>(0, 1));
		jump_nav->speed = 3;
		animator->isEnabled = false;
	}
}

void Player::strafe(MovDirection dir)
{ 
	if (dir == MOV_NONE)
	{
		animator->isEnabled = false;
		return;
	}

	// Set sign
	float mov_dist = dir == MOV_DIR_RIGHT ? movement_speed : -movement_speed;

	// Enable mov animation
	animator->isEnabled = true;

	// Tweak navigator
	mov_nav->setDirection(Vector2<float>(mov_dist, 0));
	//transform.position = transform.position + Vector2<float>(mov_dist, 0);
}

void Player::stop()
{
	mov_nav->setDirection(Vector2<float>(0.0f, 0.0f));
}

// Status effects

void Player::stun(int duration) 
{
	// Check valid values
	if (duration <= 0)
		return;

	// OnStun Callback
	for (auto power_up_pair : power_ups)
	{
		PowerUp* power_up = power_up_pair.second;
		power_up->onStun();

		if (power_up->interruptDefaultAction())
			return;
	}

	// Update status effect data
	isStunned = true;
	stun_duration = duration;

	// Disable bow, arrow, hands and chargebar
	bow->isActive = false;
	if(bow->arrow)
		bow->arrow->isActive = false;
	rHand->isActive = false;
	pHand->isActive = false;
	chargeBar->disable();

	// Disable some cannon flags and cannon
	if (isPlacingCannon)
	{
		cannon->isActive = false;
	}
	// Lose cannon if hit while charging
	else if (isChargingCannon)
	{
		// Deactivate cannon
		cannon->isActive = false;

		// Reset state
		isChargingCannon = false;

		// Re-enable movement
		mov_enabled = true;
	}

	// Reset bow state if pulled
	if (bow->state != Bow::BowState::BOW_STATE_IDLE)
		bow->reset();

	// Enable dizzy animation
	animator->setCurrentAnimation(dizzy);
	animator->isEnabled = true;

	// Enable visual effect
	dizzy_effect->isActive = true;
}

void Player::knockback(Vector2<float> dir, float strength)
{
	// onKnockBack callback
	for(auto power_up_pair : power_ups)
	{
		PowerUp* power_up = power_up_pair.second;

		power_up->onKnockback();

		if (power_up->interruptDefaultAction())
			return;
	}

	// Enable nav
	knock_nav->isEnabled = true;

	// Knockback data
	Vector2<float> knock_dir(dir.x, 0.0f);
	knock_nav->setDirection(knock_dir);
	knock_nav->speed = strength;

	// Knockback Resistance
	knock_nav->acceleration = -knock_nav->getDirection();
	knock_nav->acceleration.normalize();
	knock_nav->acceleration = knock_nav->acceleration * 0.5f;
}

void Player::recover()
{
	// Reduce duration by one frame
	--stun_duration;

	if (!stun_duration)
	{
		// Disable flag
		isStunned = false;

		// Don't re-enable bow if was placing cannon
		if (!isPlacingCannon)
		{
			// Enable bow and arrow
			bow->isActive = true;
			if(bow->arrow)
				bow->arrow->isActive = true;
			rHand->isActive = true;
			pHand->isActive = true;
		}
		// Re-enable cannon 
		else
		{
			cannon->isActive = true;
		}

		// Disable visual effect
		dizzy_effect->isActive = false;

		// Disable animation
		animator->setCurrentAnimation(move);
	}
}

void Player::increase_skill_points(float skill_points)
{
	if (isPlacingCannon | isChargingCannon)
		return;

	this->skill_points += skill_points;

	if (this->skill_points >= 100.0f)
	{
		// Set to max
		this->skill_points = 100.0f;

		// Activate skill if not yet ready
		if (!isSkillReady)
		{
			// Set flag
			isSkillReady = true;

			// Play sound effect
			aPlayer->play();
		}
	}

	// Modify bar
	if(skill_bar)
		skill_bar->setHealthPercentage(this->skill_points, true);
}

void Player::addPowerUp(PowerUp* power_up)
{
	if (!power_up)
		return;

	// Get power-up if already exists
	if (power_ups.find(power_up->type) != power_ups.end())
	{
		// Get previous and reset duration
		PowerUp* prev_power_up = power_ups[power_up->type];
		prev_power_up->timer->extend(power_up->duration);

		// Onextend hook
		prev_power_up->onExtend();

		// Delete the recv one
		power_up->remove();

		// Return, nothing else to do
		return;
	}

	// Set this power_up
	auto pair = std::pair<PowerUpType, PowerUp*>(power_up->type, power_up);
	power_ups.insert(pair);

	// Set powerUpTimeDisplay parent and position
	power_up->time_display->transform.parent = &this->transform;

	// OnApply Callback
	power_up->onApply();
}

void Player::removePowerUp(PowerUp* power_up)
{
	if (power_ups.find(power_up->type) != power_ups.end())
	{
		// Remove from list
		power_ups.erase(power_up->type);

		// onRemove callback
		power_up->onRemove();
	}
}

bool Player::powerUpHook(Player::PowerUpHook hook)
{
	bool interrupt = false;
	for (auto power_up_pair : power_ups)
	{
		PowerUp* power_up = power_up_pair.second;

		std::invoke(hook, *power_up);

		// Check interrupt
		if (!interrupt)
			interrupt = power_up->interruptDefaultAction();
	}

	return interrupt;
}

// Cannon

bool Player::withdrawCannon()
{
	bool result = false;

	if (bow->state == Bow::BowState::BOW_STATE_IDLE)
	{
		if (!isInmunne)
		{
			// Create cannon and set this player to parent
			cannon->isActive = true;
			cannon->owner = this;
			cannon->transform.parent = &this->transform;
			cannon->transform.position = { 10, -4 };
			cannon->isBeingCarried = true;

			// Disable bow and hands
			bow->isActive = false;
			bow->arrow->isActive = false;
			rHand->isActive = false;
			pHand->isActive = false;

			// Set flag
			isPlacingCannon = true;
			isSkillReady = false;

			// Reset skill points and skill bar
			skill_points = 0;
			skill_bar->setHealthPercentage(0);

			// Set result
			result = true;
		}
	}

	return result;
}

bool Player::placeCannon()
{
	bool result = false;

	// Check position
	if (cannon->isCurrentPosValid())
	{
		if (!isInmunne)
		{
			// Place cannon and lock angle
			Vector2<float> cannon_pos = cannon->getAbsolutePosition();
			cannon->transform.parent = nullptr;
			cannon->transform.position = cannon_pos;
			cannon->isBeingCarried = false;

			// Set flags
			isPlacingCannon = false;
			isChargingCannon = true;

			// Start charge bar
			chargeBar->enable(2);

			// Disable movement
			mov_enabled = false;
			stop();

			// Set result
			result = true;
		}
	}

	return result;
}

bool Player::shootCannon()
{
	// Set flags
	isChargingCannon = false;

	// Set CannonBall dmg
	cannon->next_shot_dmg = chargeBar->getChargeValue() * 20;

	// Start shot
	cannon->animator->setCurrentAnimation(cannon->shot_animation);

	// Recover bow and hands
	bow->isActive = true;
	bow->arrow->isActive = true;
	rHand->isActive = true;
	pHand->isActive = true;

	// Disable ChargeBar
	chargeBar->disable();

	// Re-enable movement
	mov_enabled = true;

	return true;
}

void Player::flipCannon(SDL_RendererFlip flip)
{
	tRenderer->flip = flip;
	cannon->tRenderer->flip = flip;
	cannon->support->getComponent<TextureRenderer>()->flip = flip;

	if (flip == SDL_FLIP_NONE)
	{
		cannon->transform.position = { 10, -4 };

		// Reset rotation center
		delete cannon->transform.rotationCenter;
		cannon->transform.rotationCenter = new Vector2<int>(8, 15);
	}
	else
	{
		cannon->transform.position = Vector2<float>(-cannon->tRenderer->texture.mWidth, -4);

		// Flip rotation center
		delete cannon->transform.rotationCenter;
		cannon->transform.rotationCenter = new Vector2<int>(22, 15);
	}
}

// TODO - Los boundaries se calculan antes del navigator -> HACK: Anadir primero el navigator -> Solucion: Poner prioridad a los componentes.
// TODO - Navigator acceleration deberia cambiar speed y no vector -> HACK: Poner direction a 0, 0 cada vez que se choca con el suelo, +1 en posicion y para evitar flickering