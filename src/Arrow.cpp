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

	// Rotatable Box Collider
	rotCollider = setComponent(new RotatableBoxCollider(Vector2<int>(0, 0), Vector2<int>(0, 3 * transform.scale.y), 
		Vector2<int>(14 * transform.scale.x, 0), Vector2<int>(14 * transform.scale.x, 3 * transform.scale.y)));
	rotCollider->isEnabled = false;
    rotCollider->debug = true;

	// Navigator
	nav = setComponent(new Navigator());
	nav->isEnabled = false;

	// Rotation
	//transform.rotationCenter = new Vector2<int>(0, 0);
}

// Hooks

void Arrow::onColliderEnter(Collider* collider)
{
	printf("Arrow Collision\n");

	//if (Player* player = dynamic_cast<Player*>(collider->gameObject))
	//{
		nav->isEnabled = false;
		rotCollider->isEnabled = false;
        mov_finished = true;
	//}
}

void Arrow::afterMove()
{
    /*if (transform.position.y > bow->owner->level->LEVEL_HEIGHT)
    {
        nav->goToPreviousPos();
        nav->isEnabled = false;
        mov_finished = true;
    }*/
}

void Arrow::onUpdate()
{
    if (!mov_finished)
        return;
    else
    {
       // bow->owner->level->finishTurn();
       // bow->state = Bow::BOW_STATE_IDLE;
        bow->arrow = nullptr;
        mov_finished = false;
    }
}