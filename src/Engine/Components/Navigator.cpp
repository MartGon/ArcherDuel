#include "Navigator.h"
#include "GameObject.h"

// Constructors

Navigator::Navigator()
{
	direction = Vector2<float>();
	speed = 1;
}

Navigator::Navigator(Vector2<float> direction, int speed) : Component()
{
	this->direction = direction;
	this->speed = speed;
}

void Navigator::update()
{
	// Hook
	gameObject->beforeMove();

	Vector2<float> pos = gameObject->transform.position;
	Vector2<float> displacement = (direction * speed);
	Vector2<float> newPos = pos + displacement;
	gameObject->transform.position = newPos;

	// Hook
	gameObject->afterMove();
}

Vector2<float> Navigator::getDirection()
{
	return direction;
}

void Navigator::setDirection(Vector2<float> dir)
{
	dir.normalize();
	direction = dir;
}