// SwitchWithSoftwareDebouncing5.c

// put LED on PC5

#ifndef F_CPU					// if F_CPU was not defined in Project -> Properties
#define F_CPU 1000000UL			// define it now as 1 GHz unsigned long
#endif

#include <avr/io.h>				// this is always included in AVR programs
#include <avr/interrupt.h>

#define BIT_IS_SET(byte, bit) (byte & (1 << bit))
#define BIT_IS_CLEAR(byte, bit) (!(byte & (1 << bit)))

#define NUM_OF_CONSECUTIVE_PRESSES 350

volatile int intConsecutivePresses = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
int main(void) {
	
	DDRC &= ~(1 << PC5);		// clear DDRC bit 5, sets PC5 (pin 28) for input
	
	PORTC |= (1 << PC5);		// set PC5 (pin 28) internal pull-up resistor
	
	DDRD = 0xFF;				// set Port D pins for output
	
	/*
	TCCR0A - Timer/Counter 0 Control Register A
	
	bit           7         6         5         4        3       2        1        0
	name        COM0A1    COM0A0    COM0B1    COM0B0     -       -      WGM01    WGM00
	set to        0         0         0         0        0       0        0        0
	
	COM0A1 = 0    normal port operation, OC0A disconnected
	COM0A0 = 0
	
	COM0B1 = 0    normal port operation, OC0B disconnected
	COM0B0 = 0
	
	bit 3 = 0
	bit 2 = 0
	
	WGM01 = 0     "Normal" timer/counter mode
	WGM00 = 1
	*/
	TCCR0A = 0b00000000;
	
	/*
	TCCR0B - Timer/Counter 0 Control Register B
	
	bit           7          6        5       4         3         2         1        0
	name        FOC0A      FOC0B      -       -       WGM02      CS02      CS01     CS00
	set to        0          0        0       0         0         0         0        1
	
	FOC0A = 0     don't use Force Output Compare A
	FOC0B = 0
	
	bit 5 = 0
	bit 4 = 0
	
	WGM02 = 0     "Normal" timer/counter mode, see TCCR0A
	
	CS02 = 0
	CS01 = 0      no prescaling
	CS00 = 1
	*/
	TCCR0B = 0b00000001;
	
	/*
	TIMSK0 - Timer/Counter 0 Interrupt Mask Register
	
	bit           7        6        5       4       3       2         1         0
	name          -        -        -       -       -     OCIE0B    OCIE0A    TOIE0
	set to        0        0        0       0       0       0         0         1
	
	bit 7 = 0     don't use Force Output Compare A
	bit 6 = 0
	bit 5 = 0
	bit 4 = 0
	bit 3 = 0
	OCIE0B = 0    don't enable Timer/Counter 0 Output Compare Match B Interrupt
	OCIE0A = 0    don't enable Timer/Counter 0 Output Compare Match A Interrupt Enable
	TOIE0 = 1     enable Timer/Counter 0 Overflow Interrupt
	*/
	TIMSK0 = 0b00000001;
	
	sei();				// enable interrupts
	
	while (1) {					// begin infinite loop
		/*
		if(BIT_IS_CLEAR(PINC, PC5)) {
			PORTD++;
			
		}
		*/
	}
	return(0);					// should never get here, this is to prevent a compiler warning
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ISR(TIMER0_OVF_vect) {
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


