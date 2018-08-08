#pragma once
#include "Updateable.h"

// Class to wrap al the different kinds of components
class GameObject;

class Component : public Updateable
{
public:
	Component();
	~Component();

	// Attributes
	bool isEnabled = true;
	GameObject *gameObject = nullptr;

	// Methods
	virtual void update() override {};
	virtual void destroy() {};
};

