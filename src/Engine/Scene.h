#pragma once
#include "GameObject.h"
#include "NetworkServer.h"
#include "NetworkClient.h"
#include <SDL_thread.h>
#include <SDL.h>
#include <vector>
#include <map>

int recvPacketThread(void *data);

class Scene
{
public:

	// Scene modes
	enum SceneMode 
	{
		SINGLE_PLAYER,
		LOCAL_MULTIPLAYER,
		ONLINE_CLIENT,
		ONLINE_SERVER
	};

	// Constructors
	Scene();
	Scene(SDL_Renderer* renderer);
	Scene(SceneMode mode);
	~Scene();

	// Network stuff
	NetworkAgent *networkAgent = nullptr;
	bool connectionEstablished = false;
	bool disconnected = false;
	bool alreadyDestroyed = false;

	// Thread
	SDL_Thread *thread = nullptr;
	SDL_sem *sem = nullptr;

	// Static last  gameObject id
	static Uint16 lastGameObjectID;

	// Scene state
	bool isPaused = false;

	// Renderer
	SDL_Renderer *renderer = nullptr;

	// GameObject map
	std::map<Uint16, GameObject*> gameObjectMap;

	// GameObjects to initialize
	std::vector<GameObject*> gameObjectsToInitialize;

	// Network Methods
	void setSceneMode(SceneMode sceneMode);
	void destroyNetworkAgent();
	void disconnect();
	virtual void onDisconnect() {};
	virtual void handleConnectionEstablished() {};
	bool isOnline();
	bool handlePacket(Packet *packet);

	// Overrided Methods
	virtual void loadMedia();
	virtual void start();
	virtual void beforeUpdate();
	virtual void onUpdate();
	virtual void destroy();
	virtual void handleEvent(SDL_Event event);

	void addGameObject(GameObject *gameObject);
	void initGameObject(GameObject *gameObject);
	void update();
	void deactivateAllGameObjects();
	void activateAllGameObjects();

protected:

	// Scene Mode
	SceneMode mode = SINGLE_PLAYER;
};

