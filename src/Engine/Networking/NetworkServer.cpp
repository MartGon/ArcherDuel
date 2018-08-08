#include "NetworkServer.h"


// Constructors

NetworkServer::NetworkServer() : NetworkAgent()
{
	state = SERVER_READ_CONFIG_FILE;
}

NetworkServer::~NetworkServer()
{
}

bool NetworkServer::readConfigFile()
{
	// Open File
	std::string configFileName = "server-config.txt";
	std::ifstream configFile(configFileName);

	if (!configFile)
	{
		std::cout << "Couldn't open " << configFileName << "\n";
		return false;
	}

	// Line for reading
	std::string line;

	// Read Server Port
	std::getline(configFile, line);
	serverPort = std::stoi(line);
	std::cout << "Server port " << serverPort << "\n";

	return true;
}

bool NetworkServer::openServerSocket()
{
	IPaddress ip;

	if (SDLNet_ResolveHost(&ip, NULL, serverPort) == -1)
	{
		std::cout << "SDLNet_ResolveHost: " << SDLNet_GetError() << "\n";
		return false;
	}

	serverSocket = SDLNet_TCP_Open(&ip);
	if (!serverSocket)
	{
		std::cout << "SDLNet_TCP_Open: " << SDLNet_GetError() << "\n";
		return false;
	}

	std::cout << "TCP Read Port " << serverPort << " Opened Successfully! \n";
	return true;
}

bool NetworkServer::pairWithClient()
{
	if (!serverSocket)
	{
		std::cout << "Can't send packet, read socket is not opened \n";
		return false;
	}

	if (clientSocket)
	{
		std::cout << "Client already paired \n";
		return false;
	}

	if (clientSocket = SDLNet_TCP_Accept(serverSocket))
	{
		std::cout << " Paired succesfully with client \n";
		return true;
	}

	std::cout << " Could not pair with client \n";
	return false;
}

bool NetworkServer::establishConnection()
{
	switch (state)
	{
	case SERVER_READ_CONFIG_FILE:
		if (readConfigFile())
			state = SERVER_STATE_OPENING_SOCKET;
		break;

	case SERVER_STATE_OPENING_SOCKET:
		if (openServerSocket())
			state = SERVER_STATE_PAIRING;
		break;
	case SERVER_STATE_PAIRING:
		if (pairWithClient())
			state = SERVER_STATE_SENDING;
		break;
	case SERVER_STATE_SENDING:
		if (sendPacket(new Packet()))
			state = SERVER_STATE_RECEIVING;
		break;
	case SERVER_STATE_RECEIVING:
		if (recvPacket())
			state = SERVER_STATE_CONNECTION_ESTABLISHED;
		break;
	case SERVER_STATE_CONNECTION_ESTABLISHED:
		return true;
	default:
		break;
	}

	return false;
}

bool NetworkServer::sendPacket(Packet *packet)
{
	return NetworkAgent::sendPacket(clientSocket, packet);
}

Packet* NetworkServer::recvPacket()
{
	return NetworkAgent::recvPacket(clientSocket);
}

void NetworkServer::beforeDestroy()
{
	SDLNet_TCP_Close(serverSocket);

	if (state == SERVER_STATE_CONNECTION_ESTABLISHED)
	{
		SDLNet_TCP_Close(clientSocket);
	}
}