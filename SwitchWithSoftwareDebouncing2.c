// SwitchWithSoftwareDebouncing2.c

// ???

#ifndef F_CPU					// if F_CPU was not defined in Project -> Properties
#define F_CPU 1000000UL			// define it now as 1 GHz unsigned long
#endif

#include <avr/io.h>				// this is always included in AVR programs
#include <util/delay.h>			// add this to use the delay function
#include <avr/interrupt.h>
#include <stdbool.h>

#define BIT_IS_SET(byte, bit) (byte & (1 << bit))
#define BIT_IS_CLEAR(byte, bit) (!(byte & (1 << bit)))

bool blnButtonWasPressed = false;

///////////////////////////////////////////////////////////////////////////////////////////////////
int main(void) {
	
	DDRC &= ~(1 << PC5);		// clear DDRC bit 5, sets PC5 (pin 28) for input
	
	PORTC |= (1 << PC5);		// set PC5 (pin 28) internal pull-up resistor
	
	/*
	PCICR - Pin Change Interrupt Control Register
	
	bit         7       6       5       4       3         2          1        0
	name        -       -       -       -       -       PCIE2      PCIE1    PCIE0
	set to      0       0       0       0       0         0          1        0
	
	bit 7 = 0
	bit 6 = 0
	bit 5 = 0
	bit 4 = 0
	bit 3 = 0
	PCIE2 = 0     don't enable pin change interrupt on pins PCINT23 through PCINT16
	PCIE1 = 1     enable pin change interrupt on pins PCINT14 through PCINT8
	PCIE0 = 0     don't enable pin change interrupt on pins PCINT7 through PCINT0
	*/
	PCICR = 0b00000010;
	
	/*
	PCMSK1 - Pin Change Mask Register 1
	
	bit           7           6           5           4          3            2           1          0
	name          -        PCINT14     PCINT13     PCINT12     PCINT11     PCINT10      PCINT9     PCINT8
	set to        0           0           1           0           0           0           0          0
	
	bit 7 = 0
	PCINT14 = 0   don't enable PCINT14 pin change interrupt
	PCINT13 = 1   enable PCINT13 pin change interrupt
	PCINT12 = 0   don't enable PCINT12 pin change interrupt
	PCINT11 = 0   don't enable PCINT11 pin change interrupt
	PCINT10 = 0   don't enable PCINT10 pin change interrupt
	PCINT9 = 0    don't enable PCINT9 pin change interrupt
	PCINT8 = 0    don't enable PCINT8 pin change interrupt
	*/
	PCMSK1 = 0b00100000;
	
	/*
	TCCR1A - Timer/Counter1 Control Register A
	
	bit           7         6         5         4        3       2        1        0
	name        COM1A1    COM1A0    COM1B1    COM1B0     -       -      WGM11    WGM10
	set to        0         0         0         0        0       0        0        0
	
	COM1A1 = 0    normal port operation, OC1A disconnected
	COM1A0 = 0
	
	COM1B1 = 0    normal port operation, OC1B disconnected
	COM1B0 = 0
	
	bit 3 = 0
	bit 2 = 0
	
	WGM11 = 0     CTC (Clear Timer on Compare match) mode, see TCCR1B also
	WGM10 = 0
	*/
	TCCR1A = 0b00000000;
	
	/*
	TCCR1B - Timer/Counter1 Control Register B
	
	bit           7          6        5         4          3         2         1        0
	name        ICNC1      ICES1      -       WGM13      WGM12      CS12      CS11     CS10
	set to        0          0        0         0          1         0         1        0
	
	ICNC1 = 0     don't use Input Capture Noise Canceler
	ICES1 = 0     don't use Input Capture Edge Select
	
	bit 5 = 0
	
	WGM13 = 0     CTC (Clear Timer on Compare match) mode, see TCCR1A also
	WGM12 = 1
	
	CS12 = 1
	CS11 = 0      clock / 1024
	CS10 = 1
	*/
	TCCR1B = 0b00001101;
	
	/*
	TCCR1C - Timer/Counter1 Control Register C
	
	bit           7          6        5       4       3       2       1      0
	name        FOC1A      FOC1B      -       -       -       -       -      -
	set to        0          0        0       0       0       0       0      0
	
	FOC1A = 0     don't use Force Output Compare for Channel A
	FOC1B = 0     don't use Force Output Compare for Channel B
	
	bit 5 = 0
	bit 4 = 0
	bit 3 = 0
	bit 2 = 0
	bit 1 = 0
	bit 0 = 0
	*/
	TCCR1C = 0b00000000;
	
	/*
	TIMSK1 - Timer/Counter 1 Interrupt Mask Register
	
	bit           7        6        5        4       3       2         1         0
	name          -        -      ICIE1      -       -     OCIE1B    OCIE1A    TOIE1
	set to        0        0        0        0       0       0         0         0
	
	bit 7 = 0     don't use Force Output Compare A
	bit 6 = 0
	ICIE1 = 0
	bit 4 = 0
	bit 3 = 0
	OCIE1B = 0    don't enable Timer/Counter 1 Output Compare Match B Interrupt
	OCIE1A = 0    don't enable Timer/Counter 1 Output Compare Match A Interrupt Enable yet, we will later
	TOIE1 = 0     don't enable Timer/Counter 1 Overflow Interrupt
	*/
	TIMSK1 = 0b00000000;
	
	OCR1AH = 0b00000001;
	OCR1AL = 0b00000000;

	
	DDRD = 0xFF;				// set Port D pins for output
	
	
	
	sei();				// enable interrupts
	
	while (1) { }
	
	return(0);					// should never get here, this is to prevent a compiler warning
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ISR(PCINT1_vect) {
	
	//DDRC |= (1 << PC4); PORTC |= (1 << PC4);		// DEBUG
	
	if(BIT_IS_CLEAR(PINC, PC5)) {			// if switch is pressed (logic low)
		
			
		cli();
		
		PCMSK1 &= ~(1 << PCINT13);			// temporarily disable PCINT13 pin change interrupt to disable switch input
		//DDRC |= (1 << PC5);
		PCIFR &= ~(1 << PCIF1);
		
		PORTD++;
		
		TCNT1 = 0x00;						// set counter back to zero
		
		TIMSK1 |= (1 << OCIE1A);			// start counter
		
		sei();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ISR(TIMER1_COMPA_vect) {
	
	TIMSK1 &= ~(1 << OCIE1A);				// stop counter
	
	PCIFR &= ~(1 << PCIF1);					// clear interrupt flag before re-enabling interrupt just in case
	
	blnButtonWasPressed = false;
	
	PCMSK1 |= (1 << PCINT13);				// re-enable PCINT13 pin change interrupt to re-enable switch input
	
	//DDRC &= ~(1 << PC5);
	
}



