#pragma once
#include "Navigator.h"
#include "GameObject.h"
#include "TextLabel.h"
#include "AudioPlayer.h"
#include "TimerHandler.h"
#include "Timer.h"

class Player;
class Bow;
class Arrow : public GameObject
{
public:
	Arrow();

    // GameObjects
	Player* owner = nullptr;
    Bow* bow = nullptr;
	TextLabel* dmg_label = nullptr;
	TextureRenderer* dmg_tRenderer = nullptr;

	// Components
	TextureRenderer* tRenderer = nullptr;
	RotatableBoxCollider* rotCollider = nullptr;
	Navigator *nav = nullptr;
	AudioPlayer* aPlayer = nullptr;
	TimerComponent* timer = nullptr;

	// Audio files
	int audio_impact_building_1 = 0;
	int audio_impact_building_2 = 0;
	int audio_impact_player = 0;

	// Hooks
	void onColliderEnter(Collider* collider) override;
    void afterMove() override;
    void onUpdate() override;
	void onVanish() override;
	void onTimerEnd(Uint8 flag) override;

	// Own Hooks
	virtual void onImpactGameObject(Collider* go) {};
};

class FireArrow : public Arrow
{
public:
	FireArrow();

	// Components
	Animator* animator = nullptr;

	// Animation
	Animation* idle = nullptr;

	// Overrided Methods
	void onImpactGameObject(Collider* col) override;
};