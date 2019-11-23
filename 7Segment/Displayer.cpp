#include "Displayer.h"

void GetSymbol(char c, char &code, bool dot = false)
{
	// Thanks to https://github.com/Ri0ee

	switch (c)
	{
		case '1': { code = 0b01100000; break; }
		case '2': { code = 0b11011010; break; }
		case '3': { code = 0b11110010; break; }
		case '4': { code = 0b01100110; break; }
		case '5': { code = 0b10110110; break; }
		case '6': { code = 0b10111110; break; }
		case '7': { code = 0b11100000; break; }
		case '8': { code = 0b11111110; break; }
		case '9': { code = 0b11110110; break; }
		case '0': { code = 0b11111100; break; }
		case 'A': { code = 0b11101110; break; }
		case 'a': { code = 0b11111010; break; }
		case 'B': { code = 0b00111100; break; }
		case 'C': { code = 0b10011100; break; }
		case 'c': { code = 0b00011010; break; }
		case 'D': { code = 0b01111010; break; }
		case 'E': { code = 0b10011110; break; }
		case 'e': { code = 0b11011110; break; }
		case 'F': { code = 0b10001110; break; }
		case 'G': { code = 0b10111100; break; }
		case 'H': { code = 0b01101110; break; }
		case 'h': { code = 0b00101110; break; }
		case 'I': { code = 0b00100000; break; }
		case 'J': { code = 0b01111000; break; }
		case 'L': { code = 0b00011100; break; }
		case 'l': { code = 0b00011000; break; }
		case 'N': { code = 0b11101100; break; }
		case 'n': { code = 0b00101010; break; }
		case 'O': { code = 0b00111010; break; }
		case 'P': { code = 0b11001110; break; }
		case 'Q': { code = 0b11100110; break; }
		case 'R': { code = 0b10001100; break; }
		case 'r': { code = 0b00001010; break; }
		case 'S': { code = 0b10110110; break; }
		case 'T': { code = 0b00011110; break; }
		case 'U': { code = 0b01111100; break; }
		case 'u': { code = 0b00111000; break; }
		case 'Y': { code = 0b01110110; break; }
		case '=': { code = 0b00010010; break; }
		case '-': { code = 0b00000010; break; }
		case '_': { code = 0b00010000; break; }
		case '"': { code = 0b01000100; break; }
		case '\'':{ code = 0b00000100; break; }
		case'x':
		default: { code = 0b00000000; break; }
	}
	if (dot)
		code += 1;
}

// timer for interruption
int timer1Counter;

void DisplayerClass::Initialize(Common_ commonPin, const short segmentPins[8], int displayCnt, const short displayPins[], int refreshRate)
{
	// Creating display containers, saving segment and display pins
	// number of displays is not limited
	displayCount = displayCnt;
	if (displayCount < 1)
	{
		initialized = false;
		return;
	}
	display = new Display[displayCount];
	memcpy(segmentPin, segmentPins, 8 * sizeof(short));

	common = commonPin;

	// disable all pins until the first refresh
	for (int segment = 0; segment < 8; ++segment)
	{
		pinMode(segmentPin[segment], OUTPUT);
		digitalWrite(segmentPin[segment], LOW + common);
	}
	// disable all cathodes until the first refresh and copy display pins
	for (int disp = 0; disp < displayCount; ++disp)
	{
		display[disp].pin = displayPins[disp];
		pinMode(display[disp].pin, OUTPUT);
		digitalWrite(display[disp].pin, HIGH - common);
	}

	// initialize all blanks
	emptyBlank = new char[displayCount + 1];

	for (int i = 0; i < displayCount; ++i)
		emptyBlank[i] = 'x';

	emptyBlank[displayCount] = '\0';


	// initialize pseudo multithreading using interrupts
	// https://arduinodiy.wordpress.com/2012/02/28/timer-interrupts/
	noInterrupts();			//disable all interrupts
	TCCR1A = 0;
	TCCR1B = 0;


	// Set preload timer to the correct value for our interrupt interval
	// preload timer = clock max value - clock speed     /prescaler/goal refresh rate
	// preload timer = 2^16            - 16MHz(atmega328)/256      /user specified refresh rate * display count
	timer1Counter    = ceil(65536      - (F_CPU          /256.     /(refreshRate * displayCount)));

	TCNT1 = timer1Counter;	// preload timer
	TCCR1B |= (1 << CS12);	// 256 prescaler 
	TIMSK1 |= (1 << TOIE1);	// enable timer overflow interrupt
	interrupts();			// enable all interrupts and start refreshing

	initialized = true;
	Show(emptyBlank);
}

void DisplayerClass::Show(const char cstring[] = "")
{
	if (initialized)
	{
		for (int displayPosition = 0, stringPosition = 0;
			cstring[stringPosition] != '\0' && displayPosition < displayCount;
			++stringPosition)
		{
			if (cstring[stringPosition] == '.')
				if (stringPosition == 0)
				{
					GetSymbol('0', display[displayPosition].segmentCode, true);
					++displayPosition;
					continue;
				}
				else
					continue;
			GetSymbol(cstring[stringPosition], display[displayPosition].segmentCode, cstring[stringPosition + 1] == '.');
			++displayPosition;
		}
	}
}

void DisplayerClass::Show(int number)
{
	if (initialized)
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
}

void DisplayerClass::Show(float number)
{
	if (initialized)
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
		if (decimalPlaces > 0)
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
}

void DisplayerClass::Refresh()
{
	if (initialized)
	{
		digitalWrite(display[refreshableDisplay].pin, HIGH - common);
		refreshableDisplay = (refreshableDisplay + 1) % displayCount;
		for (int seg = 0; seg < 8; ++seg)
		{
			digitalWrite(segmentPin[seg], ((display[refreshableDisplay].segmentCode << seg & 0b10000000) / 0b10000000) + common);
		}
		digitalWrite(display[refreshableDisplay].pin, LOW + common);
	}
}

DisplayerClass Displayer;

// Interrupt Service Routine - will execute the specified code when the timer overflows
ISR(TIMER1_OVF_vect)
{
	TCNT1 = timer1Counter;
	Displayer.Refresh();
}