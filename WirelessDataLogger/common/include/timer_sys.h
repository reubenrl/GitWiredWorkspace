/*
 * timer_sys.h
 *
 *  Created on: Mar 15, 2017
 *      Author: phytech
 */

#ifndef TIMER_SYS_H_
#define TIMER_SYS_H_


void timer_sysInit(void);

void timer_sysDelay_msec(unsigned long delay_msec);
unsigned int timer_sysGetTimeout_msec(void);
void timer_sysSetTimeout_msec(unsigned int timeout_msec);

void timer_sysDelay_sec(unsigned int delay_sec);
unsigned int timer_sysGetTimeout_sec(void);
void timer_sysSetTimeout_sec(unsigned int timeout_sec);


#endif /* TIMER_SYS_H_ */
