#pragma once
#include "GameObject.h"

class Bow : public GameObject
{
public:
	Bow();

	// Components
	TextureRenderer* tRenderer = nullptr;
	Animator* animator = nullptr;

	// Arrow
	GameObject* arrow = nullptr;

	// Animations
	Animation* pull = nullptr;
	Animation* rel = nullptr;

	// Hooks
		// Behaviour
	void onUpdate() override;
		// Animation
	void beforeAnimationFrame(Animation* anim, int frameNumber) override;
	void onAnimationFinished(Animation* anim) override;
		// Event
	void handleEvent(const SDL_Event &event) override;
};