/*
 * tcnt0.c
 *
 *	an 8 - bit timer / counter0 with PWM
 *	BOTTOM refers to 0x00, when the timer clears, MAX or 0xFF is the maximum counter, and TOP refers to the instance the counter reaches the value stored in OCR0A (output compare register 0)
 *
 * Created: 20/03/2017 6:01:26 PM
 *  Author: Callum
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "tcnt0.h"

static volatile uint32_t tcnt0_ticks;


// variables used for checking PINC5
volatile uint8_t Button_on;
static uint8_t button;
static uint8_t BTN = PINC5;


void init_tcnt0(void){
	
	// set global counter
	tcnt0_ticks = 0L;
	
	// set timer / counter
	TCNT0 = 0;
	
	// set output compare resgister; value to execute ISR
	OCR0A = 124; // between 1 and 255
	
	// timer/counter control register A - set to clear when TCNT0 matches OCR0A
	TCCR0A = (1<<WGM01);
	
	// set the timer to update at a fraction of a clock cycle
	TCCR0B = (1<<CS01)|(1<<CS00);	// set to f(clk) / 64
	
	// timer/counter 1 interrupt mask register - enable OCR0A as output compare register, only works if OCF0A is set in TIFR0
	TIMSK0 |= (1<<OCIE0A);
	
	// if it isn't already, clear the interrupt output compare flag by writing a 1 to OCF0A; switches when TCNT0 matches OCR0A
	TIFR0 &= (1<<OCF0A);
}


uint32_t get_tcnt0_ticks(void) {
	uint32_t return_value;

	uint8_t interrupts_on = bit_is_set(SREG, SREG_I);
	cli();
	return_value = clock_ticks;
	if(interrupts_on) {
		sei();
	}
	return return_value;
}


uint8_t	get_button_(void){
	/* checks if PINC5 is active */
	uint8_t interrupts_on = bit_is_set(SREG, SREG_I);
	cli();
	return_value = Button_on;
	if(interrupts_on){
		sei();
	}
	return return_value;
}


ISR(TIMER0_COMPA_vect) {
	/* Increment our clock tick count */
	tcnt0_ticks++;
	
	// check PINC5
	if(button && (1<<BTN)){
		Button_on = 1;
	}
	else{
		Button_on = 0;
	}
}
