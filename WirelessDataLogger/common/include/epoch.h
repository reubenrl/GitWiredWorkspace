/*
 * epoch.h
 *
 *  Created on: Jan 11, 2015
 *      Author: reuben
 */

#ifndef EPOCH_H_
#define EPOCH_H_

typedef struct
{
    unsigned char second; // 0-59
    unsigned char minute; // 0-59
    unsigned char hour;   // 0-23
    unsigned char day;    // 1-31
    unsigned char month;  // 1-12
    unsigned char year;   // 0-99 (representing 2000-2099)
}date_time_t;


volatile unsigned long epoch_counter;				// The current Iridium system time (counter) available from the network

unsigned long date_time_to_epoch(date_time_t* date_time);
void epoch_to_date_time(date_time_t* date_time,unsigned long epoch);

#endif /* EPOCH_H_ */
