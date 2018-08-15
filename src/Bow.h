#pragma once
#include "GameObject.h"

class Bow : public GameObject
{
public:
	Bow();

	// States
	enum BowState 
	{
		BOW_STATE_IDLE,
		BOW_STATE_PULLING,
		BOW_STATE_PULLED,
		BOW_STATE_RELEASING
	};

	// State
	BowState state = BOW_STATE_IDLE;

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

private:
	Vector2<float> getArrowInitialPosition();
};