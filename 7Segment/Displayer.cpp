#include "Displayer.h"

void GetSymbol(char c, char& code, bool dot, PinType commonPinType)
{
	switch (c)
	{
		case '1': { code = B01100000; break; }
		case '2': { code = B11011010; break; }
		case '3': { code = B11110010; break; }
		case '4': { code = B01100110; break; }
		case '5': { code = B10110110; break; }
		case '6': { code = B10111110; break; }
		case '7': { code = B11100000; break; }
		case '8': { code = B11111110; break; }
		case '9': { code = B11110110; break; }
		case '0': { code = B11111100; break; }
		case 'A': { code = B11101110; break; }
		case 'a': { code = B11111010; break; }
		case 'B': { code = B00111100; break; }
		case 'C': { code = B10011100; break; }
		case 'c': { code = B00011010; break; }
		case 'D': { code = B01111010; break; }
		case 'E': { code = B10011110; break; }
		case 'e': { code = B11011110; break; }
		case 'F': { code = B10001110; break; }
		case 'G': { code = B10111100; break; }
		case 'H': { code = B01101110; break; }
		case 'h': { code = B00101110; break; }
		case 'I': { code = B00100000; break; }
		case 'J': { code = B01111000; break; }
		case 'L': { code = B00011100; break; }
		case 'l': { code = B00011000; break; }
		case 'N': { code = B11101100; break; }
		case 'n': { code = B00101010; break; }
		case 'O': { code = B00111010; break; }
		case 'P': { code = B11001110; break; }
		case 'Q': { code = B11100110; break; }
		case 'R': { code = B10001100; break; }
		case 'r': { code = B00001010; break; }
		case 'S': { code = B10110110; break; }
		case 'T': { code = B00011110; break; }
		case 'U': { code = B01111100; break; }
		case 'u': { code = B00111000; break; }
		case 'Y': { code = B01110110; break; }
		case '=': { code = B00010010; break; }
		case '-': { code = B00000010; break; }
		case '_': { code = B00010000; break; }
		case '"': { code = B01000100; break; }
		case '\'':{ code = B00000100; break; }
		case'x':
		default: { code = B0; break; }
	}
	if (dot)
		code += 1;
	if (commonPinType == PinType::anode)
		code = ~code;	// invert all bits
}

// counter for interruption (timer 1)
int interruptionTimerCounter;

void DisplayerClass::Initialize(PinType commonPinType, uint8_t segmentPins[8], uint8_t displayPins[], int numberOfDigits, uint32_t refreshRate = 60)
{
	// Creating display containers, saving segment and display pins
	this->numberOfDigits = numberOfDigits;
	if (this->numberOfDigits < 1)
	{
		isInitialized = false;
		return;
	}
	digits = new Digit[this->numberOfDigits];
	memcpy(this->segmentPins, segmentPins, 8 * sizeof(decltype(segmentPins)));
	this->commonPinType = commonPinType;

	// initialize segment pins
	for (int segmentIndex = 0; segmentIndex < 8; ++segmentIndex)
	{
		pinMode(segmentPins[segmentIndex], OUTPUT);
		digitalWrite(segmentPins[segmentIndex], LOW + commonPinType);
	}

	// initialize all common pins
	for (int digitIndex = 0; digitIndex < numberOfDigits; ++digitIndex)
	{
		digits[digitIndex].pin = displayPins[digitIndex];
		pinMode(digits[digitIndex].pin, OUTPUT);
		digitalWrite(digits[digitIndex].pin, HIGH - commonPinType);
	}

	// initialize empty number blank
	emptyBlank = new char[numberOfDigits + 1];
	for (int blankIndex = 0; blankIndex < numberOfDigits; ++blankIndex)
	{
		emptyBlank[blankIndex] = 'x';
	}
	emptyBlank[numberOfDigits] = '\0';

	// initialize pseudo multithreading using interrupts
	// https://arduinodiy.wordpress.com/2012/02/28/timer-interrupts/
	noInterrupts();	// pause interrupts to initialize

	TC1_OPERATING_MODES = 0;	// reset random or preset TC1 settings
	TC1_COUNT_RATE = 0;	// reset the initial value

	// set preload timer to the correct value for our interrupt interval
	// preload timer = clock max value - clock speed/prescaler/goal refresh rate
	interruptionTimerCounter = ceil(65536 - (F_CPU / 256. / (refreshRate * numberOfDigits)));
	COUNTING_REGISTER = interruptionTimerCounter;
	TC1_COUNT_RATE |= (1 << PRESCALER_256);
	TC1_OVERFLOW_REACTION |= (1 << CALL_INTERRUPT_VECTOR);	// enable timer overflow interrupt

	interrupts();	// unpause all interrupts and start refreshing

	isInitialized = true;
	Show(emptyBlank);
}

