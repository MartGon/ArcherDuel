#pragma once

#include "TextLabel.h"

class DialogBox : public GameObject
{
public:
	// Constructor
	DialogBox(std::string str);

	// Attributes
	float line_offset_y = 15.f;
	Uint32 text_speed = 1;
	bool isDisplaying = false;
	bool isDisplayed = true;
	std::string text;

	// Members
	std::vector<TextLabel*> text_lines;

	// Member methods
	void setText(std::string str);
	std::string getText();

	void hideText();
	void displayText(bool instant = false);

	// Utilities
	TextLabel* createTextLabel();

	// Overrided methods
	void onUpdate() override;

private:

	Vector2<int> display_index;
	Uint32 display_ctr = 0;
};