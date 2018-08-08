#pragma once
#include "GameObject.h"
#include "TextureRenderer.h"
#include <functional>

class Button : public GameObject
{
public:
	// Constructors
	Button();
	Button(Texture texture);
	~Button();

	// Attributes
	Uint8 layer = 0;

	Collider *collider = nullptr;
	TextureRenderer *tRenderer = nullptr;

	// Methods
	void handleEvent(SDL_Event* e);
	const bool isWithinBoundaries(int mousePosX, int mousePosY);
	void onStart() override;

	// Listener
	std::function<void()> onClick;

	// set onClick
	void setOnClickListener(std::function<void()> callback);
};

