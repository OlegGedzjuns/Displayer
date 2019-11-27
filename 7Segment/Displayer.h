#ifndef _DISPLAYER_h
#define _DISPLAYER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#define INTERRUPT_SERVICE_ROUTINE ISR

#define TC1_OPERATING_MODES TCCR1A
#define TC1_COUNT_RATE TCCR1B
#define TC1_OVERFLOW_REACTION TIMSK1
#define	TC1_OVERFLOW_VECTOR TIMER1_OVF_vect

#define COUNTING_REGISTER TCNT1
#define PRESCALER_256 CS12
#define CALL_INTERRUPT_VECTOR TOIE1

enum PinType { Cathode = 0, Anode = 1 };

class DisplayerClass
{
private:
	const int numberOfSegments = 8;
	const byte lastBit = B10000000;

	bool isInitialized = false;

	class Digit
	{
	public:
		byte binaryRepresentation;	// state of each segment
		uint8_t pin;	// pin to which digit is connected
		void SetSymbol(char c, bool dot, PinType commonPinType);
	}* digits;
	int numberOfDigits;
	uint8_t segmentPins[8];	
	PinType commonPinType;

	int refreshableDigitIndex = 0;

	// blank for initializing new c strings
	char* emptyBlank;

	void InitializeDisplayContainers(int numberOfDigits, const uint8_t segmentPins[8], PinType commonPinType);
	void InitializePins(const uint8_t displayPins[]);
	void InitializeEmptyBlank();
	void InitializeInterruptions(uint32_t refreshRate);
public:
	void Initialize(PinType commonPinType, const uint8_t segmentPins[8], const uint8_t displayPins[], int numberOfDigits, uint32_t refreshRate = 60);
	void Show(const char cstring[] = "");
	void Show(int number);
	void Show(float number);
	void Refresh();
};

extern DisplayerClass Displayer;

#endif
