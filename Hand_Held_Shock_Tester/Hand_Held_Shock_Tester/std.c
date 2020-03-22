/*
 * std.c
 *
 * Created: 19-02-2020 14:28:59
 *  Author: mikkel
 */ 
#include "std.h"
#include "i2cmaster.h"
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

void initialse()
{
	register_setup();
	i2c_init();
	i2c_start(accel_i2c_adr_a0_write);
	i2c_write(0x03);
	
	i2c_stop();
}

void register_setup()
{
	cli();
	//Timer0 setup
	TCCR0B |= 0x03; //Sets prescaler for timer0 to 64
	TIMSK0 |= 0x01; //Enables interupt on timer0 overflow

	//Port setup
	DDRD = 0x87; //I/O board:PD4?7 as inputs
	DDRC = 0xF0; //I/O board PC0?3 as inputs, for buttons
	DDRB = 0xF0;
	PORTB = 0x00;
	PORTC = 0x30; // Enable internal pull at PC0..3 inputs
	PORTD = 0x00; // Set output LEDs to off
	
	ADMUX = 0x40; // sets Vref to Avcc
	ADCSRA = 0x87; // Enables ADC and sets prescaler to 128
	
	EECR = 0x00;
	
	EICRA |= (1<<ISC11) | (1<<ISC10); //sets int1 to trigger on rising edge
	EIMSK |= (1<<INT1); //enables int1 external interrupt
	
	sei();
}

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

void get_data_accel(int *x, int *y, int *z)
{
	int raw;
	i2c_start(accel_i2c_adr_a0);
	i2c_write(OUT_X_MSB);
	*x = i2c_readAck();
	*y = i2c_readAck();
	*z = i2c_readNak();

	i2c_stop();

}

void accel_calibrate()
{
	char x, y, z;
	i2c_start(accel_i2c_adr_a0_read);
	i2c_write(OUT_X_MSB);
	x = i2c_readAck();
	y = i2c_readAck();
	z = i2c_readNak();
	i2c_stop();
	i2c_start(accel_i2c_adr_a0_write);
	i2c_write(OFF_X);
	i2c_write(-x);
	i2c_stop();
	
	i2c_start(accel_i2c_adr_a0_write);
	i2c_write(OFF_X);
	i2c_write(-x);
	i2c_stop();
	
	i2c_start(accel_i2c_adr_a0_write);
	i2c_write(OFF_Z);
	i2c_write(32-z);
	i2c_stop();
}