void DisplayerClass::Show(const char cstring[] = "")
{
	if (isInitialized)
	{
		for (int digitIndex = 0, stringIndex = 0; cstring[stringIndex] && digitIndex < numberOfDigits; ++stringIndex)
		{
			if (cstring[stringIndex] == '.')
			{
				if (stringIndex == 0)
				{
					GetSymbol('0', digits[digitIndex].encoding, true, commonPinType);
					++digitIndex;
				}
				continue;
			}
			GetSymbol(cstring[stringIndex], digits[digitIndex].encoding, cstring[stringIndex + 1] == '.', commonPinType);
			++digitIndex;
		}
	}
}

void DisplayerClass::Show(int number)
{
	if (isInitialized)
	{
		// Creating new empty c string
		char* cstringNumber = new char[numberOfDigits + 1];
		memcpy(cstringNumber, emptyBlank, (numberOfDigits + 1) * sizeof(decltype(emptyBlank)));

		// one display segment is reserved for '-'
		bool negative = false;

		if (number < 0)
		{
			cstringNumber[0] = '-';
			number *= -1;
			negative = true;
		}

		// write the integer part of a number
		for (int stringIndex = numberOfDigits - 1 - (int)negative; stringIndex >= 0; --stringIndex)
		{
			if (number > 0)
			{
				cstringNumber[stringIndex + (int)negative] = number % 10 + '0';
				number /= 10;
			}
		}

		Show(cstringNumber);
		delete[] cstringNumber;
	}
}

void DisplayerClass::Show(float number)
{
	if (isInitialized)
	{
		// Creating new empty c string
		char* cstringNumber = new char[numberOfDigits + 2];
		memcpy(cstringNumber, emptyBlank, (numberOfDigits + 2) * sizeof(decltype(emptyBlank)));
		cstringNumber[numberOfDigits + 1] = '\0';

		// one display segment is reserved for '-'
		bool negative = false;

		if (number < 0)
		{
			cstringNumber[0] = '-';
			number *= -1;
			negative = true;
		}

		//	counting the length of the integer part of a number
		int numberLenght = 0;
		for (int i = number; i > 0; i /= 10, ++numberLenght);

		// free space for decimal digits
		int decimalPlaces = numberOfDigits - (int)negative - numberLenght;

		// write the maximum number of digits after the decimal point
		if (decimalPlaces > 0)
		{
			for (int i = decimalPlaces; i > 0; --i)
			{
				int decimalMultiplier = pow(10, i);
				int multipliedNumber = number * decimalMultiplier;
				cstringNumber[numberOfDigits - (decimalPlaces - i)] = multipliedNumber % 10 + '0';
			}
			cstringNumber[numberOfDigits - decimalPlaces] = '.';
		}
		else
		{
			decimalPlaces = 0;
		}

		// write the integer part of a number
		for (int stringIndex = numberOfDigits - 1 - decimalPlaces - (int)negative; stringIndex >= 0; --stringIndex)
		{
			if ((int)number > 0)
			{
				cstringNumber[stringIndex + (int)negative] = (int)number % 10 + '0';
				number /= 10;
			}
		}

		Show(cstringNumber);
		delete[] cstringNumber;
	}
}

void DisplayerClass::Refresh()
{
	if (isInitialized)
	{
		digitalWrite(digits[refreshableDigitIndex].pin, HIGH - commonPinType);
		refreshableDigitIndex = (refreshableDigitIndex + 1) % numberOfDigits;
		for (int segmentIndex = 0; segmentIndex < 8; ++segmentIndex)
		{
			digitalWrite(segmentPins[segmentIndex], digits[refreshableDigitIndex].encoding << segmentIndex & 0b10000000);
		}
		digitalWrite(digits[refreshableDigitIndex].pin, LOW + commonPinType);
	}
}

DisplayerClass Displayer;

// Interrupt Service Routine - will execute the specified code when the timer overflows
INTERRUPT_SERVICE_ROUTINE(TC1_OVERFLOW_VECTOR)
{
	COUNTING_REGISTER = interruptionTimerCounter;
	Displayer.Refresh();
}