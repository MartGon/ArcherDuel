#pragma once
#include "Component.h"
#include "Vector2.h"
#include "Texture.h"
#include <vector>

class Collider : public Component
{
public:
	Collider();
	Collider(int width, int height);
	Collider(Texture texture);

	// MeanWhile
	static std::vector<Collider*> gColliders;

	// Collider boundaries
	int cTop = 0;
	int cBottom = 0;
	int cRight = 0;
	int cLeft = 0;

	// Collider dimensions
	int cWidth = 0;
	int cHeight = 0;

	// Collider offset
	Vector2<float> offset;

	// Methods
	void calculateColliderBoundaries();
	Vector2<float> getCollisionCenter();
	bool isCollidingWith(Collider *collider);
	Vector2<float> getDimensions();
	Vector2<float> getOffsetVector();

	// Debug
	void drawCollisionBoundaries(SDL_Renderer* renderer);

	// Overrided
	void update() override;
	void destroy() override;
};