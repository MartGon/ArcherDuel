#pragma once
#include "Texture.h"

// Constants
extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;

// Var
class Scene;
extern Scene *gFirstScene;

bool initGameWindow(SDL_Window* &window, SDL_Renderer* &renderer);

void closeGame();

int engine_main();


