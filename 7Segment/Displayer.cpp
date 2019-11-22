#include "Displayer.h"

void GetSymbol(char c, bool segment[8], bool dot = false)
{
	switch (c)
	{
		case '1': { bool src[] = { 0,1,1,0,0,0,0 }; memcpy(segment, src, 7); break; }
		case '2': { bool src[] = { 1,1,0,1,1,0,1 }; memcpy(segment, src, 7); break; }
		case '3': { bool src[] = { 1,1,1,1,0,0,1 }; memcpy(segment, src, 7); break; }
		case '4': { bool src[] = { 0,1,1,0,0,1,1 }; memcpy(segment, src, 7); break; }
		case '5': { bool src[] = { 1,0,1,1,0,1,1 }; memcpy(segment, src, 7); break; }
		case '6': { bool src[] = { 1,0,1,1,1,1,1 }; memcpy(segment, src, 7); break; }
		case '7': { bool src[] = { 1,1,1,0,0,0,0 }; memcpy(segment, src, 7); break; }
		case '8': { bool src[] = { 1,1,1,1,1,1,1 }; memcpy(segment, src, 7); break; }
		case '9': { bool src[] = { 1,1,1,1,0,1,1 }; memcpy(segment, src, 7); break; }
		case '0': { bool src[] = { 1,1,1,1,1,1,0 }; memcpy(segment, src, 7); break; }
		case 'A': { bool src[] = { 1,1,1,0,1,1,1 }; memcpy(segment, src, 7); break; }
		case 'B': { bool src[] = { 0,0,1,1,1,1,0 }; memcpy(segment, src, 7); break; }
		case 'C': { bool src[] = { 1,0,0,1,1,1,0 }; memcpy(segment, src, 7); break; }
		case 'D': { bool src[] = { 0,1,1,1,1,0,1 }; memcpy(segment, src, 7); break; }
		case 'E': { bool src[] = { 1,0,0,1,1,1,1 }; memcpy(segment, src, 7); break; }
		case 'F': { bool src[] = { 1,0,0,0,1,1,1 }; memcpy(segment, src, 7); break; }
		case '-': { bool src[] = { 0,0,0,0,0,0,1 }; memcpy(segment, src, 7); break; }
		case'x':
		default: { bool src[] = { 0,0,0,0,0,0,0 }; memcpy(segment, src, 7); break; }
	}
	if (dot)
		segment[7] = 1;
	else
		segment[7] = 0;
}

// timer for interruption
int timer1Counter;

void DisplayerClass::Initialize(const short segmentPins[8], int displayCnt, const short displayPins[], int refreshRate)
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

	for (int i = 0; i < displayCount; ++i)
		emptyBlank[i] = 'x';

	emptyBlank[displayCount] = '\0';

	Show(emptyBlank);


	// initialize pseudo multithreading using interrupts
	// https://arduinodiy.wordpress.com/2012/02/28/timer-interrupts/
	noInterrupts();			//disable all interrupts
	TCCR1A = 0;
	TCCR1B = 0;


	// Set preload timer to the correct value for our interrupt interval
	// preload timer = clock max value - clock speed     /prescaler/goal refresh rate
	// preload timer = 2^16            - 16MHz(atmega328)/256      /user specified refresh rate * display count
	timer1Counter = ceil(65536         - (F_CPU          /256.     /(refreshRate * displayCount)));

	TCNT1 = timer1Counter;	// preload timer
	TCCR1B |= (1 << CS12);	// 256 prescaler 
	TIMSK1 |= (1 << TOIE1);	// enable timer overflow interrupt
	interrupts();			// enable all interrupts and start refreshing

	initialized = true;
}

void DisplayerClass::Show(const char cstring[] = "")
{
	for (int displayPosition = 0, stringPosition = 0;
		cstring[stringPosition] != '\0' && displayPosition < displayCount;
		++stringPosition)
	{
		if (cstring[stringPosition] == '.')
			if(stringPosition == 0)
				GetSymbol(cstring[stringPosition], display[displayPosition].segment, true);
			else
				continue;
		GetSymbol(cstring[stringPosition], display[displayPosition].segment, cstring[stringPosition + 1] == '.');
		++displayPosition;
	}
}

void DisplayerClass::Show(int number)
{
	// Creating new empty c string
	char* cstringNumber = new char[displayCount + 1];
	memcpy(cstringNumber, emptyBlank, (displayCount + 1) * sizeof(char));

	// one display segment is reserved for '-'
	int negative = 0;

	if (number < 0)
	{
		cstringNumber[0] = '-';
		number *= -1;
		negative = 1;
	}
	for (int i = displayCount - 1 - negative; i >= 0; --i)
	{
		if (number > 0)
		{
			cstringNumber[i + negative] = number % 10 + '0';
			number /= 10;
		}
	}

	Show(cstringNumber);
	delete[] cstringNumber;
}

void DisplayerClass::Show(float number)
{
	// Creating new empty c string
	char* cstringNumber = new char[displayCount + 2];
	memcpy(cstringNumber, emptyBlank, (displayCount + 2) * sizeof(char));
	cstringNumber[displayCount + 1] = '\0';

	// one display segment is reserved for '-'
	int negative = 0;

	if (number < 0)
	{
		cstringNumber[0] = '-';
		number *= -1;
		negative = 1;
	}

	//	counting the length of the integer part of a number
	int numberLenght = 0;
	for (int i = number; i > 0; i /= 10, ++numberLenght);

	// free space for decimal digits
	int decimalPlaces = displayCount - negative - numberLenght;

	for (int i = decimalPlaces; i > 0; --i)
		cstringNumber[displayCount - (decimalPlaces - i)] = (int)(number * (int)pow(10, i)) % 10 + '0';
	if(decimalPlaces > 0)
		cstringNumber[displayCount - decimalPlaces] = '.';
	else
		decimalPlaces = 0;
	
	for (int i = displayCount - 1 - decimalPlaces - negative; i >= 0; --i)
	{
		if ((int)number > 0)
		{
			cstringNumber[i + negative] = (int)number % 10 + '0';
			number /= 10;
		}
	}
	Show(cstringNumber);
	delete[] cstringNumber;
}

void DisplayerClass::Refresh()
{
	digitalWrite(display[refreshableDisplay].pin, HIGH);
	refreshableDisplay = (refreshableDisplay + 1) % displayCount;
	for (int seg = 0; seg < 8; ++seg)
	{
		digitalWrite(segmentPin[seg], display[refreshableDisplay].segment[seg]);
	}
	digitalWrite(display[refreshableDisplay].pin, LOW);
}

DisplayerClass Displayer;

// Interrupt Service Routine - will execute the specified code when the timer overflows
ISR(TIMER1_OVF_vect)
{
	TCNT1 = timer1Counter;
	Displayer.Refresh();
}