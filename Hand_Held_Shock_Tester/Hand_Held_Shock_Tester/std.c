/*
 * std.c
 *
 * Created: 19-02-2020 14:28:59
 *  Author: mikkel
 */ 
#include "std.h"
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>


volatile unsigned long timer0_overflow_count = 0;
volatile unsigned long timer0_millis = 0;
static unsigned char timer0_fract = 0;

ISR (TIMER0_OVF_vect) // timer0 overflow interrupt
{
	timer0_millis += MILLIS_INC;
	timer0_fract += FRACT_INC;
	if (timer0_fract >= FRACT_MAX) {
		timer0_fract -= FRACT_MAX;
		timer0_millis += 1;
	}
	timer0_overflow_count++;
}


unsigned long millis()
{
	unsigned long m;
	// disable interrupts while we read timer0_millis or we might get an
	// inconsistent value (e.g. in the middle of a write to timer0_millis)
	cli();
	m = timer0_millis;
	sei();
	return m;
}


unsigned int read_adc(char channel)
{
	ADMUX &= 0xf0; // clears preivesly selected channel
	ADMUX |= channel; //sets adc multiplexer to channel
	ADCSRA |= (1<<ADSC); // Starts conversion
	while(ADCSRA & (1<<ADSC)); //"aits for conversion to complete
	return ADC; //Returns value from conversion
}