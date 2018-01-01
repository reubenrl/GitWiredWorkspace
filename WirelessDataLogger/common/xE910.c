/*
 * xE910.c
 *
 *  Created on: Jun 7, 2016
 *      Author: phytech
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

#include "bsp.h"
#include "timer.h"
#include "buffer.h"
#include "uart2.h"
#include "timer_sys.h"
#include "uartDbgPrint.h"
#include "leds.h"
#include "xE910.h"

volatile t_e_MODEN_GENERATION	e_MODEN_GENERATION = UNKNOW_GENERATION;
volatile t_e_MODEL_MODEM		e_MODEL_MODEM = MODEL_MODEM_UNKNOW;

volatile t_E_AccessTechnology  E_AccessTechnology;
//const char strRoamingApn[] = APN_MOVISTAR;
const char strRoamingApn[] = APN_VERIZON;
const char strLocalApn[] = APN_LOCAL;
static unsigned char isSimRoaming;

// local functions
uint8_t xE910SimCardChk(uint8_t nUart);
uint8_t xE910SimCardIsRoaming(uint8_t nUart);
uint8_t xE910ApnUpdate(uint8_t nUart, const char strApnUpdate[]);
uint8_t xE910AccessTechnology(uint8_t nUart);
/*
void xE910ModemHwShDwnUnconditional(void)
{
	TXT_DBG("modem3GHwShDwnUnconditional");
	sbi(PORTC, GSM_SHDW_ENA); 	// start pulse low
	timer_sysDelay_msec(200);
	cbi(PORTC, GSM_SHDW_ENA);	// end pulse. released
	timer_sysDelay_sec(1);
}
*/

/*
 * return:
 * 		0	- failed
 * 		1	- successful
 */
/*
uint8_t xE910ModemPwrOn(void)
{
	uint32_t u32BattValue;
	uint8_t powerUp = 0, shdwUnconditionalRetries = 2;

	u32BattValue = batteryReadVoltageMeasure();
	if(u32BattValue > 3220UL){
		if(PINC & (1<<GSM_PWR_MON)){// modem is OFF
			sbi(PORTC, GSM_IGNITION); 	// start ignition pulse low
			timer_sysDelay_sec(5);
			cbi(PORTC, GSM_IGNITION);	// end ignition pulse released
			timer_sysDelay_sec(1);
			if(PINC & (1<<GSM_PWR_MON)){// modem is OFF
				do{
					xE910ModemHwShDwnUnconditional();
					timer_sysDelay_sec(5);
					shdwUnconditionalRetries--;
				}while((PINC & (1<<GSM_PWR_MON)) && shdwUnconditionalRetries);
				if(shdwUnconditionalRetries){// modem is ON
					powerUp = 1;
				}
			}else{// modem is ON
				powerUp = 1;
			}
		}else{// modem is ON
			powerUp = 1;
		}
	}else TXT_DBG("Protect modem 3G... Low battery.");

	timer_sysDelay_sec(1);

	return powerUp;
}
*/
/*
 * return:
 * 		0	- successful
 * 		1	- failed
 */
/*
uint8_t xE910ModemPwrOff(uint8_t nUart, uint8_t atOffMode)
{
	uint8_t offModeFail=0, shdwUnconditionalRetries=2;

	if(!(PINC & (1<<GSM_PWR_MON))){// modem is ON
		if(atOffMode){// AT OFF mode
			TXT_DBG("AT OFF mode");
			if(SendAtCmd_P(nUart, "AT#SHDN\r", "OK", 5) == NULL) offModeFail = 1;
			modemAtCmdTimeoutSec = 15;
			while(!(PINC & (1<<GSM_PWR_MON)) && modemAtCmdTimeoutSec);
			if(!modemAtCmdTimeoutSec) offModeFail = 1;

		}else{// Key OFF mode
			TXT_DBG("Key OFF mode");
			sbi(PORTC, GSM_IGNITION); 	// start ignition pulse low
			timer_sysDelay_sec(4);
			cbi(PORTC, GSM_IGNITION);	// end ignition pulse released
			timer_sysDelay_sec(1);
		}

		if(offModeFail){
			do{
				xE910ModemHwShDwnUnconditional();
				timer_sysDelay_sec(5);
				shdwUnconditionalRetries--;
			}while(!(PINC & (1<<GSM_PWR_MON)) && shdwUnconditionalRetries);

			if(shdwUnconditionalRetries){
				offModeFail = 1;
			}
		}
	}

	return offModeFail;
}
*/


 /*
  * RETURN:
  * 0 		- successful
  * other 	- failed
  * 			- 1 general
  * 			- 2 sim card not present
  * 			- 3 baudrate
  */
