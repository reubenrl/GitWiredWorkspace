/*
 * modem.c
 *
 *  Created on: Dec 10, 2015
 *      Author: reuben
 */

#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

#include "bsp.h"
#include "timer.h"
#include "buffer.h"
#include "uart2.h"
#include "uartDbgPrint.h"
#include "modem.h"
#include "timer_sys.h"
#include "battery.h"
#include "leds.h"

#include "uart1_split.h"

#define __DEBUG_GSM_MODEM

#define MODEM_BAUDRATE	19200UL
#define MAX_BAUDRATE 8

volatile unsigned char modemPwrRetries;

static u32 aBaudrate[] = {115200,57600,38400,19200,9600,4800,2400,1200};

// display enable during the search network.
volatile uint8_t modemKeepAliveDisplayEna;


void modemInit(void)
{
	modemKeepAliveDisplayEna = 0;
	modemPwrOffProc(COM_GSM);
}



//#include "..//RTS23-VER1.0 Wireless/include/battery.h"
/*
 * return:
 * 		0	-	power monitor is on
 * 		1	-	power monitor is off
*/
uint8_t modemHwShDwnUnconditional(void)
{

#if defined(__DEBUG_GSM_MODEM)
	TXT_DBG("modem3GHwShDwnUnconditional");
#endif
	sbi(PORTC, GSM_SHDW_ENA); 	// start pulse low
	timer_sysDelay_msec(200);
	cbi(PORTC, GSM_SHDW_ENA);	// end pulse. released
	timer_sysDelay_sec(1);
	return(PINC & (1<<GSM_PWR_MON));
}
 /*
 * return:
 * 		0	- failed
 * 		1	- successful
*/
uint8_t modemPwrOnProc(void)
{
	uint32_t u32BattValue;
	uint8_t powerUp = 0;

	TXT_DBG("start modemPwrOnProc.");
	u32BattValue = batteryReadVoltageMeasure();
	if(u32BattValue > 3220u){
		if(PINC & (1<<GSM_PWR_MON)){// power monitor is off
			TXT_DBG("GSM Modem Power is OFF.");
			sbi(PORTC, GSM_IGNITION); 	// start ignition pulse low
			timer_sysDelay_sec(10);
			cbi(PORTC, GSM_IGNITION);	// end ignition pulse released
			timer_sysDelay_sec(1);
			if(PINC & (1<<GSM_PWR_MON)){// power monitor is off
				if(!modemHwShDwnUnconditional()){
					powerUp = 1;
				}
			}else powerUp = 1;
		}else powerUp = 1;
		TXT_DBG("GSM Modem Power ON.");

	}else{
#if defined(__DEBUG_GSM_MODEM)
		TXT_DBG("Protect modem 3G... Low battery.");
#endif
	}
	timer_sysDelay_sec(1);

	return powerUp;
}


void modemPwrOffProc(uint8_t nUart)
{

	TXT_DBG("start modemPwrOffProc.");
	if(!(PINC & (1<<GSM_PWR_MON))){// power monitor is on
		TXT_DBG("GSM Modem Power is ON.");
		if(SendAtCmd_P(nUart, "AT#SHDN\r", "OK", 5) == NULL){
			// off mode key procedure
			TXT_DBG("off mode key procedure");
			sbi(PORTC, GSM_IGNITION); 	// start ignition pulse low
			timer_sysDelay_sec(4);
			cbi(PORTC, GSM_IGNITION);	// end ignition pulse released
		}
		timer_sysSetTimeout_sec(15);
		while(!(PINC & (1<<GSM_PWR_MON)) && timer_sysGetTimeout_sec());
		if(!timer_sysGetTimeout_sec()){
			if(modemHwShDwnUnconditional()){

#if defined(__DEBUG_GSM_MODEM)
				TXT_DBG("GSM Modem Power OFF Serious Problem!!!");
#endif
				modemPwrOffProc(nUart); // infinite loop
			}
		}else{
			timer_sysDelay_sec(5);
		}
	}

	TXT_DBG("GSM Modem Power OFF.");
}



/*
 * nUart		: uart device
 * timeout_sec 	: wait for command response
 * return:
 *			0 - no data receive
 * 			1 - data receive
 */
