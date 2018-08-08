#include "NetworkAgent.h"
#pragma once

class NetworkServer : public NetworkAgent
{
public:

	enum ServerState
	{
		SERVER_STATE_NULL,
		SERVER_READ_CONFIG_FILE,
		SERVER_STATE_OPENING_SOCKET,
		SERVER_STATE_PAIRING,
		SERVER_STATE_SENDING,
		SERVER_STATE_RECEIVING,
		SERVER_STATE_CONNECTION_ESTABLISHED
	};

	// Constructors
	NetworkServer();
	~NetworkServer();

	// Attributes

		// State
	ServerState state;

		// Server side
	TCPsocket serverSocket = nullptr;
	Uint16 serverPort;

		// Client side
	TCPsocket clientSocket = nullptr;

	// Methods
		// Uppper
	virtual bool readConfigFile();
	virtual bool establishConnection();

		// Own
	bool openServerSocket();
	bool pairWithClient();

		// Communication
	virtual bool sendPacket(Packet* packet);
	virtual Packet* recvPacket();

	// Other
	virtual void beforeDestroy();
};

