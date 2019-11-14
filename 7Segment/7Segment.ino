#include "Displayer.h"

void setup() 
{
	const short segPins[] = { 2, 3, 4, 5, 6, 7, 8, 9 };
	const short digPins[] = { 10, 11, 12 };
	Displayer.Init(segPins, 3, digPins);
	Displayer.Show("8.8.8.");
}

void loop() 
{
	Displayer.RefreshStart();

	// You'r code here 

	Displayer.RefreshEnd();
}
