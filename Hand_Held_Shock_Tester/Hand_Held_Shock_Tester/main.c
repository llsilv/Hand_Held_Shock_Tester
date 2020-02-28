/*
 * Hand_Held_Shock_Tester.c
 *
 * Created: 19-02-2020 13:58:15
 * Author : mikkel
 */ 

#define vref 4.8
#define F_CPU 16000000UL
#define analog_sensitivity 0.33

//include standard libraries
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "i2cmaster.h"
#include "lcd.h"
#include "usart.h"
#include "std.h"

void get_analog_accelerometer(int *x, int *y, int *z);
void convert_raw_to_g(int x_raw, int y_raw, int z_raw, float *x_g, float *y_g, float *z_g);
void conversion(int raw, int *g);

int main(void)
{
	register_setup();
	int x_raw, y_raw, z_raw;
	float x_g, y_g, z_g;
	
    while (1) 
    {
		get_analog_accelerometer(&x_raw , &y_raw, &z_raw);
		convert_raw_to_g(int x_raw, int y_raw, int z_raw, float *x_g, float *y_g, float *z_g);
    }
}


void convert_raw_to_g(int x_raw, int y_raw, int z_raw, float *x_g, float *y_g, float *z_g)
{
	conversion(x_raw, x_g);
	conversion(y_raw, y_g);
	conversion(z_raw, z_g);
}

void conversion(int raw, float*g)
{
	*g  = raw * (vref/1024)*analog_sensitivity;
}

void get_analog_accelerometer(int *x, int *y, int *z)
{
	*x = read_adc(0);
	*y = read_adc(1);
	*z = read_adc(2);
}