uint8_t xE910tSetParm(uint8_t nUart)
{
	char *res;

	if(debugON) TXT_DBG("----  Modem Information  ----");

	res = SendAtCmd_P(nUart, "AT+CGMM\r", "OK", 5);
	if(res != NULL){
		if(strstr(res, "GE910") != NULL){
			e_MODEN_GENERATION = GENERATION_2;
			TXT_DBG("Modem 2G Technology ");
		}else if( (strstr_P(res, PSTR("HE910")) != NULL) | (strstr_P(res, PSTR("UE910")) != NULL) ){
			e_MODEN_GENERATION = GENERATION_3;
			TXT_DBG("Modem 3G Technology");
		}else if(strstr_P(res, PSTR("LE910")) != NULL){
			e_MODEN_GENERATION = GENERATION_4;
			if(strstr_P(res, PSTR("LE910-SVL")) != NULL){
				e_MODEL_MODEM = MODEL_MODEM_LE910_SVL;
			}
			TXT_DBG("Modem 4G Technology");
		}else return 1;
	}else return 1;


	(void)SendAtCmd_P(nUart, "AT+CGMR\r", "OK", 2);

	(void)SendAtCmd_P(nUart, "AT+CGMI\r", "OK", 2);
	// IMEI  - International Mobile Equipment Identity
	// Request Product Serial Number Identification
	(void)SendAtCmd_P(nUart, "AT+CGSN\r", "OK", 2);
	(void)SendAtCmd_P(nUart, "AT+WS46?\r","OK", 5);


	if(debugON) TXT_DBG("----  Modem Setting  ----");

	if(xE910SimCardChk(nUart)){
		isSimRoaming = xE910SimCardIsRoaming(nUart);
		if(isSimRoaming != -1){
			if(isSimRoaming) TXT_DBG("Roaming Sim Card Detected.");
			else TXT_DBG("Local Sim Card Detected.");
		}else{
			TXT_DBG("Unknown Sim Card Detected.");
			return 2;
		}

		if(SendAtCmd_P(nUart, "AT#STIA=1\r", "OK", 2) == NULL) return 1;
	}else{
		return 2;
	}

	if( (e_MODEN_GENERATION == GENERATION_3) || (e_MODEN_GENERATION == GENERATION_4)){

//		if(SendAtCmd_P(nUart, "AT$GPSP?\r", "$GPSP:", 3) != NULL){
	//		if(SendAtCmd_P(nUart, "AT$GPSP=0;$GPSSAV\r", "OK", 2) == NULL) return 1;
	//	}
		if(SendAtCmd_P(nUart, "AT#GPIO=1,0,2\r", "OK", 2)== NULL)		return 1;
	}

	if(SendAtCmd_P(nUart, "AT#SLED=2,10\r", "OK", 2) == NULL){
		return 1;
	}else if(SendAtCmd_P(nUart, "AT#SLEDSAV\r", "OK", 2) == NULL){
		return 1;
	}

	if(SendAtCmd_P(nUart, "AT&K0;&D0;+ICF=3;&P0;&W\r", "OK", 5) == NULL){
		return 1;
	}

	return 0;
}


/* AT#QSS - Query SIM Status.	>>#QSS: <mode>,<status>
 *
 *	RETURN:
 *		0 - SIM NOT INSERTED
 *		1 - SIM INSERTED
 */
uint8_t xE910SimCardChk(uint8_t nUart)
{
	char *res;

	res = SendAtCmd_P(nUart, "AT#QSS?\r", "OK", 5);
	if(res != NULL){
		res = strstr_P(res, PSTR("#QSS: "));
		if(res != NULL){
			if(res[8] == '1') return 1;
		}
	}

	TXT_DBG("Sim card not found.");
	return 0;
}

