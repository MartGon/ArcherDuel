#pragma once
#include "Texture.h"
#include "Vector2.h"
#include "Transform.h"
#include "Collider.h"
#include <string>
#include <vector>

class Component;
class GameObject : public Updateable
{
public:
	GameObject();
	GameObject(Texture texture);
	~GameObject();

	// Network Flag
	bool updateFromClient = false;

	// Name
	std::string name = "";

	// Id
	Uint16 id = 0;

	// Active
	bool isActive = true;

	// Transform
	Transform transform;

	// Methods

	// Init
	void start();
	virtual void onStart();

	// setComponent
	void setComponent(Component *component);

	// getComponent
	template <typename T>
	T* getComponent()
	{
		for (auto component : components)
		{
			if (T* subComponent = dynamic_cast<T*>(component))
			{
				return subComponent;
			}
		}

		return nullptr;
	}

	// Position
	void setRelativePosition(Vector2<float> pos);

	// Update
	virtual void onUpdate();
	void update() override;

	// Scale 
	void setScale(Vector2<float> scale);

	// Cleaning
	virtual void destroy();

	// Hooks
		// Collisions
	virtual void onColliderEnter(Collider *collider);
		// Movement
	virtual void beforeMove();
	virtual void afterMove();

private:

	// Components
	std::vector<Component*> components;

	// Bool
	bool isInitialized = false;
};

