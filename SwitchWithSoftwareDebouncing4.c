// SwitchWithoutDebouncing.c

// put LED on PC5

#ifndef F_CPU					// if F_CPU was not defined in Project -> Properties
#define F_CPU 1000000UL			// define it now as 1 GHz unsigned long
#endif

#include <avr/io.h>				// this is always included in AVR programs
#include <util/delay.h>			// add this to use the delay function

#define BIT_IS_SET(byte, bit) (byte & (1 << bit))
#define BIT_IS_CLEAR(byte, bit) (!(byte & (1 << bit)))

#define NUM_OF_CONSECUTIVE_PRESSES 9500

///////////////////////////////////////////////////////////////////////////////////////////////////
int main(void) {
	
	DDRC &= ~(1 << PC5);		// clear DDRC bit 5, sets PC5 (pin 28) for input
	
	PORTC |= (1 << PC5);		// set PC5 (pin 28) internal pull-up resistor
	
	DDRD = 0xFF;				// set Port D pins for output
	
	int intConsecutivePresses = 0;
	
	while (1) {					// begin infinite loop
		if(BIT_IS_CLEAR(PINC, PC5)) {				// if button is pressed (logic low)
			intConsecutivePresses++;
			if(intConsecutivePresses >= NUM_OF_CONSECUTIVE_PRESSES) {
				PORTD++;
				intConsecutivePresses = 0;
			}
		} else if(BIT_IS_SET(PINC, PC5)) {			// if button is not pressed (logic low)
			intConsecutivePresses = 0;
		} else {
			// ideally should never get here, but may occasionally due to timing
		}
	}
	return(0);					// should never get here, this is to prevent a compiler warning
}




