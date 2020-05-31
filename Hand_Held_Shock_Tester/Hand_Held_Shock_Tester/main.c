/*
 * Hand_Held_Shock_Tester.c
 *
 * Created: 19-02-2020 13:58:15
 * Author : mikkel
 */ 

#define vref 4.8
#define F_CPU 16000000UL
#define analog_sensitivity 0.3
#define zero_g_voltage 1.65
#define BAUDRATE 9600
#define BAUD_PRESCALER ((F_CPU/(16UL*BAUDRATE))-1)
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
float conversion(int raw);
char transmit = 0;
char transdata1 = 0;
char transdata2 = 0;

int main(void)
{
	initialse();	
//	LCD_init();
	int x_raw, y_raw, z_raw;
	float x_g, y_g, z_g;
	int sensor_values[6];
	uint16_t b = 0;
	uint8_t checksum;
	
    while (1) 
    {
	//	get_analog_accelerometer(&x_raw , &y_raw, &z_raw);
	//	convert_raw_to_g(x_raw, y_raw, z_raw, &x_g, &y_g, &z_g);
	_delay_ms(200);
	//LCD_clear();
	//	LCD_set_cursor(0,0);
	//	printf("x=%d y=%d z=%d", read_adc(0),read_adc(1),read_adc(2));
	//	LCD_set_cursor(0,1);
	//	printf("x=%0.1f y=%0.1f, z=%01.f", x_g, y_g, z_g);
	//printf("x=%d y=%d z=%d",read_adc(0),read_adc(1),read_adc(2));
	get_data_accel(&x_raw,&y_raw,&z_raw);
	//LCD_set_cursor(0,2);
	//printf("x=%0.2f y=%0.2f z=%0.2f \n", (float)x_raw/4096,(float)y_raw/4096,(float)z_raw/4096);
	if (transmit == 1)
		{
			
			checksum = 0;
			
			for (int n = 0; n<6 ;n++)
			{
				b = sensor_values[n]<<8;
				transdata1 = sensor_values[n]>>8;
				transdata2 =  b>>8;
				checksum ^= transdata1;
				checksum ^= transdata2; 
				USART_send(transdata1);
				USART_send(transdata2);
			}
			USART_send(checksum);
			transmit = 0;
			
		}
    }
}


void convert_raw_to_g(int x_raw, int y_raw, int z_raw, float *x_g, float *y_g, float *z_g)
{
	*x_g = conversion(x_raw);
	*y_g = conversion(y_raw);
	*z_g = conversion(z_raw);
}

float conversion(int raw)
{
	return (float)((raw * (vref/1024))-zero_g_voltage)/analog_sensitivity;
}

void get_analog_accelerometer(int *x, int *y, int *z)
{
	*x = read_adc(0);
	*y = read_adc(1);
	*z = read_adc(2);
}
