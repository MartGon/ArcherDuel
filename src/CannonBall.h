#pragma once
#include "GameObject.h"
#include "Timer.h"
#include "TimerHandler.h"

#include "TextLabel.h"

class CannonBall : public GameObject, TimerHandler
{
public:
	CannonBall();

	// Components
	TextureRenderer* tRenderer = nullptr;
	Navigator* nav = nullptr;
	BoxCollider* bCollider = nullptr;

	// Timer
	Timer* timer = nullptr;

	// Member
	TextLabel* dmg_label = nullptr;

	// Overrided Methods
	void onColliderEnter(Collider* collider);
	void onTimerFinish(void *param);
	void onVanish();

};