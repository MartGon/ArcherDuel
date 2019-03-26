#pragma once
#include "Navigator.h"
#include "GameObject.h"
#include "TextLabel.h"

class Bow;
class Arrow : public GameObject
{
public:
	Arrow();

    // Attributes
    bool isPinned = false;

    // GameObjects
    Bow* bow = nullptr;
	TextLabel* dmg_label = nullptr;
	TextureRenderer* dmg_tRenderer = nullptr;

	// Components
	TextureRenderer* tRenderer = nullptr;
	RotatableBoxCollider* rotCollider = nullptr;
	Navigator *nav = nullptr;

	// Hooks
	void onColliderEnter(Collider* collider) override;
    void afterMove() override;
    void onUpdate() override;
};