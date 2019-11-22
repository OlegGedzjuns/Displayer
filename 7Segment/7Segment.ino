// does all the calculations with temperature
#include <DallasTemperature.h>
// Unique 1-Wire interface requires only one port pin for communication with many devices
// Power supply range is 3.0V to 5.5V
#include <OneWire.h>
// Protothreads library
#include <TimedAction.h>

#include "Displayer.h"

// Setup a oneWire instance to communicate with any OneWire devices
constexpr auto ONE_WIRE_BUS = 13;
OneWire OneWireManager(ONE_WIRE_BUS);

// link Dallas Temperature with One Wire Manager
DallasTemperature Sensors(&OneWireManager);

int timedCounter = 0;

void incrementNumber()
{
	Displayer.Show(timedCounter);
	++timedCounter;
}

void updateThermometer()
{
	// requests all possible devices on 1-wire, even if they are not connected
	// the number of devices is hard coded in the header of the 1-wire library, standard value 15
	//Sensors.requestTemperatures();

	// only requests from one device, many times faster
	Sensors.requestTemperaturesByIndex(0);

	Displayer.Show(Sensors.getTempCByIndex(0));
}

TimedAction NumberThread = TimedAction(500, incrementNumber);
TimedAction ThermometerThread = TimedAction(3000, updateThermometer);

void setup()
{
	Sensors.begin();
	const short segmentPins[] = { 2, 3, 4, 5, 6, 7, 8, 9 };
	const short digitPins[] = { 10, 11, 12 };
	Displayer.Initialize(segmentPins, sizeof digitPins / sizeof(short), digitPins, 60);
}

void loop() 
{
	// Simple timer
	// NumberThread.check();

	// Thermometer using 1-Wire
	ThermometerThread.check();

	Displayer.Refresh();
}
