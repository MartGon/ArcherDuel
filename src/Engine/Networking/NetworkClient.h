#include "NetworkAgent.h"
#pragma once

class NetworkClient : public NetworkAgent
{
public:

	enum ClientState
	{
		CLIENT_NULL,
		CLIENT_READ_CONFIG_FILE,
		CLIENT_OPENING_SOCKET,
		CLIENT_SENDING,
		CLIENT_RECEIVING,
		CLIENT_CONNECTION_ESTABLISHED
	};

	// Constructors

	NetworkClient();
	~NetworkClient();

	// Attributes

		// State
	ClientState state;

		// Client side
	TCPsocket clientSocket = nullptr;

		// Server side
	std::string serverIP;
	Uint16 serverPort;

	// Methods
		// Upper
	virtual bool readConfigFile();
	virtual bool establishConnection();

			// Communication
	virtual bool sendPacket(Packet* packet);
	virtual Packet* recvPacket();

		// Own
	bool openClientSocket();
		
	// Other
	virtual void beforeDestroy();
};

