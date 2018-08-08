#pragma once
#include "Component.h"
#include "Vector2.h"

// Class to handle position, scale and rotation
class Transform : public Component
{
public:
	Transform();

	Vector2<float> position;
	Vector2<float> scale;
	double zRotation;
	
	void update() override {};
};