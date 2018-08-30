#include "Arrow.h"
#include "Player.h"

Arrow::Arrow()
{
	// Setting components

	// Texture Renderer
	MapRGB *colorKey = new MapRGB();
	colorKey->blue = 0xFF;
	tRenderer = setComponent(new TextureRenderer("ArrowTrim.png", colorKey));

	// Rotatable Box Collider
	rotCollider = setComponent(new RotatableBoxCollider(Vector2<int>(0, 0), Vector2<int>(0, 3), Vector2<int>(14, 0), Vector2<int>(14, 3)));

	// Navigator
	nav = setComponent(new Navigator());
	nav->isEnabled = false;

	// Rotation
	transform.rotationCenter = new Vector2<int>(2, 2);
}

// Hooks

void Arrow::onColliderEnter(Collider* collider)
{
	if (Player* player = dynamic_cast<Player*>(collider->gameObject))
	{
		nav->isEnabled = false;
		rotCollider->isEnabled = false;
	}
}