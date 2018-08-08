#pragma once
#include <vector>

class Utilities
{
public:
	Utilities();
	~Utilities();

	static int getNumberOfDigits(int number);
	static std::vector<int> getDigitsFromNumber(int number);
};

