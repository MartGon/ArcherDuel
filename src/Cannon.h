#pragma once
#include "GameObject.h"
#include "AudioPlayer.h"

class Cannon : public GameObject
{
public:
	Cannon();

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
	void aim(Vector2<float> target);
	void shoot();

	// Overrided methods

		// General
	void onUpdate() override;

		// Animation
	void beforeAnimationFrame(Animation* anim, int frame_index) override;
	void onAnimationFinished(Animation* anim) override;
};