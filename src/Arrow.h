#pragma once
#include "Navigator.h"
#include "GameObject.h"

class Bow;
class Arrow : public GameObject
{
public:
	Arrow();

    // Attributes
    bool mov_finished = false;

    // GameObjects
    Bow* bow = nullptr;

	// Components
	TextureRenderer* tRenderer = nullptr;
	RotatableBoxCollider* rotCollider = nullptr;
	Navigator *nav = nullptr;

	// Hooks
	void onColliderEnter(Collider* collider) override;
    void afterMove() override;
    void onUpdate() override;
};