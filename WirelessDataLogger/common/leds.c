/*
 * leds.c
 *
 *  Created on: Oct 31, 2013
 *      Author: reuben
 */

#include <avr/io.h>
#include "bsp.h"
#include "leds.h"

#define led_on(n)	PORTA |= (1<<(n+5))
#define led_off(n)	PORTA &= ~(1<<(n+5))

static unsigned tick_led;
static uint32_t led_tick_50ms_counter;

void ledIni(void)
{
	DDRA |= (1<<LED3_IO) | (1<<LED2_IO) | (1<<LED1_IO);
	PORTA &= ~((1<<LED3_IO) | (1<<LED2_IO) | (1<<LED1_IO));

}

void ledTasking(void)
{
	uint8_t i;

	// tick = 50ms
	// changed timer0, compareA, tick 50ms.
	tick_led++;
	for(i=0;i<3;i++){
		if(led_stat[i]){
			if(led_stat[i] == 255) led_on(i);
			else{
				if(led_stat[i] & tick_led) led_on(i);
				else led_off(i);
			}

		}else led_off(i);
	}// for

	led_tick_50ms_counter++;
}

uint32_t ledGetTickCounter(void)
{
	return led_tick_50ms_counter;
}
