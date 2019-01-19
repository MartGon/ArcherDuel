#include "Player.h"
#include "LevelOne.h"

Player::Player()
{
	// Texture Renderer
	MapRGB *colorKey = new MapRGB();
	colorKey->green = 255;
	tRenderer = setComponent(new TextureRenderer("Archer.png", colorKey, 255));

	// BoxCollider
	bCollider = setComponent(new BoxCollider(5, 15));
	bCollider->offset = Vector2<float>(3, 4);
    bCollider->debug = false;
}

void Player::handleEvent(const SDL_Event & event)
{
    if (event.type != SDL_KEYDOWN)
        return;

	//if (!level)
	//	return;

    //if (!level->canPlayerAct(this))
    //    return;

    if (mov_enabled)
    {
        switch (event.key.keysym.sym)
        {
        case SDLK_d:
            transform.position = transform.position + Vector2<float>(movement_speed, 0);
            break;
        case SDLK_a:
            transform.position = transform.position + Vector2<float>(-movement_speed, 0);
            break;
        case SDLK_w:
            transform.position = transform.position + Vector2<float>(0, -movement_speed);
            break;
        case SDLK_s:
            transform.position = transform.position + Vector2<float>(0, movement_speed);
            break;
        }
    }
}
