/*
 * timer_sys.c
 *
 *  Created on: Mar 15, 2017
 *      Author: phytech
 */

#include "bsp.h"
#include "timer.h"
#include "timer_sys.h"

void timer_sysCallbackOneMs(void);

volatile unsigned long tmr_sysTick_msec;
volatile unsigned int tmr_sysTick_sec;

volatile unsigned int tmr_sysDelay_sec;
volatile unsigned long tmr_sysDelay_msec;

volatile unsigned int tmr_sysTimeout_sec;
volatile unsigned int tmr_sysTimeout_msec;

void timer_sysCallbackOneMs(void)
{
	tmr_sysTick_msec++;

	if(!(tmr_sysTick_msec % 1000UL)){ // 1 second
		tmr_sysTick_sec++;
		if(tmr_sysTimeout_sec) tmr_sysTimeout_sec--;
		if(tmr_sysDelay_sec) tmr_sysDelay_sec--;
	}

	if(tmr_sysDelay_msec) tmr_sysDelay_msec--;
	if(tmr_sysTimeout_msec) tmr_sysTimeout_msec--;
}

/// delay miliseconds
void timer_sysDelay_msec(unsigned long delay_msec)
{
	tmr_sysDelay_msec = delay_msec;
	while(tmr_sysDelay_msec);
}

// get timeout miliseconds
unsigned int timer_sysGetTimeout_msec(void)
{
	return tmr_sysTimeout_msec;
}
// set timeout miliseconds
void timer_sysSetTimeout_msec(unsigned int timeout_msec)
{
	tmr_sysTimeout_msec = timeout_msec;
}

/// delay seconds
void timer_sysDelay_sec(unsigned int delay_sec)
{
	tmr_sysDelay_sec = delay_sec;
	while(tmr_sysDelay_sec);
}

// get timeout seconds
unsigned int timer_sysGetTimeout_sec(void)
{
	return tmr_sysTimeout_sec;
}

// set timeout seconds
void timer_sysSetTimeout_sec(unsigned int timeout_sec)
{
	tmr_sysTimeout_sec = timeout_sec;
}

void timer_sysInit(void)
{
	tmr_sysTick_msec = 0;
	tmr_sysTimeout_msec = 0;
	tmr_sysTick_sec = 0;
	tmr_sysTimeout_sec = 0;

	timer2CompareAInit(timer_sysCallbackOneMs);


}

