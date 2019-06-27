#include "GameObject.h"

#include "TextLabel.h"

#include <functional>

class QuestTracker : public GameObject
{
public:

	enum QuestTrackerType
	{
		QUEST_TRACKER_UNIQUE_OBJECTIVE,
		QUEST_TRACKER_MULTI_OBJECTIVE
	};

	QuestTrackerType type = QUEST_TRACKER_UNIQUE_OBJECTIVE;

	// Constructor
	QuestTracker(std::string quest_text, QuestTrackerType type, Uint32 max_amount = 1);

	// Attributes
	bool isComplete = false;
	Uint32 progress_amount = 0;
	Uint32 max_amount = 5;

	// Members
	TextLabel* quest_text = nullptr;
	TextLabel* progress_text = nullptr;

	// Member methods
	void setQuestText(std::string quest_text);
	void setTrackingText(bool completed);
	void setTrackingText(std::string completed_text);

	void setType(QuestTrackerType type, Uint32 max_amount = 1);

	void setProgressText(Uint32 amount);
	void updateProgress(Uint32 amount);

	void reset();

	// Callback methods

	std::function<void()> onComplete;
};