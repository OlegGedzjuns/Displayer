#include "Displayer.h"

void setup()
{
	const short segPins[] = { 2, 3, 4, 5, 6, 7, 8, 9 };
	const short digPins[] = { 10, 11, 12 };
	Displayer.Init(segPins, 3, digPins);
}

void loop() 
{
	Displayer.RefreshStart();

	for (int i = 0; millis() % 1000 == 0; ++i)
	{
		Displayer.Show(i);
	}

	Displayer.RefreshEnd();
}
