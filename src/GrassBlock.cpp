#include "GrassBlock.h"

GrassBlock::GrassBlock(bool flip)
{
	// Texture Renderer
	tRenderer = setComponent(new TextureRenderer("HDGrass.png"));
	if (flip)
		tRenderer->flip = SDL_FLIP_HORIZONTAL;

	// Collider
	bCollider = setComponent(new BoxCollider(32, 32));
	//bCollider->debug = true;
}