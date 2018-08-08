#include "Collider.h"
#include "GameObject.h"
#include <algorithm>

// Attributes
std::vector<Collider*> Collider::gColliders = std::vector<Collider*>();

Collider::Collider() : Component()
{

}

Collider::Collider(Texture texture) : Collider(texture.mWidth, texture.mHeight)
{
}

Collider::Collider(int width, int height) : Component()
{
	// Set values
	cWidth = width;
	cHeight = height;

	// Add to static list
	gColliders.push_back(this);
}

void Collider::calculateColliderBoundaries()
{
	Vector2<float> position = gameObject->transform.position;
	Vector2<float> dimensions = getDimensions();
	Vector2<float> offsetVector = getOffsetVector();

	int fWidth = dimensions.x;
	int fHeight = dimensions.y;
	int xOffset = offsetVector.x;
	int yOffset = offsetVector.y;

	cLeft = position.x + xOffset;
	cRight = position.x + fWidth + xOffset;
	cTop = position.y + yOffset;
	cBottom = position.y + fHeight + yOffset;
}

Vector2<float> Collider::getCollisionCenter()
{
	int x = (float)(cLeft + cRight) / 2;
	int y = (float)(cTop + cBottom) / 2;

	return Vector2<float>(x, y);
}

void Collider::update()
{
	calculateColliderBoundaries();

	for (auto const& collider : gColliders)
	{
		if (this->gameObject == collider->gameObject)
			continue;
		else if (isCollidingWith(collider))
			gameObject->onColliderEnter(collider);
	}
}

bool Collider::isCollidingWith(Collider *collider)
{
	if (cRight <= collider->cLeft)
		return false;

	if (cLeft >= collider->cRight)
		return false;

	if (cBottom <= collider->cTop)
		return false;

	if (cTop >= collider->cBottom)
		return false;

	return true;
}

void Collider::drawCollisionBoundaries(SDL_Renderer *renderer)
{
	SDL_Rect rect;
	rect.x = cLeft;
	rect.w = cRight - cLeft;
	rect.y = cBottom;
	rect.h = cTop - cBottom;
	SDL_RenderDrawRect(renderer, &rect);
}

void Collider::destroy()
{
	gColliders.erase(std::remove(gColliders.begin(), gColliders.end(), this), gColliders.end());
}

Vector2<float> Collider::getDimensions()
{
	int fWidth = cWidth * gameObject->transform.scale.x;
	int fHeight = cHeight * gameObject->transform.scale.y;

	return Vector2<float>(fWidth, fHeight);
}

Vector2<float> Collider::getOffsetVector()
{
	int xOffset = offset.x * gameObject->transform.scale.x;
	int yOffset = offset.y * gameObject->transform.scale.y;

	return Vector2<float>(xOffset, yOffset);
}