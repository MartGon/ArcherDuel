#include "MainGameLoop.h"
#include "RendererManager.h"
#include "SceneManager.h"

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;
Scene *gFirstScene = nullptr;

bool initGameWindow(SDL_Window* &window, SDL_Renderer* &renderer)
{
	if (SDL_Init(SDL_INIT_VIDEO))
	{
		printf("Could not initialize SDL Video, Error: %s \n", SDL_GetError());
		return false;
	}

	int imgFlags = IMG_INIT_PNG; 
	if (!(IMG_Init(imgFlags) & imgFlags)) 
	{ 
		printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError()); 
		return false;
	}

	window = SDL_CreateWindow("SDL-Pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if (!window)
	{
		printf("Window could not be created! SDL Error: %s \n", SDL_GetError());
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer)
	{
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	return true;
}

int engine_main()
{
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;

	if (!initGameWindow(window, renderer))
	{
		printf("Failed to init");
		return 0;
	}
	else
	{
		// Game initialization
		RendererManager::renderer = renderer;

		Scene *scene = gFirstScene;
		gFirstScene->renderer = renderer;
		SceneManager::loadScene(*scene);

		SDL_Event e;
		bool *quit = &SceneManager::quit;

		//While application is running
		while (!(*quit))
		{
			//Handle events on queue
			while (SDL_PollEvent(&e) != 0)
			{
				//User requests quit
				if (e.type == SDL_QUIT)
				{
					*quit = true;
				}
				else
					SceneManager::scene->handleEvent(e);
			}

			// Update scener
			SceneManager::scene->update();

			// Render buffer
			SDL_RenderPresent(renderer);
			SDL_RenderClear(renderer);
		}
	}

	return 0;
}

void closeGame()
{
	// Free all textures

	/*Destroy window	
	SDL_DestroyRenderer(*renderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;
	*/

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}