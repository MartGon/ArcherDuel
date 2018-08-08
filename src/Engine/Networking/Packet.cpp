#include "Packet.h"
#include "Navigator.h"

// Constructors

Packet::Packet()
{
}

Packet::Packet(Packet::PacketType type)
{
	packetType = type;
	position = Vector2<float>(0, 0);
	direction = Vector2<float>(0, 0);
}

Packet::Packet(GameObject gameObject)
{
	packetType = PACKET_MISC;
	id = gameObject.id;
	position = gameObject.transform.position;

	if (Navigator *nav = gameObject.getComponent<Navigator>())
		direction = nav->getDirection();
}

Packet::~Packet()
{
}
