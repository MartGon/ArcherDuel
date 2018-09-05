#include "Player.h"

Player::Player()
{
	// Texture Renderer
	MapRGB *colorKey = new MapRGB();
	colorKey->blue = 0xFF;
	tRenderer = setComponent(new TextureRenderer("PlayerTrim.png", colorKey, 255));

	// BoxCollider
	bCollider = setComponent(new BoxCollider(36, 47));
	bCollider->offset = Vector2<float>(8, 0);
    bCollider->debug = true;
}

void Player::handleEvent(const SDL_Event & event)
{
    if (event.type != SDL_KEYDOWN)
        return;

    switch (event.key.keysym.sym)
    {
    case SDLK_d:
        transform.position = transform.position + Vector2<float>(5, 0);
        break;
    case SDLK_a:
        transform.position = transform.position + Vector2<float>(-5, 0);
        break;
    case SDLK_w:
        transform.position = transform.position + Vector2<float>(0, -5);
        break;
    case SDLK_s:
        transform.position = transform.position + Vector2<float>(0, 5);
        break;
    }
}
