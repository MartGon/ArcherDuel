#pragma once
#include "Navigator.h"
#include "GameObject.h"

class Arrow : public GameObject
{
public:
	Arrow();

	// Components
	TextureRenderer* tRenderer = nullptr;
	RotatableBoxCollider* rotCollider = nullptr;
	Navigator *nav = nullptr;

	// Hooks
	void onColliderEnter(Collider* collider);
};