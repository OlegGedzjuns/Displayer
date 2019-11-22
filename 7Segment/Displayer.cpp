#include "Displayer.h"
#include "DisplayEncoding.h"

void DisplayerClass::Initialize(const short segmentPins[8], int displayCnt, const short displayPins[])
{
	// Creating display containers, saving segment and display pins
	// number of displays is not limited
	displayCount = displayCnt;
	display = new Display[displayCount];
	memcpy(segmentPin, segmentPins, 8 * sizeof(short));

	// disable all pins until the first refresh
	for (int segment = 0; segment < 8; ++segment)
	{
		pinMode(segmentPin[segment], OUTPUT);
		digitalWrite(segmentPin[segment], LOW);
	}
	// disable all cathodes until the first refresh and copy display pins
	for (int disp = 0; disp < displayCount; ++disp)
	{
		display[disp].pin = displayPins[disp];
		pinMode(display[disp].pin, OUTPUT);
		digitalWrite(display[disp].pin, HIGH);
	}

	// initialize all blanks
	emptyBlank = new char[displayCount + 1];
	negativeIntBlank = new char[displayCount + 1];
	negativeFloatBlank = new char[displayCount + 2];

	for (int i = 0; i < displayCount; ++i)
	{
		emptyBlank[i] = 'x';
		negativeIntBlank[i] = '0';
		negativeFloatBlank[i] = '0';
	}

	emptyBlank[displayCount] = '\0';
	negativeIntBlank[displayCount] = '\0';
	negativeFloatBlank[displayCount] = '.';
	negativeFloatBlank[displayCount + 1] = '\0';

	Show(emptyBlank);

	initialized = true;
}

void DisplayerClass::Show(const char cstring[] = "")
{
	for (int displayPosition = 0, stringPosition = 0;
		cstring[stringPosition] != '\0' && displayPosition < displayCount;
		++stringPosition)
	{
		if (cstring[stringPosition] == '.')
			continue;
		GetSymbol(cstring[stringPosition], display[displayPosition].segment, cstring[stringPosition + 1] == '.');
		++displayPosition;
	}
}

void DisplayerClass::Show(int number)
{
	if (number < 0)
	{
		Show(negativeIntBlank);
		return;
	}

	// Creating new empty c string
	char* cstringNumber = new char[displayCount + 1];
	memcpy(cstringNumber, emptyBlank, (displayCount + 1) * sizeof(char));

	for (int i = displayCount - 1; i >= 0; --i)
	{
		if (number > 0)
		{
			cstringNumber[i] = number % 10 + '0';
			number /= 10;
		}
	}

	Show(cstringNumber);
	delete[] cstringNumber;
}

void DisplayerClass::Show(float number)
{
	if (number < 0)
	{
		Show(negativeFloatBlank);
		return;
	}

	// Creating new empty c string
	char* cstringNumber = new char[displayCount + 2];
	memcpy(cstringNumber, emptyBlank, (displayCount + 2) * sizeof(char));
	cstringNumber[displayCount + 1] = '\0';

	//	counting the length of the integer part of a number
	int numberLenght = 0;
	for (int i = number; i > 0; i /= 10, ++numberLenght);

	int decimalPlaces = displayCount - numberLenght;
	// TODO: create a translation of a number into a string
	// while preserving the maximum number of digits after the decimal point
	//for (int i = decimalPlaces; i > 0; --i){}
	if (decimalPlaces > 0)
	{
		cstringNumber[displayCount] = (int)(number * 10) % 10 + '0';
		cstringNumber[displayCount - 1] = '.';
	}
	else
	{
		Show((int)number);
		return;
	}
	for (int i = 1; i >= 0; --i)
	{
		if (number > 0)
		{
			cstringNumber[i] = (int)number % 10 + '0';
			number /= 10;
		}
	}
	Show(cstringNumber);
	delete[] cstringNumber;
}

void DisplayerClass::Refresh()
{
	digitalWrite(display[refreshableDisp].pin, HIGH);
	refreshableDisp = (refreshableDisp + 1) % displayCount;
	for (int seg = 0; seg < 8; ++seg)
	{
		digitalWrite(segmentPin[seg], display[refreshableDisp].segment[seg]);
	}
	digitalWrite(display[refreshableDisp].pin, LOW);
}

DisplayerClass Displayer;