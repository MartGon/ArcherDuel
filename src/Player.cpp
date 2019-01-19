#include "Player.h"
#include "LevelOne.h"
#include "Tower.h"

Player::Player()
{
	// Texture Renderer
	MapRGB *colorKey = new MapRGB();
	colorKey->green = 255;
	tRenderer = setComponent(new TextureRenderer("Archer.png", colorKey, 255));

	// Navigator
	nav = setComponent(new Navigator(Vector2<float>(0, 0), 1));
	nav->isKinematic = true;
	nav->acceleration = Vector2<float>(0, -0.05);
	nav->isEnabled = true;

	// BoxCollider
	bCollider = setComponent(new BoxCollider(5, 15));
	bCollider->offset = Vector2<float>(3, 4);
    bCollider->debug = true;

	// Animator
	animator = setComponent(new Animator());
	move = animator->addAnimation("Archer_Walk", colorKey, tRenderer, 4, 1, PLAYER_ANIMATION_WALK);
	move->loop = true;
	animator->setCurrentAnimation(move);
	//animator->isEnabled = false;

	for (auto &frame : move->frames)
	{
		frame->duration = 6;
	}
	//move->frames.at(0)->duration = 10;
	//move->frames.at(2)->duration = 10;
}

// Hooks

	// General
void Player::handleEvent(const SDL_Event & event)
{
    if (event.type != SDL_KEYDOWN)
        return;

    if (mov_enabled)
    {
        switch (event.key.keysym.sym)
        {
        case SDLK_w:
			// Jump
			if (!airborne)
			{
				nav->setDirection(Vector2<float>(0, -1));
				nav->speed = 3;
				animator->isEnabled = false;
			}
            break;
        case SDLK_s:
			if (airborne)
			{
				nav->setDirection(Vector2<float>(0, 1));
				nav->speed = 5;
				animator->isEnabled = false;
			}
            break;
        }
    }
}

void Player::onUpdate()
{
	const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

	if (currentKeyStates[SDL_SCANCODE_A])
	{
		animator->isEnabled = true;
		transform.position = transform.position + Vector2<float>(-movement_speed, 0);
	}
	else if (currentKeyStates[SDL_SCANCODE_D])
	{
		animator->isEnabled = true;
		transform.position = transform.position + Vector2<float>(movement_speed, 0);
	}
	else
		animator->isEnabled = false;
}

	// Navigator

void Player::afterMove() 
{
	airborne = true;
}

	// Collider
void Player::onColliderEnter(Collider *collider) 
{
	GameObject* owner = collider->gameObject;

	if (owner)
	{
		float height = owner->transform.position.y + collider->offset.y;
		transform.position = Vector2<float>(transform.position.x, height - 19 + 1);

		nav->setDirection(Vector2<float>(0, 0));
		airborne = false;
	}
}

// TODO - Los boundaries se calculan antes del navigator -> HACK: Anadir primero el navigator -> Solucion: Poner prioridad a los componentes.
// TODO - Aun asi, se renderiza antes de chequear las colisones -> HACK: Poner primer el CollisionManager -> Problema: No se pueden ver los boundaries -> Solucion: Dibujar en RendererManager
// TODO - Navigator acceleration deberia cambiar speed y no vector -> HACK: Poner direction a 0, 0 cada vez que se choca con el suelo, +1 en posicion y para evitar flickering