#pragma once
#include "GameObject.h"
#include "Timer.h"
#include "TimerHandler.h"

#include "TextLabel.h"

class CannonBall : public GameObject
{
public:
	CannonBall();

	// Components
	TextureRenderer* tRenderer = nullptr;
	Navigator* nav = nullptr;
	BoxCollider* bCollider = nullptr;

	// Timer
	TimerComponent* timer = nullptr;

	// Member
	TextLabel* dmg_label = nullptr;

	// Attributes
	Uint32 dmg = 20;

	// Overrided Methods
	void onColliderEnter(Collider* collider);
	void onTimerEnd(Uint32 flag);
	void onVanish();

};