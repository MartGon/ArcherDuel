#include "Arrow.h"
#include "Player.h"
#include "LevelOne.h"

Arrow::Arrow()
{
	// Setting components
	name = "arrow";

	// Texture Renderer
	MapRGB *colorKey = new MapRGB();
	colorKey->blue = 0xFF;
	tRenderer = setComponent(new TextureRenderer("ArrowTrim.png", colorKey, 255));

	// Set scale
	transform.scale = Vector2<float>(0.5f, 0.5f);

	// Navigator
	nav = setComponent(new Navigator());
	nav->isEnabled = false;

	// Rotatable Box Collider
	rotCollider = setComponent(new RotatableBoxCollider(Vector2<int>(0, 0), Vector2<int>(0, 3 * transform.scale.y), 
		Vector2<int>(14 * transform.scale.x, 0), Vector2<int>(14 * transform.scale.x, 3 * transform.scale.y)));
	rotCollider->isEnabled = false;
    //rotCollider->debug = true;
}

// Hooks

void Arrow::onColliderEnter(Collider* collider)
{
	printf("Arrow Collision\n");

	nav->isEnabled = false;
	rotCollider->isEnabled = false;

	if (Player* player = dynamic_cast<Player*>(collider->gameObject))
	{
		transform.position = getAbsolutePosition() - player->getAbsolutePosition();
		transform.parent = &player->transform;
	}
}

void Arrow::afterMove()
{
	transform.zRotation = nav->getDirection().getAngle();
}

void Arrow::onUpdate()
{
	
}

// TODO - Los colliders de la flecha no aparecen ir acordes al movimiento. HACK: Poner el navigator antes del collider