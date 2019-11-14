#ifndef _DISPLAYER_h
#define _DISPLAYER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

struct Display
{
	bool segment[8];
	short pin;
};

class DisplayerClass
{
private:
	Display* display;
	int dispCnt;
	short* segPin;

	int refreshableDisp = 0;
	unsigned long startTime;
	bool refresh = true;
public:
	void Init(const short segPin_[8], const short dispCnt_, const short dispPin_[]);
	void Show(char str[]);
	void Show(int num);
	void Show(float num);
	void RefreshStart();
	void RefreshEnd();
};

extern DisplayerClass Displayer;

#endif
