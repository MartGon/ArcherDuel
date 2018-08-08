#include "Utilities.h"


Utilities::Utilities()
{
}

Utilities::~Utilities()
{
}

int Utilities::getNumberOfDigits(int number)
{
	if (!number)
		return 1;

	int n = 0;
	float fNumber = number;

	while (fNumber >= 1)
	{
		fNumber /= 10;
		n++;
	}

	return n;
}

// Lo devuelve en el otro sentido
// Para el número 123
// Lo devuelve [0] = 3, [1] = 2, [2] = 3

std::vector<int> Utilities::getDigitsFromNumber(int number)
{
	int digitsNumber = getNumberOfDigits(number);
	std::vector<int> retVector;

	for (int i = 0; i < digitsNumber; i++)
	{
		int digit = number % 10;
		retVector.push_back(digit);
		number /= 10;
	}

	return retVector;
}