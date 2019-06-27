#include "QuestTracker.h"

// Constructor

QuestTracker::QuestTracker(std::string quest_text, QuestTracker::QuestTrackerType type, Uint32 max_amount)
{
	// Create quest_text TextLabel
	this->quest_text = new TextLabel();

	// Create completed_text TextLabel
	this->progress_text = new TextLabel();

	// Set parent and text
	this->quest_text->transform.parent = &this->transform;
	setQuestText(quest_text);

	// Depending on type
	setType(type, max_amount);

	// Set parent and text
	this->progress_text->transform.parent = &this->transform;
	this->progress_text->transform.position.x = this->quest_text->getDimensions().x + 10;
}

// Member methods

void QuestTracker::setQuestText(std::string text)
{
	// Set text
	std::string text_to_set = text + ':';
	quest_text->setText(text_to_set);

	// Update pos
	if (progress_text)
	{
		auto dim = this->quest_text->getDimensions();
		this->progress_text->transform.position.x = dim.x + 10;
	}
}

void QuestTracker::setTrackingText(bool completed)
{
	if (completed)
	{
		// Set completed
		progress_text->setText("Completed");
		progress_text->setTextColor({ 0, 255, 0 });
	}
	else
	{
		if (type == QuestTrackerType::QUEST_TRACKER_UNIQUE_OBJECTIVE)
		{
			this->progress_text->setText("Not Completed\n");
			progress_text->setTextColor({ 255, 0, 0 });
		}
	}
}

void QuestTracker::setTrackingText(std::string completed_text)
{
	this->progress_text->setText(completed_text);
}

void QuestTracker::setType(QuestTracker::QuestTrackerType type, Uint32 max_amount)
{
	// Reset status
	reset();

	// Set type
	this->type = type;

	if (type == QUEST_TRACKER_UNIQUE_OBJECTIVE)
	{
		// Set not completed
		progress_text->setText("Not Completed");
		progress_text->setTextColor({ 255, 0, 0 });

		// Set max_amount to 1
		this->max_amount = 1;
	}
	else if (type == QUEST_TRACKER_MULTI_OBJECTIVE)
	{
		this->max_amount = max_amount;

		// Set progress numbers
		setProgressText(progress_amount);
		progress_text->setTextColor({ 255, 255, 255 });
	}
}

void QuestTracker::setProgressText(Uint32 amount)
{
	std::string track_text = std::to_string(progress_amount) + "/" + std::to_string(max_amount);
	setTrackingText(track_text);
}

void QuestTracker::updateProgress(Uint32 amount)
{
	// Do nothing if already complete
	if (isComplete)
		return;

	// Increase amount
	progress_amount = progress_amount + amount;

	// Check
	if (progress_amount >= max_amount)
	{
		// Set flag
		isComplete = true;
		
		// Set to max
		progress_amount = max_amount;

		// Call callback method
		if(onComplete)
			onComplete();

		setTrackingText(true);
	}

	// Set progress text
	if (type == QUEST_TRACKER_MULTI_OBJECTIVE)
	{
		setProgressText(progress_amount);
	}
}

void QuestTracker::reset()
{
	isComplete = false;
	progress_amount = 0;
}