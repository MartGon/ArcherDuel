#include <SDL_net.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Packet.h"
#pragma once
class NetworkAgent
{
public:
	// Constructors
	NetworkAgent();
	~NetworkAgent();

	// Methods
		// Initializing
	static void initNetworking();

		// Configuration
	bool readingError = false;
	virtual bool readConfigFile();
	virtual bool establishConnection();

	// Communication
	virtual bool sendPacket(Packet* packet);
	virtual Packet* recvPacket();

	// Other
	void destroy();
	virtual void beforeDestroy();

protected:
		// Communication
	bool sendPacket(TCPsocket socket, Packet* packet);
	Packet* recvPacket(TCPsocket socket);
};

