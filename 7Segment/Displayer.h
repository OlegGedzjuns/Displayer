#ifndef _DISPLAYER_h
#define _DISPLAYER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

enum Common_ { Common_cathode = 0, Common_anode = 1 };

class DisplayerClass
{
private:
	bool initialized = false;

	struct Display
	{
		// state of each segment for each display from a to DP
		char segmentCode;
		// pin to which the display is connected
		short pin;
	}* display;
	int displayCount;
	short segmentPin[8];

	// current display for async refreshing
	int refreshableDisplay = 0;

	// blank for initializing new c strings
	char* emptyBlank;

	Common_ common;
public:
	void Initialize(Common_ commonPin, const short segmentPins[8], int displayCnt, const short displayPins[], int refreshRate);
	void Show(const char cstring[] = "");
	void Show(int number);
	void Show(float number);
	void Refresh();
};

extern DisplayerClass Displayer;

#endif
