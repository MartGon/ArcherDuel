#include "Player.h"
#include "LevelOne.h"
#include "Tower.h"

Player::Player()
{
	// Texture Renderer
	MapRGB *colorKey = new MapRGB();
	colorKey->green = 255;
	tRenderer = setComponent(new TextureRenderer("Archer.png", colorKey, 254));

	// Move Navigator
	nav = setComponent(new Navigator(Vector2<float>(0, 0), 1, true, Vector2<float>(0, -0.15)));
	nav->isEnabled = true;
	//nav->isEnabled = false;

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
	move = animator->addAnimation("Archer_Walk", colorKey, tRenderer, 4, 1, PLAYER_ANIMATION_WALK);
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
	bow->transform.rotationCenter = new Vector2<int>(-5, 7);
	
	// Offset is 11, 4
	Vector2<float> offset(11, 4);
	bow->transform.position = offset;

	// Add player pHand
	pHand = new GameObject();
	pHand->setComponent(new TextureRenderer("ArcherHand.png", colorKey, 255));
	pHand->setScale(Vector2<float>(0.65f, 0.65f));
	pHand->transform.parent = &this->transform;
	pHand->transform.position = Vector2<float>(13, 11);
	pHand->setAbsoluteRotationCenter(bow->getAbsoluteRotationCenter());

	// Add player rHand
	rHand = new GameObject();
	rHand->setComponent(new TextureRenderer("ArcherHand.png", colorKey, 252));
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
	TextureRenderer* dizzy_tRenderer = dizzy_effect->setComponent(new TextureRenderer("dizzy_effect1.png", colorKey, 255));
	Animator* dizzy_anim = dizzy_effect->setComponent(new Animator("dizzy_effect", colorKey, dizzy_tRenderer, 4, 1 ));
	dizzy_anim->currentAnimation->loop = true;
	dizzy_effect->isActive = false;

	// Animation - Dizzy
	dizzy = animator->addAnimation("Archer_Dizzy_", colorKey, tRenderer, 1, 1, PLAYER_ANIMATION_DIZZY);
}

// Hooks

	// General
void Player::handleEvent(const SDL_Event & event)
{
	if (isAI)
		return;

	if (isStunned)
		return;

    if (event.type != SDL_KEYDOWN)
        return;

    if (mov_enabled)
    {
        switch (event.key.keysym.sym)
        {
        case SDLK_w:
			jump();
            break;
        case SDLK_s:
			fast_fall();
            break;
        }
    }
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

	const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

	// Check if is AI before move
	if (!isAI)
	{
		MovDirection dir = MOV_NONE;
		// Check movement
		if (currentKeyStates[SDL_SCANCODE_A])
			dir = MOV_DIR_LEFT;
		else if (currentKeyStates[SDL_SCANCODE_D])
			dir = MOV_DIR_RIGHT;

		strafe(dir);
	}
	else
		// AI hook
		onPlayerUpdate();

	// Check sprite orientation
	double orientation = bow->transform.zRotation;

	if (orientation > 90 && orientation <= 270)
	{
		tRenderer->flip = SDL_FLIP_HORIZONTAL;
		pHand->getComponent<TextureRenderer>()->flip = SDL_FLIP_VERTICAL;
	}
	else
	{
		tRenderer->flip = SDL_FLIP_NONE;
		pHand->getComponent<TextureRenderer>()->flip = SDL_FLIP_NONE;
	}

	// Update hands rotation
	pHand->transform.zRotation = orientation;
	rHand->transform.zRotation = orientation;
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
		float height = owner->transform.position.y + collider->offset.y - 19 + 1;

		// Hack to allow the AI to jump
		if (height == transform.position.y)
			return;

		transform.position = Vector2<float>(transform.position.x, height);

		nav->setDirection(Vector2<float>(0, 0));
		airborne = false;
	}
}

// Own methods

void Player::jump()
{
	if (!airborne)
	{
		nav->setDirection(Vector2<float>(0, -1));
		nav->speed = 3;
		animator->isEnabled = false;
		airborne = true;
	}
}

void Player::fast_fall() 
{
	if (airborne)
	{
		nav->setDirection(Vector2<float>(0, 1));
		nav->speed = 3;
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

	// Mov distance
	transform.position = transform.position + Vector2<float>(mov_dist, 0);
}

// Status effects

void Player::stun(int duration) 
{
	// Check valid values
	if (duration <= 0)
		return;

	// Update status effect data
	isStunned = true;
	stun_duration = duration;

	// Disable bow, arrow, hands and chargebar
	bow->isActive = false;
	bow->arrow->isActive = false;
	rHand->isActive = false;
	pHand->isActive = false;
	chargeBar->disable();

	// Reset bow state if pulled
	if (bow->state = bow->BOW_STATE_PULLED)
		bow->reset();

	// Enable dizzy animation
	animator->setCurrentAnimation(dizzy);
	animator->isEnabled = true;

	// Enable visual effect
	dizzy_effect->isActive = true;
}

void Player::knockback(Vector2<float> dir, float strength)
{
	// Enable nav
	knock_nav->isEnabled = true;

	// Knockback data
	knock_nav->setDirection(dir);
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

		// Enable bow and arrow
		bow->isActive = true;
		bow->arrow->isActive = true;
		rHand->isActive = true;
		pHand->isActive = true;

		// Disable visual effect
		dizzy_effect->isActive = false;

		// Disable animation
		animator->setCurrentAnimation(move);
	}
}

// TODO - Los boundaries se calculan antes del navigator -> HACK: Anadir primero el navigator -> Solucion: Poner prioridad a los componentes.
// TODO - Navigator acceleration deberia cambiar speed y no vector -> HACK: Poner direction a 0, 0 cada vez que se choca con el suelo, +1 en posicion y para evitar flickering