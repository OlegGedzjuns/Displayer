
# Displayer
#### Asynchronous, flexible, customizable library for managing seven-segment indicators under Arduino control

## Important! May not be compatible with Servo library

## Features:
- asynchronous display refresh
- user specified refresh rate (+/- 3Hz)
- user specified number of displays
- support different common pins ( cathode / anode )
- display strings, negative or positive integers or floats
- support for most latin characters ( excluding K, M, V, W, X, Z )

## Guide:
### Instalation
 1. Download the Displayer_vX.X.zip file added to the [latest release](https://github.com/SecyrBb/Displayer/releases "Releases").
 2. Unzip the downloaded archive into the folder "~Documents/Arduino/libraries/"
### Use
 1. Include library header
~~~cpp
#include <Displayer.h>
~~~
 2. Сreate two constant arrays listing segments pins and indicator common pins inside setup() and call Inintialization method
~~~cpp
void setup()
{
	// From A to DP
	const short segmentPins[] = { 2, 3, 4, 5, 6, 7, 8, 9 };
	
	// From left to right
	const short сommonPins[] = { 13, 11, 10 };
	
	// No need to create a prototype of the Displayer class
	Displayer.Initialize
	(
		Common_cathode,			 // Common pin type on your display (Common_cathode or Common_anode) 
		segmentPins,			 // The array of segment pins you created 
		sizeof сommonPins/ sizeof(short),// Number of indicators 
		сommonPins,			 // The array of indicator pins you created 
		75				 // Required display refresh rate 
	);
}
~~~
3. Call the Show() method to display something
### Examples:
~~~cpp
// Simple int counter
int number = 1;

void loop()
{
	Displayer.Show(number);
	++number;
	// you can lock the main cycle, the display will still be refreshing
	delay(1000);
}
~~~
~~~cpp
// Simple float counter
float number = 0.f;

void loop()
{
	Displayer.Show(number);
	number += 0.1f;
	// you can lock the main cycle, the display will still be refreshing
	delay(1000);
}
~~~
~~~cpp
// Show() method capabilities
void loop()
	// String will be displayed until the next Show() method call
	
	Show("ABCXF");
	// characters that cannot be displayed on the seven-segment indicator will be ignored
	// if string or number does not fit on the display, the right side will preferably be displayed.
	// On the display with 3 indicators the line will be displayed like this: "C F"
  	delay(1000);
	
	Show(12345);
	// On the display with 3 indicators the line will be displayed like this: "345"
  	delay(1000);
	
	Show(-0.125f);
	// The maximum possible number of decimal places will be displayed
	// First zero will be ignored
	// On the display with 3 indicators the line will be displayed like this: "-.125"
  	delay(1000);
}
~~~

### Thanks to:
- [Ri0ee](https://github.com/Ri0ee) for helping with binary segment encoding
- [E](https://arduinodiy.wordpress.com/2012/02/28/timer-interrupts/) for information about Arduino interruptions 
- and [biokin](https://habr.com/ru/post/453276/) for translation
