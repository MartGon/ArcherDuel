#pragma once
#include "Navigator.h"
#include "GameObject.h"
#include "TextLabel.h"
#include "AudioPlayer.h"
#include "TimerHandler.h"

class Bow;
class Arrow : public GameObject, TimerHandler
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
	AudioPlayer* aPlayer = nullptr;

	// Audio files
	int audio_impact_building_1 = 0;
	int audio_impact_building_2 = 0;
	int audio_impact_player = 0;

	// Hooks
	void onColliderEnter(Collider* collider) override;
    void afterMove() override;
    void onUpdate() override;
	void onVanish() override;
	void onTimerFinish(void* param) override;
};