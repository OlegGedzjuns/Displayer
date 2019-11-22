#include <TimedAction.h>
#include "Displayer.h"

int timedCounter = 0;

void incrementNumber()
{
	Displayer.Show(timedCounter);
	++timedCounter;
}

TimedAction NumberThread = TimedAction(500, incrementNumber);

void setup()
{
	const short segmentPins[] = { 2, 3, 4, 5, 6, 7, 8, 9 };
	const short digitPins[] = { 10, 11, 12 };
	Displayer.Initialize(segmentPins, sizeof digitPins / sizeof(short), digitPins, 60);
}

void loop() 
{
	NumberThread.check();

	Displayer.Refresh();
}
