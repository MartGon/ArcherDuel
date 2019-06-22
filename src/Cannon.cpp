#include "Cannon.h"

#include "InputManager.h"
#include "SceneManager.h"

#include "Navigator.h"

#include "CannonBall.h"
#include "Tower.h"

Cannon::Cannon()
{
	// Texture Renderer
	MapRGB colorKey;
	colorKey.green = 255;
	tRenderer = setComponent(new TextureRenderer("Cannon_idle.png", &colorKey, 254));

	// Animator
	animator = setComponent(new Animator());

	// Shoot Animation 
	shot_animation = animator->addAnimation("Cannon_shot", &colorKey, tRenderer, 27, 1);

	for (auto &frame : shot_animation->frames)
	{
		frame->duration = 10;
	}

	// CannonBall is shot
	for (int i = 22; i < 26; ++i)
	{
		shot_animation->frames[i]->duration = 3;
	}

	// Return to original pos
	for (int i = 25; i < shot_animation->frames.size(); ++i)
	{
		shot_animation->frames[i]->duration = 10;
	}

	// Idle Animation
	idle_animation = new Animation();
	Frame* idle_frame = new Frame();
	idle_frame->texture = tRenderer->texture;
	idle_animation->frames.push_back(idle_frame);
	idle_animation->loop = false;
	idle_animation->tRenderer = tRenderer;
	idle_animation->name = "Idle";

	idle_animation = animator->addAnimation(idle_animation);

	animator->setCurrentAnimation(idle_animation);

	// Support
	support = new GameObject();
	support->setComponent(new TextureRenderer("Cannon_support.png", &colorKey, 254));
	
	support->transform.parent = &transform;

	// Set rotation center
	transform.rotationCenter = new Vector2<int>(8, 15);

	// Audio Player
	aPlayer = setComponent(new AudioPlayer());
	fuse_index = aPlayer->addAudioToList(aPlayer->loadAudioFile("fuse.wav"));
	shot_index = aPlayer->addAudioToList(aPlayer->loadAudioFile("Cannon_2.wav"));

	// Set red color only
	MapRGB red = { 255, 0, 0 };
	tRenderer->texture.color_mod = red;
	support->getComponent<TextureRenderer>()->texture.color_mod = red;
}

void Cannon::onUpdate()
{
	InputManager* input_mgr = InputManager::get();

	if (isBeingCarried)
	{
		// Aim to mouse
		Vector2<float> mouse_pos = input_mgr->getInputStatus(owner->network_owner).mouse_pos;
		Vector2<float> dest = mouse_pos / RendererManager::getScaler();
		aim(dest);

		// Set normal color when is valid
		if (isCurrentPosValid())
		{
			MapRGB normal = { 255, 255, 255 };
			tRenderer->texture.color_mod = normal;
			support->getComponent<TextureRenderer>()->texture.color_mod = normal;
		}
		// Set red color otherwise
		else
		{
			MapRGB red = { 255, 0, 0 };
			tRenderer->texture.color_mod = red;
			support->getComponent<TextureRenderer>()->texture.color_mod = red;
		}
	}
}

void Cannon::aim(Vector2<float> target)
{
	// Calculate vector
	Vector2<float> center = getAbsoluteRotationCenter();
	Vector2<float> dir = (target - center);
	dir.normalize();

	double angle = dir.getAngle();

	if (tRenderer->flip == SDL_FLIP_NONE)
	{
		if (angle < -45 || angle > 45)
			return;
	}
	else
	{
		if (angle < 135 || angle > 225)
			return;

		// Get Complementary
		angle -= 180;
	}

	// Rotate Bow
	transform.zRotation = angle;
}

void Cannon::shoot()
{
	CannonBall* ball = new CannonBall();

	// Spawn ball
	Vector2<float> offset = tRenderer->flip == SDL_FLIP_NONE ? Vector2<float>{ 26, 11 } : Vector2<float>{4, 11};
	Vector2<float> ball_pos = getAbsolutePosition() + offset;
	ball->transform.position = Utilities::rotatePointFromCenter(getAbsoluteRotationCenter(), transform.zRotation, ball_pos);

	// Start ball movement
	Vector2<float> dir = tRenderer->flip == SDL_FLIP_NONE ? Vector2<float>(transform.zRotation) : -Vector2<float>(transform.zRotation);
	ball->nav->setDirection(dir);
	ball->nav->speed = 20;

	// Set cannon ball dmg
	ball->dmg = next_shot_dmg;

	// Update collider
	ball->bCollider->update();

	// Set trail effect
	ball->tRenderer->hasTrailEffect = true;
}

bool Cannon::isCurrentPosValid()
{
	if (!owner)
		return false;

	Vector2<float> pos = getAbsolutePosition();
	pos.x = tRenderer->flip == SDL_FLIP_NONE ? pos.x + 9 : pos.x + 21;
	pos.y = pos.y + tRenderer->texture.mHeight - 1;

	if (Tower* player_tower = owner->tower)
	{
		if (BoxCollider* floor = player_tower->getComponent<BoxCollider>())
		{
			if (pos.x > floor->cRight)
				return false;
			if (pos.x < floor->cLeft)
				return false;
			if (pos.y != floor->cTop)
				return false;

			return true;
		}
	}
}

void Cannon::beforeAnimationFrame(Animation* anim, int frame_index)
{
	if (anim->id == shot_animation->id)
	{
		if (frame_index == 1)
		{
			// Audio
			aPlayer->setAudioToPlay(fuse_index);
			aPlayer->loop = true;
			aPlayer->play();
		}
		else if (frame_index == 22)
		{
			// Shoot
			shoot();

			// Audio
			aPlayer->setAudioToPlay(shot_index);
			aPlayer->loop = false;
			aPlayer->play();
		}
	}
}

void Cannon::onAnimationFinished(Animation* anim)
{
	if (anim->id == shot_animation->id)
	{
		// Return to idle animation
		animator->setCurrentAnimation(idle_animation);

		// Star to vanish
		tRenderer->isVanishing = true;
		support->getComponent<TextureRenderer>()->isVanishing = true;
	}
}

void Cannon::onVanish()
{
	// Deactivate
	isActive = false;

	// Reset flag
	tRenderer->isVanishing = false;
	support->getComponent<TextureRenderer>()->isVanishing = false;

	// Reset alpha values
	tRenderer->alpha = SDL_ALPHA_OPAQUE;
	support->getComponent<TextureRenderer>()->alpha = SDL_ALPHA_OPAQUE;
}