uint8_t modemGetAtCmdResponse(uint8_t nUart, uint8_t timeout_sec)
{
	timer_sysSetTimeout_sec(timeout_sec);

	do{
		if (!uartReceiveBufferIsEmpty(nUart)){	// data in the uart
			timer_sysDelay_msec((200UL*10UL*1000UL)/19200UL); // aprox. 200 bytes for baudrate 19200
			//timer_sysDelay_msec(250);
			if (bufferAddToEnd(uartGetRxBuffer(nUart), '\0')){
				return 1;
			}
			return 0;
		}
	}while(timer_sysGetTimeout_sec());

	return 0;
}

char *modemGetResponseBuff(uint8_t nUart)
{
	unsigned short idx = uartGetRxBuffer(nUart)->dataindex;
	return  (char *)&uartGetRxBuffer(nUart)->dataptr[idx];
}


/*
 * char *modemChkAtCmdResponse(const char str_cmd_response[], unsigned char timeout_sec)
 * description:	Get data from uart and compare strings, if equal return data from uart, other return NULL
 *
 * str_cmd_response
 * timeout_sec
 */

char *modemChkAtCmdResponse(uint8_t nUart, const char str_cmd_response[], uint8_t timeout_sec)
{
	char *res;//, isData;
	//unsigned short int idx;

	//isData = modemGetAtCmdResponse(nUart, timeout_sec);
	if(str_cmd_response != NULL){
		if(modemGetAtCmdResponse(nUart, timeout_sec)){
			//idx = uartGetRxBuffer(nUart)->dataindex;
			//res = (char *)&uartGetRxBuffer(nUart)->dataptr[idx];
			res = modemGetResponseBuff(nUart);
			if(strstr( res , str_cmd_response) != NULL) return res;
		}
	}

	return NULL;
}
/*
 * In:
 * 	timeout_sec: 0 - not wait for response
 * return:
 * 	NULL 	- AT response fail
 * 	OTHER 	- AT response successful
 */
char *modemSendAtCmd(uint8_t nUart, const char str_cmd[], const char str_cmd_response[], uint8_t timeout_sec)
{
	char *res = NULL;

#ifdef __DEBUG_GSM_MODEM
	if(debugON)	UART_DBG((char*)str_cmd);
#endif

	uartFlushReceiveBuffer(nUart);
	uartSendBuffer(nUart, (char*)str_cmd, strlen(str_cmd));
	// add for transmitter only
	if(timeout_sec){
		res = modemChkAtCmdResponse(nUart, str_cmd_response, timeout_sec);
#ifdef __DEBUG_GSM_MODEM
		if(res != NULL){// Send AT Command Successful
			if(debugON) UART_DBG(res);
		}
#endif
	}

	return res;
}


char *modemSendAtCmd_P(uint8_t nUart, const char str_cmd[], const char str_cmd_response[], uint8_t timeout_sec)
{
	char cmd_response_buff[16];
	char cmd_buffer[64];

	if( (strlen_P(str_cmd) < 64) && strlen_P(str_cmd) ){/*sizeof(cmd_buffer)*/
		if(strcpy_P(&cmd_buffer[0], str_cmd) == NULL)		return NULL;
	}else
		return NULL;

	if((strlen_P(str_cmd_response) < 16) && strlen_P(str_cmd_response)){ /*sizeof(strCmdResp)*/
		if(strcpy_P(&cmd_response_buff[0], str_cmd_response) == NULL)	return NULL;
	}else
		return NULL;

	return modemSendAtCmd(nUart, cmd_buffer, cmd_response_buff, timeout_sec);
}

/*
 * return:
 * 		0 - failed
 * 		1 - found modem baudrate
 */
uint8_t modemSearchBaudrate(uint8_t nUart, uint32_t *modemBaudrateFound)
{
	char strTmp[48];
	uint32_t modemBaudrate;
	uint8_t i;

	for(i=0; i < MAX_BAUDRATE; i++){
		modemBaudrate = aBaudrate[i];
		uartSetBaudRate(nUart, modemBaudrate);
		timer_sysDelay_msec(100u);
		if(sprintf_P(&strTmp[0], PSTR("Searching:%lubsp"), modemBaudrate)){
			UART_DBG(strTmp);
			if(SendAtCmd_P(nUart,"AT\r", "OK", 2) != NULL){
				*modemBaudrateFound = modemBaudrate;
				(void)SendAtCmd_P(nUart,"ATE0\r", "OK", 2);
				return 1;
			}
		}
	}// end for

	return 0;
}

