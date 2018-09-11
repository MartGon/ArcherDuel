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
		BOW_STATE_RELEASING,
		BOW_STATE_ARROW_LAUNCHED
	};

	// State
	BowState state = BOW_STATE_IDLE;

	// Components
	TextureRenderer* tRenderer = nullptr;
	Animator* animator = nullptr;
	BoxCollider* collider = nullptr;

	// Arrow
	GameObject* arrow = nullptr;

	// Animations
	Animation* pull = nullptr;
	Animation* rel = nullptr;

	// Shoot dir
    double angle = 0;
	Vector2<float> dir;
    double angle_inc = 5;

    // Gameplay
    int charge = 1;

	// Hooks
		// Behaviour
	void onUpdate() override;
		// Animation
	void beforeAnimationFrame(Animation* anim, int frameNumber) override;
	void onAnimationFinished(Animation* anim) override;
		// Event
	void handleEvent(const SDL_Event &event) override;

	// Own Methods
	Vector2<float> getArrowInitialPosition();
    void pointBowToMouse();
};
