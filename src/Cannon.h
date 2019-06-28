#pragma once
#include "GameObject.h"
#include "AudioPlayer.h"

#include "Player.h"

#include <functional>

class Cannon : public GameObject
{
public:
	Cannon();

	// Attributes
	bool isBeingCarried = false;
	Uint32 next_shot_dmg = 20;

	// References
	Player* owner = nullptr;

	// Components
	TextureRenderer* tRenderer = nullptr;
	Animator* animator = nullptr;
	AudioPlayer* aPlayer = nullptr;

		// Animations
	Animation* shot_animation = nullptr;
	Animation* idle_animation = nullptr;

		// Audios
	int fuse_index = 0;
	int shot_index = 0;

	// Members
	GameObject* support = nullptr;

	// Methods
	bool isCurrentPosValid();
	void aim(Vector2<float> target);
	void shoot();

	// Overrided methods

		// General
	void onUpdate() override;

		// Animation
	void beforeAnimationFrame(Animation* anim, int frame_index) override;
	void onAnimationFinished(Animation* anim) override;

		// TextureRenderer
	void onVanish() override;

	// Callbacks
	std::function<void()> onCannonShoot;
};