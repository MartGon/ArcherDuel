#include <string>
#include "Vector2.h"
#include "GameObject.h"
#pragma once

class Packet
{
public:

	// This is a hack to be able to have the packet system independent from the game
	// TODO - Needs Serialization

	// Packet Type
	enum PacketType
	{
		PACKET_INIT_MATCH,
		PACKET_PLAYER_1_POSITION,
		PACKET_PLAYER_2_POSITION,
		PACKET_BALL_POSITION,
		PACKET_MISC
	};

	// Constructors
	Packet();
	Packet(PacketType type);
	Packet(GameObject gameObject);
	~Packet();

	// Packet data
	PacketType packetType = PACKET_MISC;
	Uint16 id = -1;
	
	// Vectors
	Vector2<float> position;
	Vector2<float> direction;
};

