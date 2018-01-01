/*
 * leds.h
 *
 *  Created on: Oct 31, 2013
 *      Author: reuben
 */

#ifndef LEDS_H_
#define LEDS_H_

#define LED1		1
#define LED2		2
#define LED3		3

#define LED_ON		-1
#define LED_OFF	 	0

#define LED_TIME_PERIOD_100MS		(unsigned char)(1 << 0)
#define LED_TIME_PERIOD_200MS		(unsigned char)(1 << 1)
#define LED_TIME_PERIOD_400MS		(unsigned char)(1 << 2)
#define LED_TIME_PERIOD_800MS		(unsigned char)(1 << 3)
#define LED_TIME_PERIOD_1600MS		(unsigned char)(1 << 4)
#define LED_TIME_PERIOD_3200MS		(unsigned char)(1 << 5)
#define LED_TIME_PERIOD_6400MS		(unsigned char)(1 << 6)
#define LED_TIME_PERIOD_12800MS		(unsigned char)(1 << 7)

volatile unsigned char led_stat[3];

#define ledControl(n,s) led_stat[(n - 1)] = (s)

void ledIni(void);
void ledTasking(void);
/*
 * ledGetTickCounter = 50ms
 */
uint32_t ledGetTickCounter(void);

#endif /* LEDS_H_ */
