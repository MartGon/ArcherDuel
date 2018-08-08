#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include "Vector2.h"

class Texture
{
public:
	// Constructors
	Texture();
	Texture(const char* resourcePath, SDL_Surface* screenSurface);
	Texture(const char* resourcePath, SDL_Renderer* renderer);
	~Texture();
	
	// SDL Stuff
	SDL_Texture *mTexture = nullptr;
	SDL_Surface *optimizedSurface = nullptr;
	SDL_Renderer *mRenderer = nullptr;

	// Dimensions
	int mWidth = 0;
	int mHeight = 0;

	// Scale
	Vector2<float> scale;

	// Other
	const char* folder = "resources/";
	const char* path = nullptr;

	// Methods

	void render(int x, int y);
	void setAlpha(Uint8 alpha);
	Uint8 getAlpha();
	void free();
	std::string getPathFromResourceFolder(const char* localPath);
};

