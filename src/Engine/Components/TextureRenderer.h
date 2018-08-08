#pragma once
#include "Component.h"
#include "Texture.h"
#include <string>

class TextureRenderer : public Component
{
public:
	
	TextureRenderer();
	TextureRenderer(Texture texture);
	TextureRenderer(const char* path);

	// Attributes
	Texture texture;
	std::string tPath;

	SDL_Renderer *renderer = nullptr;

	// methods
	void update() override;
};