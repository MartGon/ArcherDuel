#pragma once
#include "GameObject.h"
#include "Arrow.h"
#include "AudioPlayer.h"

class Player;
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
	AudioPlayer* aPlayer = nullptr;

	// Arrow
	Arrow* arrow = nullptr;

	// Animations
	Animation* pull = nullptr;
	Animation* rel = nullptr;

	// Audio
	int pull_index = 0;
	int rel_index = 0;
	int rel_special_index = 0;

	// Shoot dir
    double angle = 0;
	Vector2<float> dir;
    double angle_inc = 5;

	// GamePlay
	bool instant_cast = false;

    // Owner
    Player* owner = nullptr;

	// Hooks
		// Behaviour
    void onStart() override;
	void onUpdate() override;
		// Animation
	void beforeAnimationFrame(Animation* anim, int frameNumber) override;
	void onAnimationFinished(Animation* anim) override;
		// Event
	bool OnHandleEvent(const SDL_Event &event) override;

	// Own Methods
	Vector2<float> getArrowInitialPosition(bool reversed = false);
	void reset();

	// Aiming
	void aimBow(Vector2<float> target);
    void pointBowToMouse();

	// Arrow
	void launchArrow(Arrow* arrow, float charge);
    void rotateArrow();
	Arrow* loadArrow();

	// Shooting
	void draw();
	void shoot();
};