/*
 * return:
 * 		0 - failed
 * 		1 - set modem baudrate
 */
uint8_t modemSetBaudrate(uint8_t nUart, uint32_t nModemBaudrate)
{
	char strTmpBuf[32];
	uint32_t nBaudrate;

	if(modemSearchBaudrate(nUart, &nBaudrate)){
		if(sprintf_P(strTmpBuf, PSTR("AT+IPR=%lu;&W\r"),nModemBaudrate)){
			if(SendAtCmd(nUart, strTmpBuf, "OK", 5) != NULL){
				uartSetBaudRate(nUart, nModemBaudrate);
				timer_sysDelay_sec(5u);
				return 1;
			}
		}
	}
	return 0;
}


/*
 * Network Registration Report
 * return:
 * 			0 - not registered
 * 			1 - registered
 */
uint8_t modemNetworkRegistrationReport(uint8_t nUart, uint16_t  nIntervals_sec, uint8_t nRetries)
{
	char *res = NULL, c;
	uint8_t nRegStatus, n, cnt_denied = 0;

	if(!debugON) TXT_DBG("\r\nNetwork Registration Report.");

	for(n=0; n < nRetries; n++, timer_sysDelay_sec(nIntervals_sec)){
		if(!debugON){
			if(n & 0x01){
				c = '\\';
				uartSendByte(UART1, c);
			}
			else{
				c = '/';
				uartSendByte(UART1, c);
			}
		}

		if((res = SendAtCmd_P(nUart, "AT+CREG?\r","OK", 2)) == NULL) continue;
		else{
			nRegStatus = res[11];
			if(nRegStatus == '3'){
				cnt_denied++;
				// 3 time registration denied exit.
				if(cnt_denied == 3) return 0;
			}
			if((nRegStatus == '1') || (nRegStatus == '5')){

				return 1;
			}
		}
	}// end for

	return 0;
}

/*
 * GPRS Network Registration Status
 * return:
 * 			0 - GPRS not registered
 * 			1 - GPRS registered
 */
uint8_t modemGPRSNetworkRegistrationStatus(uint8_t nUart, uint16_t  nIntervals_sec, uint8_t nRetries)
{
	char *res = NULL;
	uint8_t nRegStatus, n, cnt_denied = 0;

	if(!debugON) TXT_DBG("GPRS Registration.");
	for(n=0; n < nRetries; n++, timer_sysDelay_sec(nIntervals_sec)){
		if((res = SendAtCmd_P(nUart, "AT+CGREG?\r", "OK", 2)) == NULL) continue;
		else{
			res = strstr_P(res, PSTR(","));
			if(res != NULL){
				nRegStatus = res[1];
				if(nRegStatus == '3'){
					cnt_denied++;
					// 3 time registration denied exit.
					if(cnt_denied == 3) return 0;
				}

				if((nRegStatus == '1') || (nRegStatus == '5')) return 1;
			}
		}
	}// for

	return 0;
}


uint8_t modemOperatorSelection(uint8_t nUart, uint8_t roamingSim)
{
	t_mccOperatorList	mccOperatorList;
	char strTmp[32];
	uint8_t idx, n, j;

	if(roamingSim){
		if(!debugON) modemKeepAliveDisplayEna = 1;
		for(j=0; (j < 20) && !modemOperatorDiscovery(nUart, &mccOperatorList); timer_sysDelay_sec(5), j++);// tries 20 times - all the operators in the list
		if(mccOperatorList.numOperatorsFound){
			n = mccOperatorList.numOperatorsFound;
			for(idx=0; idx< n; idx++){ // select manual operator.
				if(sprintf_P(strTmp, PSTR("AT+COPS=1,2,%s\r"), &mccOperatorList.strMobileCountryCode[idx][0])){
					if(SendAtCmd(nUart, strTmp, "OK", 5) != NULL){// operator is selected
						timer_sysDelay_msec(5);
						if(modemKeepAliveDisplayEna) modemKeepAliveDisplayEna = 0;

						if(modemNetworkRegistrationReport(nUart, 10, 10)){// operator registration each 10 seconds retries 5 times
							if(modemGPRSNetworkRegistrationStatus(nUart, 10,5)){// GPRS registered. can send data.
								return 1;	// operator + GPRS is selected & registered
							}else TXT_DBG("GPRS access denied.");
						}else TXT_DBG("Network access denied.");
					}
				}
			}// end for
		}
		if(modemKeepAliveDisplayEna) modemKeepAliveDisplayEna = 0;
	}else{
		if(modemNetworkRegistrationReport(nUart, 10, 10)){// operator registration each 10 seconds retries 10 times
			if(modemGPRSNetworkRegistrationStatus(nUart, 10,5)){// GPRS registered. can send data.
				return 1;	// operator + GPRS is selected & registered
			}else TXT_DBG("GPRS access denied.");
		}else TXT_DBG("Network access denied.");
	}

	return 0;
}