/*
 * return:
 * 		0 		- local
 * 		1 		- roaming
 * 		0xFF 	- failed
 */
uint8_t xE910SimCardIsRoaming(uint8_t nUart)
{
	char *res,*pTmp;

	/* Read ICCID (Integrated Circuit Card Identification) */
	res = SendAtCmd_P(nUart, "AT#CCID\r","OK", 5);

	if(res != NULL){
		res = strstr_P(res, PSTR(":"));
		if(res != NULL){
			res += 4;
			res[3] = '\0';
			pTmp =  strstr_P(res, PSTR("972")); // +972 IL
			if(pTmp !=NULL){// local
				return 0;
			}else return 1;
		}
	}

	return -1;
}

/*
 * return:
 * 		0 - successful
 * 		1 - failed
 */
uint8_t xE910CommunicationTest(uint8_t nUart)
{
	const char *pApn;
	char strTmp[64];
	char *res;
	uint8_t i, pdp_context;

	if(debugON) TXT_DBG("------ APN Configuration  ---------");
	else TXT_DBG("APN:");

	if(isSimRoaming) pApn = strRoamingApn;
	else pApn = strLocalApn;

	if(!debugON) UART_DBG((char *)pApn);

	if(xE910ApnUpdate(nUart, pApn)){

		if(isSimRoaming) TXT_DBG("Roaming APN Configuration.");
		else TXT_DBG("Local APN Configuration.");
		if(e_MODEL_MODEM == MODEL_MODEM_LE910_SVL){
			pdp_context = 3;
		}else{
			pdp_context = 1;
		}
		if(sprintf_P(&strTmp[0],PSTR("AT+CGDCONT=%d,\"IP\",%s,\"0.0.0.0\",0,0\r"),pdp_context, pApn)){
			res = SendAtCmd(nUart, strTmp, "OK",5);
			if(res == NULL) return 1;
		}
	}


	res = SendAtCmd_P(nUart, "AT+COPS=0\r","OK", 2);
	timer_sysDelay_sec(5);

	if(modemOperatorSelection(nUart, isSimRoaming)){// operator registered

		if(debugON){
			TXT_DBG("------  Network Information  ---------");
			res = SendAtCmd_P(nUart, "AT+CSQ\r", "OK", 2);
			res = SendAtCmd_P(nUart, "AT+COPS?\r", "OK",5); // operator status
			if(res != NULL){
				res = strstr_P(res, PSTR(",")); // format
				if(res != NULL){
					res = strstr_P(res+1, PSTR(",")); // operator
					if(res != NULL){
						res = strstr_P(res+1, PSTR(",")); // access technology selected
						if(res != NULL){
							if(res[1] == '0'){
								TXT_DBG("GSM/GPRS Access Technology Selected");
								E_AccessTechnology = E_2G;
							}else if(res[1] == '2'){
								TXT_DBG("UTRAN(3G) Access Technology Selected");
								E_AccessTechnology = E_3G;
							}else if(res[1] == '3'){
								TXT_DBG("GSM w/EGPRS Access Technology Selected");
								E_AccessTechnology = E_3G;
							}else if(res[1] == '4'){
								TXT_DBG("UTRAN w/HSDPA Access Technology Selected");
								E_AccessTechnology = E_3G;
							}else if(res[1] == '5'){
								TXT_DBG("UTRAN w/HSUPA Access Technology Selected");
								E_AccessTechnology = E_3G;
							}else if(res[1] == '6'){
								TXT_DBG("UTRAN w/HSDPA and HSUPA Access Technology Selected");
								E_AccessTechnology = E_3G;
							}else if(res[1] == '7'){
								TXT_DBG("E-UTRAN Access Technology Selected");
								E_AccessTechnology = E_3G;
							}
						}
					}
				}
			}
			SendAtCmd_P(nUart, "AT#MONI\r", "OK",5);
		}

		timer_sysDelay_sec(1);

		if(e_MODEL_MODEM == MODEL_MODEM_LE910_SVL){
			res = SendAtCmd_P(nUart, "AT+CGQREQ=3,0,0,3,0,0\r", "OK",2);
			res = SendAtCmd_P(nUart, "AT+CGEQREQ=3,4,0,0,0,0,2,0,\"0E0\",\"0E0\",3,0,0\r", "OK",2);
			res = SendAtCmd_P(nUart, "AT#SCFG=3,1,500,60,300,10\r", "OK",2);
		}

		// Telit suggests to setup AT+CGQREQ=1,0,0,3,0,0
		res = SendAtCmd_P(nUart, "AT+CGQREQ=1,0,0,3,0,0\r", "OK",2);
		// Telit suggests to setup AT+CGEQREQ=1,4,0,0,0,0,2,0,”0E0”,”0E0”,3,0,0(default setting	value)
		res = SendAtCmd_P(nUart, "AT+CGEQREQ=1,4,0,0,0,0,2,0,\"0E0\",\"0E0\",3,0,0\r", "OK",2);

		/*
		 * Before opening a connection we have to set the socket parameters with the new #SCFG command.
		 *
		 * Socket Configuration:  AT#SCFG=<connId>,<cid>,<pktSz>,<maxTo>,<connTo>,<txTo>
		 * pktSz 	= 500 bytes.
		 * maxTo 	= 60 seconds  - exchange timeout (or socket inactivity timeout); if there’s no data
		 * 							exchange within this timeout period the connection is closed.
		 * connTo 	= 30 seconds  - connection timeout; if we can’t establish a connection to the remote
		 * 							within this timeout period, an error is raised.
		 * txTo 	= 1 second    - data sending timeout; after this period data are sent also if they’re less
		 * 							than max packet size.
		 */
		res = SendAtCmd_P(nUart, "AT#SCFG=1,1,500,60,300,10\r", "OK",2);

		for(i=0; (i < 5) && xE910AccessTechnology(nUart); timer_sysDelay_sec(5), i++);
		if(i != 5){
			for(i=0; (i < 5) && (SendAtCmd_P(nUart, "AT#SGACT=1,1\r", "OK",35) == NULL); timer_sysDelay_sec(10), i++);
			if(i != 5){
				if(!debugON) TXT_DBG("1018,\"proxy.phytech.com\"");
				for(i=0; (i < 3) && (SendAtCmd_P(nUart, "AT#SD=1,0,1018,\"proxy.phytech.com\"\r", "CONNECT",30) == NULL); timer_sysDelay_sec(5), i++);
				//for(i=0; (i < 3) && (SendAtCmd_P(nUart, "AT#SD=1,0,1018,\"54.246.85.255\"\r", "CONNECT",30) == NULL); timer_sysDelay_sec(5), i++);
				if(i != 3){// test communication successful.
					timer_sysDelay_sec(5);
					if(SendAtCmd_P(nUart, "+++", "OK", 5) != NULL){
						res = SendAtCmd_P(nUart, "AT#SH=1\r", "OK", 5);
					}
					return 0;
				}
			}
		}
	}


	return 1;
}


uint8_t xE910AccessTechnology(uint8_t nUart)
{
	char *res,*pTmp;

	res = SendAtCmd_P(nUart, "AT+CGATT?\r", "+CGATT:",5);
	if(res != NULL){
		pTmp = strstr_P(res, PSTR(":"));
		if(pTmp != NULL){
			if(*(pTmp + 2) == '0'){
				if(SendAtCmd_P(nUart, "AT+CGATT=1\r", "OK",2) != NULL) return 0;
			}else return 0;
		}
	}

	return 1;
}

uint8_t xE910ApnUpdate(uint8_t nUart, const char strApnUpdate[])
{
	char *res;

	res = SendAtCmd_P(nUart, "AT+CGDCONT?\r","OK",2);
	if(res != NULL){// OK
		res = strstr_P(res,PSTR(","));
		if(res != NULL){// PDP_type
			res = strstr_P(res+1, PSTR(","));
			if(res != NULL){// APN,
				res = strstr(res+1,&strApnUpdate[0]);
				if(res != NULL){
					return 0;
				}
			}
		}
		return 1;
	}

	return 0;
}

