#include "Scene.h"

#include "Player.h"

#include "Button.h"
#include "DialogBox.h"
#include "QuestTracker.h"

#include <unordered_map>

// Forward declaration
class Tower;
class TutorialLevel;

// Bullseye

class Bullseye : public GameObject
{
public:
	// Constructor
	Bullseye();

	// Refs
	TutorialLevel* tLevel = nullptr;

	// Components
	TextureRenderer* tRenderer = nullptr;
	BoxCollider* bCollider = nullptr;

	// Override methods
	void onColliderEnter(Collider* col);
};

class TutorialLevel : public Scene
{
public:
	// Const
	static const int LEVEL_WIDTH;
	static const int LEVEL_HEIGHT;

	// Phases
	enum TutorialLevelPhase
	{
		TUTORIAL_PHASE_NULL,
		TUTORIAL_PHASE_INTRO,
		TUTORIAL_PHASE_STRAFE,
		TUTORIAL_PHASE_JUMP,
		TUTORIAL_PHASE_FASTFALL,
		TUTORIAL_PHASE_BOW,
		TUTORIAL_PHASE_POWERUP,
		TUTORIAL_PHASE_CANNON,
		TUTORIAL_PHASE_FINALE
	};

	// Phase
	TutorialLevelPhase phase = TUTORIAL_PHASE_NULL;

	// Phase->Dialog map
	using DialogList = std::vector<std::string>;
	std::unordered_map<TutorialLevelPhase, DialogList> phase_dialog_map;

	// Dialog index
	Uint32 dialog_index = 0;

	// Constructor
	TutorialLevel();

	// Overrided methods
			// Scene
	void loadMedia() override;
	void onUpdate() override;

	// GameObjects
	Player* player = nullptr;
	Tower* tower = nullptr;
	Tower* tower2 = nullptr;
	std::vector<Bullseye*> targets;
	std::vector<PowerUpObject*> power_up_objects;

	// GUI
	Button* continue_button = nullptr;
	DialogBox* dialog_box = nullptr;
	QuestTracker* quest_tracker = nullptr;

	// GUI methods
	void exitButtonHandler();
	void continueButtonHandler();
	void onDisplayDialog();

	// Dialog
	std::string getNextText(TutorialLevelPhase phase, Uint32 dialog_index);

	// Tutorial
	void onChangeTutorialPhase(TutorialLevelPhase phase);

	// Player callbacks
	void onPlayerStrafe();
	void onPlayerJump();
	void onPlayerFastFall();

	// Other callbacks
	void onPracticeTargetHit();
	void onPowerUpHit(PowerUp* pu);

	// Quest callback
	void onCompleteQuest();
};