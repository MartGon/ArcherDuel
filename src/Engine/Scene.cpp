#include "Scene.h"
#include "Navigator.h"
#include <stdio.h>

// Attributes
Uint16 Scene::lastGameObjectID = 0;

// Constructors

Scene::Scene()
{
}

Scene::Scene(Scene::SceneMode mode)
{
	setSceneMode(mode);
}

Scene::Scene(SDL_Renderer* renderer)
{

}

Scene::~Scene()
{
}

// Virtual Methods

void Scene::loadMedia() 
{
	printf("Base class\n");
}

void Scene::start()
{

}

void Scene::beforeUpdate()
{

}

void Scene::onUpdate()
{

}

void Scene::destroy()
{
	//printf("Destroying scene\n");
	for (auto &gameObject : gameObjectsToInitialize)
		gameObject->destroy();

	for (auto &gameObjectPair : gameObjectMap)
		if (GameObject *gameObject = gameObjectPair.second)
			gameObject->destroy();

	// Reset las id
	lastGameObjectID = 0;

	// Destroy network agent
	destroyNetworkAgent();

	this->~Scene();
}

void Scene::handleEvent(SDL_Event event)
{

}

// Methods

void Scene::addGameObject(GameObject *gameObject)
{
	gameObjectsToInitialize.push_back(gameObject);
}

void Scene::initGameObject(GameObject *gameObject)
{
	gameObject->start();

	gameObjectsToInitialize.erase(gameObjectsToInitialize.begin());
	gameObjectMap.insert_or_assign(gameObject->id, gameObject);
}

void Scene::update()
{
	// Return if paused
	if (isPaused)
		return;

	// Init gameObjects
	// This loops allows the list to be altered during the gameObjects' initialization
	while (!gameObjectsToInitialize.empty())
		initGameObject(gameObjectsToInitialize.front());

	if (isOnline())
	{
		// Try to establish connection
		if (!connectionEstablished)
		{
			if (connectionEstablished = networkAgent->establishConnection())
				handleConnectionEstablished();
			return;
		}
	}

	// Update hook
	onUpdate();

    // Update every object
    for (auto &gameObjectPair : gameObjectMap)
    {
        if (GameObject *gameObject = gameObjectPair.second)
        {
            if (gameObject->isActive)
            {
                // Update the object
                gameObject->update();

                // Online measures
                if (isOnline())
                    // Once connection is established
                {
                    if (mode == ONLINE_SERVER)
                    {
                        Packet *packet = new Packet(*gameObject);
                        networkAgent->sendPacket(packet);
                    }
                    else if (mode == ONLINE_CLIENT)
                    {
                        if (gameObject->updateFromClient)
                        {
							Packet *packet = new Packet(*gameObject);
							networkAgent->sendPacket(packet);
                        }
                    }
                }
            }
        }
    }
}

void Scene::deactivateAllGameObjects()
{
	for (auto &gameObjectPair : gameObjectMap)
		if (GameObject *gameObject = gameObjectPair.second)
			gameObject->isActive = false;
}

void Scene::activateAllGameObjects()
{
	for (auto &gameObjectPair : gameObjectMap)
		if (GameObject *gameObject = gameObjectPair.second)
			gameObject->isActive = true;
}

// Network

void Scene::destroyNetworkAgent()
{
	if (alreadyDestroyed)
		return;

	if (isOnline())
	{
		if (sem)
		{
			SDL_DestroySemaphore(sem);
			sem = nullptr;
		}

		networkAgent->destroy();

		alreadyDestroyed = true;
	}
}

void Scene::setSceneMode(Scene::SceneMode sceneMode)
{
	printf("Setting scene mode\n");

	mode = sceneMode;

	switch (sceneMode)
	{
	case Scene::ONLINE_CLIENT:
		networkAgent = new NetworkClient();
		break;
	case Scene::ONLINE_SERVER:
		networkAgent = new NetworkServer();
		break;
	}

	if (isOnline())
	{
		// Threading
		sem = SDL_CreateSemaphore(1);
		thread = SDL_CreateThread(recvPacketThread, "defu", this);
		SDL_DetachThread(thread);
	}
}

bool Scene::isOnline()
{
	return mode == ONLINE_CLIENT || mode == ONLINE_SERVER;
}

void Scene::disconnect()
{
	onDisconnect();

	disconnected = true;
}

bool Scene::handlePacket(Packet *packet)
{
	// TODO

	if (!packet)
		return false;

	Uint16 id  = packet->id;

	GameObject *gameObject = gameObjectMap.at(id);

	if (mode == ONLINE_SERVER)
	{
		if (gameObject->updateFromClient)
			gameObject->transform.position = packet->position;
	}
	else if (mode == ONLINE_CLIENT)
	{
		if (!gameObject->updateFromClient)
			gameObject->transform.position = packet->position;

		if (Navigator *nav = gameObject->getComponent<Navigator>())
			nav->setDirection(packet->direction);
	}

	return true;
}

int recvPacketThread(void* data)
{
	Scene *scene = (Scene*)data;
	Packet *packet = nullptr;
	bool valid = false;

	while (true)
	{
		if (scene->disconnected)
			break;

		if (!scene->connectionEstablished)
			continue;

		// Receive data
		packet = scene->networkAgent->recvPacket();

		SDL_SemWait(scene->sem);

		if (!packet)
		{
			scene->disconnect();
			SDL_SemPost(scene->sem);
			break;
		}

		valid = scene->handlePacket(packet);

		if (!valid)
		{
			scene->disconnect();
			SDL_SemPost(scene->sem);
			break;
		}

		SDL_SemPost(scene->sem);

		//printf("Main Thread\n", game->player->score);
	}

	return 0;
}