#include "NetworkAgent.h"

// Attributes


// Constructors

NetworkAgent::NetworkAgent()
{
		std::cout << "Inicializando networking\n";
		initNetworking();
}

NetworkAgent::~NetworkAgent()
{
}

// Methods

void NetworkAgent::initNetworking()
{
	if (SDLNet_Init() == -1)
	{
		std::cout << "SDLNet_Init: " << SDLNet_GetError() << "\n";
		return;
	}

}

bool NetworkAgent::readConfigFile()
{
	std::cout << "Parent read mehtod\n";
	return false;
}

bool NetworkAgent::establishConnection()
{
	return false;
}

bool NetworkAgent::sendPacket(TCPsocket socket, Packet* packet)
{
	if (!socket)
	{
		std::cout << "Can't send packet, socket is not opened \n";
		return false;
	}

	int result;
	int len = sizeof(Packet);
	//printf("Longitud enviada %i\n", len);
	result = SDLNet_TCP_Send(socket, packet, len);

	if (result < len)
	{
		std::cout << "SDLNet_TCP_Send: " << SDLNet_GetError() << "\n";
		return false;
	}

	//std::cout << "Pair packet sent succesfully \n";
	return true;
}

Packet* NetworkAgent::recvPacket(TCPsocket socket)
{
	if (!socket)
	{
		std::cout << "Can't receive packet, socket is not opened \n";
		return nullptr;
	}

	Packet *packet = new Packet();
	int result;
	int len = sizeof(Packet);
	result = SDLNet_TCP_Recv(socket, packet, len);

	//printf("Longitud recibida %i\n", len);

	if (result <= 0)
	{
		std::cout << "SDLNet_TCP_Recv: " << SDLNet_GetError() << "\n";
		return nullptr;
	}

	//std::cout << "Pair packet: " << len << " recv succesfully \n";
	return packet;
}

bool NetworkAgent::sendPacket(Packet * packet)
{
	return false;
}

Packet* NetworkAgent::recvPacket()
{
	return nullptr;
}

void NetworkAgent::destroy()
{
	// Virtual hook
	beforeDestroy();

	// Reset flag
	printf("Destroying network agent\n");

	// Calling SDL
	SDLNet_Quit();

	// Destroying pointer
	this->~NetworkAgent();
}

void NetworkAgent::beforeDestroy()
{
	printf("Parent method\n");
}