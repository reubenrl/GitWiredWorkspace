/*
 * satellite.c
 *
 *  Created on: Jun 16, 2016
 *      Author: phytech
 */

#if defined(WBRD200_VER1) && defined(SATELLITE_MODULE_ENABLE)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#include "bsp.h"
#include "modem.h"
#include "leds.h"
#include "uartDbgPrint.h"
#include "epoch.h"
#include "satellite.h"



// Iridium Epoch Time : 08/03/2007 03:50:21 - dd/mm/yyyy hh:mm:ss
// date_time_t EEMEM ee_epoch_time = {21,50,3,8,3,7}; // ss:mm:hh:dd:mm:yy

// changed to: 11/05/2014 14:23:55
date_time_t EEMEM ee_epoch_time = {55,23,14,11,5,14}; // ss:mm:hh:dd:mm:yy

void satelliteModemPwr(uint8_t status)
{

	if(status){// Satellite Power 5VDC ON/OFF
		sbi(PORTC,SAT_PWR_ENA);
	}else{// Satellite modem OFF
		cbi(PORTC,SAT_PWR_ENA);
	}


	if(status){// Satellite modem ON/OFF
		sbi(PORTC,SAT_PWR_ON);
	}else{// Satellite modem OFF
		cbi(PORTC,SAT_PWR_ON);
	}


}

/*
 * return:
 * 	1 - successful
 *  0 - fail
 */
uint8_t satelliteNetworkAvailable(uint8_t retries, uint8_t period_sec)
{

	do{
		TXT_DBG("Read Network Available");

		if(!(PINC & (1<<SAT_NTW_AVAIBLE))){
			if(satelliteSignalQuality(COM_SATELLITE)){
				TXT_DBG("Good RSSI.");
				return 1; // network available
			}else{
				TXT_DBG("Pour RSSI.");
			}
		}

		modemDelay_sec(period_sec);
	}while(retries--);

	return 0; // not network available
}

/*
 * return:
 * 	0 		- fail
 *  other 	- successful
 */
uint8_t satelliteSignalQuality (uint8_t nUart)
{
	char *res;
	uint8_t rssi = 0;

	res = SendAtCmd_P(nUart,"AT+CSQ\r", "OK", 20);
	if(res != NULL){
		res = strstr(res, ":");
		if(res != NULL){
			if(res[1] > '1') rssi = 1;
		}
	}
	return rssi;
}

/*
 * return:
 * 	1 - successful
 *  0 - fail
 */
uint8_t satelliteSearchBaudrate(uint8_t *p_modemBaurateIdx) //unsigned char *modemBaurateIndexFound)
{
	char* res;
	uint8_t idx;


	if(modemSearchBaudrate(COM_SATELLITE, &idx)){

		res = SendAtCmd_P(COM_SATELLITE, "ATE0;&D0;&K0;&W0\r","OK", 5);

		if(SendAtCmd_P(COM_SATELLITE, "AT\r", "OK", 2) != NULL){
			*p_modemBaurateIdx = idx+1;
			return 1;
		}
	}// end for

	return 0;
}

/* Iridium satellite modem setting.
 * return:
 * 	0 - successful
 *  1 - fail
 */
uint8_t satelliteSetting(uint8_t nUart)
{
	// connect RTS = GND & DTR = GND ... DTR??? checked with power modem on/off pin
	if(SendAtCmd_P(nUart, "ATE0;&D0;&K0;&W0;&Y0\r", "OK", 3) != NULL){
		TXT_DBG("Setting satellite modem successful.");
		return 0;
	}

	return 1;
}



/* Iridium satellite modem communication test.
 * return:
 * 	0 - successful
 *  1 - fail
 */
uint8_t satelliteCommunicationTest(void)
{
	date_time_t epoch_date_time;
	char  *res, *pTmp;
	char strTmp[64];

	if(satelliteNetworkAvailable(10, 10)){
		TXT_DBG("Waiting 5 sec.");
		modemDelay_sec(5);

		res = SendAtCmd_P(COM_SATELLITE, "AT-MSSTM\r", "OK", 0); // get epoch counter - hex 32 bits text string "ABCDEFGH"
		if(modemGetAtCmdResponse(COM_SATELLITE, 20)){
			res = modemGetResponseBuff(COM_SATELLITE);
			if(strstr((const char*)res,"no network service") == NULL){
				pTmp = strstr((const char*)res,":");
				res = &pTmp[2];
				pTmp = strstr((const char*)res,"OK");
				res[(pTmp - res) - 2] = '\0';
				UART_DBG(res);
				epoch_counter = strtoul((const char*)res, &pTmp, 16); // counter epoch time tick 90ms.
				//sprintf_P(&strTmp[0], PSTR("epoch counter: %lu"), epoch_counter);
				//UART_DBG(strTmp);
				// converter iridium epoch system time to seconds => 1sec = 90/1000 => Xsec = counter*(90/1000)
				epoch_counter = (epoch_counter/100)*9;
				//sprintf_P(&strTmp[0], PSTR("epoch counter converted to seconds: %lu sec"), epoch_counter);
				//UART_DBG(strTmp);

				// Iridium Epoch Time : 08/03/2007 03:50:21
				sprintf_P(&strTmp[0], PSTR("Iridium Epoch Time : 11/05/2014 14:23:55"));
				UART_DBG(strTmp);
				epoch_date_time.year 	= eeprom_read_byte(&ee_epoch_time.year);
				epoch_date_time.month 	= eeprom_read_byte(&ee_epoch_time.month);
				epoch_date_time.day 	= eeprom_read_byte(&ee_epoch_time.day);
				epoch_date_time.hour 	= eeprom_read_byte(&ee_epoch_time.hour);
				epoch_date_time.minute 	= eeprom_read_byte(&ee_epoch_time.minute);
				epoch_date_time.second 	= eeprom_read_byte(&ee_epoch_time.second);

				unsigned long iridium_epoch_time_seconds = date_time_to_epoch(&epoch_date_time);
				sprintf_P(&strTmp[0], PSTR("iridium epoch time converted to seconds: %lu sec"), iridium_epoch_time_seconds);
				UART_DBG(strTmp);

				date_time_t  local_date_time;

				epoch_to_date_time(&local_date_time , epoch_counter + iridium_epoch_time_seconds);
				sprintf_P(&strTmp[0], PSTR("Local Date: dd/mm/yy: %d/%d/%d"), local_date_time.day, local_date_time.month, local_date_time.year);
				UART_DBG(strTmp);
				sprintf_P(&strTmp[0], PSTR("Local Time: hh:mm:ss: %d:%d:%d"), local_date_time.hour, local_date_time.minute, local_date_time.second);
				UART_DBG(strTmp);

				return 0;
			}
		}
	}else TXT_DBG("Network not available.");

	return 1;
}

#endif

