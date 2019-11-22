#ifndef _DISPLAYER_h
#define _DISPLAYER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

struct Display
{
	// state of each segment for each display from a to DP
	bool segment[8];
	// pin to which the display is connected
	short pin;
};

class DisplayerClass
{
private:
	Display* display;
	int displayCount;
	short segmentPin[8];

	// current display for async refreshing
	int refreshableDisp = 0;
	// delay on each display needed to achieve refresh rate
	int delay;
	// time when refresh started
	unsigned long startTime;
	// true if the next display should light up
	bool refresh = true;

	bool initialized = false;

	// blanks to display negative values
	char* emptyBlank;
	char* negativeIntBlank;
	char* negativeFloatBlank;
public:
	void Initialize(const short segmentPins[8], int displayCnt, const short displayPins[], int refreshRate);
	void Show(const char cstring[] = "");
	void Show(int number);
	void Show(float number);
	void Refresh();
};

extern DisplayerClass Displayer;

#endif
