#include "DialogBox.h"

#include "InputManager.h"

#include <string>
#include <sstream>

// Constructors

DialogBox::DialogBox(std::string str)
{
	setText(str);
}

// Member methods

void DialogBox::setText(std::string str)
{
	// Set value
	this->text = str;

	// Get string divided in lines
	std::vector<std::string> sub_strs;
	std::istringstream iss(str);

	std::string get_str;
	while (std::getline(iss, get_str, '\n'))
	{
		sub_strs.push_back(get_str);
	}

	// Create textlabels accordingly
	int diff = sub_strs.size() - text_lines.size();

	// Create text labels if we need more
	if (diff > 0)
	{
		for (int i = 0; i < diff; i++)
			text_lines.push_back(createTextLabel());
	}

	// Activate text labels we need
	for (int i = 0; i < text_lines.size(); i++)
	{
		TextLabel* label = text_lines[i];

		// Enable text label if needed
		if (i < sub_strs.size())
		{
			label->isActive = true;

			// Set text
			std::string str_to_set = sub_strs.at(i);
			label->setText(str_to_set);
		}
		// Disable other wise
		else
			label->isActive = false;
	}

	// Reorder labels
	TextLabel* last_label = nullptr;
	for(auto text_label : text_lines)
	{
		if (last_label)
		{
			text_label->transform.position.y = last_label->transform.position.y + line_offset_y;
			text_label->transform.position.x = -5;
		}

		last_label = text_label;
	}
	
	return;
}

std::string DialogBox::getText()
{
	return text;
}

void DialogBox::hideText()
{
	for (auto label : text_lines)
	{
		for (auto tRenderer : label->font_tRenderers)
		{
			tRenderer->isEnabled = false;
		}
	}

	isDisplayed = false;
}

void DialogBox::displayText(bool instant)
{
	if (!isDisplayed)
	{
		if (instant)
		{
			for (auto label : text_lines)
			{
				for (auto tRenderer : label->font_tRenderers)
				{
					tRenderer->isEnabled = true;
				}
			}
		}
		else if (!isDisplaying)
			isDisplaying = true;
	}
}

TextLabel* DialogBox::createTextLabel()
{
	TextLabel* text_label = new TextLabel();

	// Set parent
	text_label->transform.parent = &this->transform;

	// Get last if not empty
	if (!text_lines.empty())
	{
		TextLabel* last_label = text_lines.back();

		// Set downwards
		text_label->transform.position.y = last_label->transform.position.y + line_offset_y;
		text_label->transform.position.x = -5;
	}

	return text_label;
}

void DialogBox::onUpdate()
{
	InputManager* iMgr = InputManager::get();

	if (isDisplaying)
	{
		int label_index = display_index.x;
		if (label_index < text_lines.size())
		{
			TextLabel* label = text_lines[label_index];
			if (label->isActive)
			{
				int char_index = display_index.y;
				if (char_index < label->font_tRenderers.size())
				{
					display_ctr = (display_ctr + 1) % text_speed;
					bool display = display_ctr == 0;
					if (display)
					{
						// Enable char
						TextureRenderer* fRenderer = label->font_tRenderers[char_index];
						fRenderer->isEnabled = true;

						// Increase char ptr
						display_index.y++;

						// Reset ctr
						display_ctr = 0;
					}
				}
				else
				{
					// Increase line
					display_index.x++;

					// Reset char ptr
					display_index.y = 0;
				}
			}
			else
			{
				// Stop at inactive label
				isDisplaying = false;

				// Reset index
				display_index = { 0, 0 };

				// Set flag
				isDisplayed = true;
			}
		}
		else
		{
			// Stop if out of index
			isDisplaying = false;

			// Reset index
			display_index = { 0, 0 };

			// Set flag
			isDisplayed = true;
		}
	}
	else
	{
		if (iMgr->isKeyEvent(SDL_SCANCODE_Q, network_owner, KeyEvent::DOWN))
			hideText();
		else if (iMgr->isKeyEvent(SDL_SCANCODE_E, network_owner, KeyEvent::DOWN))
			displayText();
	}
}