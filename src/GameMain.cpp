#include "MainGameLoop.h"
#include "MainMenu.h"

int main(int argc, char* args[])
{
	gFirstScene = new MainMenu();

	engine_main();

	return 0;
}