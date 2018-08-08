#include "Transform.h"

Transform::Transform() : Component()
{
	position = Vector2<float>(0, 0);
	scale = Vector2<float>(1, 1);
	zRotation = 0;
}