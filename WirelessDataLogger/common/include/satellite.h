/*
 * satellite.h
 *
 *  Created on: Dec 10, 2015
 *      Author: reuben
 */

#ifndef SATELLITE_H_
#define SATELLITE_H_

#define MAX_SBD_MODEM_BR 			7
#define SBD_MODEM_BR_600			1
#define SBD_MODEM_BR_1200			2
#define SBD_MODEM_BR_2400			3
#define SBD_MODEM_BR_4800			4
#define SBD_MODEM_BR_9600			5
#define SBD_MODEM_BR_19200			6
#define SBD_MODEM_BR_38400			7

#define SATELLITE_PWR_ON			1
#define SATELLITE_PWR_OFF			1


/* Nonzero if YEAR is a leap year (every 4 years, except every 100th isn't, and every 400th is).  */
# define __isleap(year)	 ((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))

typedef struct
{
	uint8_t	tm_sec;		/* Seconds.			[0-60] (1 leap second) 	*/
	uint8_t	tm_min;		/* Minutes.			[0-59] 					*/
	uint8_t	tm_hour;	/* Hours.			[0-23] 					*/
	uint8_t	tm_mday;	/* Day.				[1-31] 					*/
	uint8_t	tm_mon;		/* Month.			[0-11] 					*/
	uint8_t	tm_year;	/* Year	- 1900.  							*/
	uint8_t	tm_wday;	/* Day of week.		[0-6] 					*/
	uint8_t	tm_yday;	/* Days in year.	[0-365]					*/
	uint8_t	tm_isdst;	/* DST.				[-1/0/1]				*/
}t_tm;

#define IRIDIUM_SYSTEM_TIME(t) ((t*9UL)/100UL) 	// counter = every 90ms => 1 sec = (9/100)*counter
#define IRIDIUM_EPOCH_TIME		"2014/02/01 18:10:25"

#define EPOCHH 		18*3600	// 18 hours to seconds
#define	EPOCHM 		10*60	// 10 minutes to seconds
#define	EPOCHS 		25		// 25 seconds to seconds
#define	EPOCHT 		EPOCHH + EPOCHM + EPOCHS 	// total seconds - reset GMT 03:50:35


#define SECONDSPERDAY 	3600UL*24UL



void satelliteModemInit(void);
void satelliteModemPwr(uint8_t status);
uint8_t satelliteSetting(uint8_t nUart);
uint8_t satelliteSetBaudrate(void);
uint8_t satelliteCommunicationTest(void);
uint8_t satelliteNetworkAvailable(uint8_t retries, uint8_t period_sec);
uint8_t satelliteSignalQuality (uint8_t nUart);

#endif /* SATELLITE_H_ */
