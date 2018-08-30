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
}