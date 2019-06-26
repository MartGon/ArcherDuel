#include "Scene.h"

#include "Player.h"

#include "Button.h"
#include "DialogBox.h"

// Forward declaration
class Tower;

class TutorialLevel : public Scene
{
public:
	// Const
	static const int LEVEL_WIDTH;
	static const int LEVEL_HEIGHT;

	// Phases
	enum TutorialLevelPhase
	{
		TUTORIAL_PHASE_INTRO,
		TUTORIAL_PHASE_STRAFE,
		TUOTIRAL_PHASE_JUMP
	};

	// Constructor
	TutorialLevel();

	// Overrided methods
			// Scene
	void loadMedia() override;
	void onUpdate() override;

	// GameObjects
	Player* player = nullptr;
	Tower* tower = nullptr;

	// GUI
	Button* continue_button = nullptr;
	DialogBox* dialog_box = nullptr;

	// GUI methods
	void exitButtonHandler();
	void continueButtonHandler();
};