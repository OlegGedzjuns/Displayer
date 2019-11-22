// Does all the calculations with temperature
#include <DallasTemperature.h>
// Unique 1-Wire interface requires only one port pin for communication with many devices
// Power supply range is 3.0V to 5.5V
#include <OneWire.h>

#include "Displayer.h"

// Setup a 1-Wire manager to communicate with any 1-Wire devices
constexpr auto ONE_WIRE_BUS = 12;
OneWire OneWireManager(ONE_WIRE_BUS);

// Link Dallas Temperature with One Wire Manager
DallasTemperature Sensors(&OneWireManager);


void updateThermometer()
{
	// Requests all possible devices on 1-wire, even if they are not connected
	// The number of devices is hard coded in the header of the 1-wire library, standard value 15
	Sensors.requestTemperatures();

	// Only requests from one device, many times faster
	//Sensors.requestTemperaturesByIndex(0);

	Displayer.Show(Sensors.getTempCByIndex(0));
}

int timer1_counter;

ISR(TIMER1_OVF_vect)
{
	TCNT1 = timer1_counter;
	Displayer.Refresh();
}

void setup()
{
	// Arduino pseudo multithreading using interrupts
	// http://www.hobbytronics.co.uk/arduino-timer-interrupts
	noInterrupts();

	TCCR1A = 0;
	TCCR1B = 0;

	// Set timer1_counter to the correct value for our interrupt interval
	//timer1_counter = 64911;   // preload timer 65536-16MHz/256/100Hz
	//timer1_counter = 64286;   // preload timer 65536-16MHz/256/50Hz
	//timer1_counter = 34286;
	timer1_counter = 65200;

	TCNT1 = timer1_counter;   // preload timer
	TCCR1B |= (1 << CS12);    // 256 prescaler 
	TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
	interrupts();             // enable all interrupts
	
	Sensors.begin();
	const short segmentPins[] = { 2, 3, 4, 5, 6, 7, 8, 9 };
	const short digitPins[] = { 13, 11, 10 };
	Displayer.Initialize(segmentPins, sizeof digitPins / sizeof(short), digitPins);
}

void loop()
{
	updateThermometer();
	delay(1000);
}