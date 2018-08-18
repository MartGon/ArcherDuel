#pragma once
#include "Scene.h"
#include "Bow.h"

class MainMenu : public Scene
{
public:
	void loadMedia() override;

	void onClickBow();

	void handleEvent(const SDL_Event& event) override;

	Bow* bow = nullptr;
	GameObject* arrow = nullptr;
};