uint8_t modemOperatorDiscovery(uint8_t nUart, t_mccOperatorList *p_mccOperatorList)
{
	char strOperatorList[48];
	char strOperatorName[32];
	char strAccessTechnology[2];
	char *res ;
	char *pStart, *pEnd, *p_idx;
	uint8_t i,j, MobileCountryCodeLen;

	MobileCountryCodeLen = 0;
	p_mccOperatorList->numOperatorsFound = 0;

	res = SendAtCmd_P(nUart, "AT+COPS=?\r", "OK", 60);
	if(res != NULL){
		pStart = pEnd = NULL;
		for(p_idx = res; *p_idx; p_idx++){
			if(*p_idx == '('){
				pStart = p_idx + 1;
			}else if(*p_idx == ')'){
				pEnd = p_idx;
				j = (pEnd - pStart);
				for(i=0; i<j; i++) strOperatorList[i] = *pStart++;
				strOperatorList[i] = '\0';
				//UART_DBG(strOperatorList);
				////////   operator name
				pStart = strstr_P(strOperatorList,PSTR("\""));
				if(pStart != NULL){
					pStart += 1;
					pEnd = strstr_P(pStart,PSTR("\""));
					if(pEnd != NULL){
						j = pEnd - pStart;
						for(i=0; i<j; i++) strOperatorName[i] = pStart[i];
						strOperatorName[i] = '\0';
						if(debugON) UART_DBG(strOperatorName);
					}
				}
				////////   operator number
				pStart = pEnd + 1;
				pStart = strstr_P(pStart,PSTR("\""));
				if(pStart != NULL){
					pEnd = strstr_P(pStart + 1,PSTR("\""));
					if(pEnd != NULL){
						if(MobileCountryCodeLen < 8){
							pEnd += 1;
							j = pEnd - pStart;

							for(i=0; i<j; i++) p_mccOperatorList->strMobileCountryCode[MobileCountryCodeLen][i] = pStart[i];
							p_mccOperatorList->strMobileCountryCode[MobileCountryCodeLen][i] = '\0';

							//UART_DBG(&p_mccOperatorList->strMobileCountryCode[MobileCountryCodeLen][0]);
							MobileCountryCodeLen++;

							////////   Access Technology
							strAccessTechnology[0] = *(pEnd + 1);
							strAccessTechnology[1] = '\0';
							if(debugON) UART_DBG(&strAccessTechnology[0]);
						}
					}
				}

				pStart = pEnd = NULL;
			}
		}// end for
	}

	if(!MobileCountryCodeLen) return 0;

	p_mccOperatorList->numOperatorsFound = MobileCountryCodeLen;

	return 1;
}

volatile uint8_t wdt_cnt, wdt_flag;
ISR(WDT_vect){
	wdt_cnt++;
	if(wdt_cnt & 1)
		ledControl(LED1, LED_ON);
	else
		ledControl(LED1, LED_OFF);
}

void modemBypassUart(void)
{
	unsigned char c;


	uart1_splitPortSelect(UART1_SPLIT_DBG);
	WDTCSR |= (1<<WDIF);
	WDTCSR |= (1<<WDIE);

	if(modemPwrOnProc()){
		if(modemSetBaudrate(COM_GSM,19200u)){
			TXT_DBG("Debug Software...Ready.");
			wdt_enable(WDTO_500MS);
			while(1){
				cli();
				wdt_flag |= 1;
				sei();
				if(uartReceiveByte(COM_GSM, &c)){
					uartSendByte(COM_DBG,c);
				}
				cli();
				wdt_flag |= 2;
				sei();
				if(uartReceiveByte(COM_DBG, &c)){
					uartSendByte(COM_GSM,c);
				}

				if(wdt_flag == 3){
					wdt_reset();
					wdt_flag = 0;
				}

			}
		}
	}
}
