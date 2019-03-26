#include "Arrow.h"
#include "Player.h"
#include "LevelOne.h"
#include "Tower.h"

Arrow::Arrow()
{
	// Setting components
	name = "arrow";

	// Texture Renderer
	MapRGB *colorKey = new MapRGB();
	colorKey->blue = 0xFF;
	tRenderer = setComponent(new TextureRenderer("ArrowTrim.png", colorKey, 254));

	// Set scale
	transform.scale = Vector2<float>(0.5f, 0.5f);

	// Navigator
	nav = setComponent(new Navigator());
	nav->isEnabled = false;
	nav->isKinematic = true;
	nav->acceleration.y = -0.03f;

	// Rotatable Box Collider
	rotCollider = setComponent(new RotatableBoxCollider(Vector2<int>(0, 0), Vector2<int>(0, 3 * transform.scale.y), 
		Vector2<int>(14 * transform.scale.x, 0), Vector2<int>(14 * transform.scale.x, 3 * transform.scale.y)));
	rotCollider->isEnabled = false;
    //rotCollider->debug = true;

	// Damage text label
	dmg_label = new TextLabel();
	dmg_label->transform.parent = &transform;
	dmg_label->transform.position = Vector2<float>(0, -3);
	dmg_label->setText("0");
	dmg_label->setScale(Vector2<float>(0.5, 0.5));

	// Get Texture Renderer
	dmg_tRenderer = dmg_label->getComponent<TextureRenderer>();

	// Disable until the arrow is pinned
	dmg_label->isActive = false;
}

// Hooks

void Arrow::onColliderEnter(Collider* collider)
{
	printf("Arrow Collision\n");

	nav->isEnabled = false;
	rotCollider->isEnabled = false;

	if (Player* player = dynamic_cast<Player*>(collider->gameObject))
	{
		//transform.position = getAbsolutePosition() - player->getAbsolutePosition();
		//transform.parent = &player->transform;
		
		// Stun player hit
		player->stun(120);

		// Knock player back
		player->knockback(nav->getDirection(), nav->speed * 0.65);

		// Disable arrow
		isActive = false;
	}
	if (Tower* tower = dynamic_cast<Tower*>(collider->gameObject))
	{
		// Deal dmg to the tower
		tower->takeDamage(nav->speed);

		// Set dmg label
		dmg_label->setText(std::to_string((int)nav->speed));

		// Enable dmg label
		dmg_label->isActive = true;

		// Set flag
		isPinned = true;

		std::cout << "This arrow did " << nav->speed << "dmg \n";
	}
}

void Arrow::afterMove()
{
	transform.zRotation = nav->getDirection().getAngle();
}

void Arrow::onUpdate()
{

	if (dmg_label->isActive)
	{
		if (isPinned)
		{
			Uint8 alpha = dmg_tRenderer->texture.getAlpha();
			if (!alpha)
			{
				dmg_label->isActive = false;
			}
			else
				dmg_tRenderer->texture.setAlpha(alpha - 1);
		}
	}
}

// TODO - Los colliders de la flecha no aparecen ir acordes al movimiento. HACK: Poner el navigator antes del collider