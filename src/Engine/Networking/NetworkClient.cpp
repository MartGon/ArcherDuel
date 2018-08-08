#include "NetworkClient.h"

// Constructors

NetworkClient::NetworkClient() : NetworkAgent()
{
	state = CLIENT_READ_CONFIG_FILE;
}

NetworkClient::~NetworkClient()
{
}

bool NetworkClient::readConfigFile()
{
	// Open File
	std::string configFileName = "client-config.txt";
	std::ifstream configFile(configFileName);

	if (!configFile)
	{
		std::cout << "Couldn't open " << configFileName << "\n";
		return false;
	}

	// Line for reading
	std::string line;

	// Read Server IP
	std::getline(configFile, serverIP);
	std::cout << "Read IP " << serverIP << "\n";

	// Read Server Port
	std::getline(configFile, line);
	serverPort = std::stoi(line);
	std::cout << "Server port " << serverPort << "\n";

	return true;
}

bool NetworkClient::openClientSocket()
{
	IPaddress ip;

	if (SDLNet_ResolveHost(&ip, serverIP.c_str(), serverPort) == -1)
	{
		std::cout << "SDLNet_ResolveHost: " << SDLNet_GetError() << "\n";
		return false;
	}

	clientSocket = SDLNet_TCP_Open(&ip);
	if (!clientSocket)
	{
		std::cout << "SDLNet_TCP_Open: " << SDLNet_GetError() << "\n";
		return false;
	}

	std::cout << "TCP Write Port " << serverPort << " Opened Successfully! \n";
	return true;
}

bool NetworkClient::establishConnection()
{
	switch (state)
	{
	case CLIENT_READ_CONFIG_FILE:
		if (readConfigFile())
			state = CLIENT_OPENING_SOCKET;
		break;
	case CLIENT_OPENING_SOCKET:
		if (openClientSocket())
			state = CLIENT_SENDING;
		break;
	case CLIENT_SENDING:
		if (sendPacket(new Packet()))
			state = CLIENT_RECEIVING;
		break;
	case CLIENT_RECEIVING:
		if (recvPacket())
			state = CLIENT_CONNECTION_ESTABLISHED;
		break;
	case CLIENT_CONNECTION_ESTABLISHED:
		return true;
	default:
		break;
	}

	return false;
}

bool NetworkClient::sendPacket(Packet* packet)
{
	return NetworkAgent::sendPacket(clientSocket, packet);
}

Packet* NetworkClient::recvPacket()
{
	return NetworkAgent::recvPacket(clientSocket);
}

void NetworkClient::beforeDestroy()
{
	SDLNet_TCP_Close(clientSocket);
}