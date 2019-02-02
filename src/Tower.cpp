#include "Tower.h"

Tower::Tower(RoofColor roofColor) : GameObject()
{
	// Texture Renderer
	MapRGB *colorKey = new MapRGB();
	colorKey->red = 48;
	colorKey->green = 96;
	colorKey->blue = 130;
	
	if(roofColor)
		tRenderer = setComponent(new TextureRenderer("SymmetricTowerWRoof2Bluew.png", colorKey, 2));
	else
		tRenderer = setComponent(new TextureRenderer("SymmetricTowerWRoof2.png", colorKey, 2));

	// Colliders
	BoxCollider* collider = nullptr;
	
	// Floor Collider 
	collider = setComponent(new BoxCollider(112 - 16 + 1, 9));
	collider->offset = Vector2<float>(16, 68);
	//collider->debug = true;

	// Upper block Collider
	collider = setComponent(new BoxCollider(95 - 33 + 1, 99 - 77));
	collider->offset = Vector2<float>(33, 77);
	//collider->debug = true;

	// Second floor collider
	collider = setComponent(new BoxCollider(98 - 30 + 1, 106 - 98));
	collider->offset = Vector2<float>(30, 99);
	//collider->debug = true;

	// Middle Block collider
	collider = setComponent(new BoxCollider(95 - 33 + 1, 135 - 106));
	collider->offset = Vector2<float>(33, 107);
	//collider->debug = true;

	// Down block collider
	collider = setComponent(new BoxCollider(103 - 25 + 1, 159 - 135));
	collider->offset = Vector2<float>(25, 136);
	//collider->debug = true;
}