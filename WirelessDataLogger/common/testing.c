/*
 * testing.c
 *
 *  Created on: Mar 22, 2017
 *      Author: phytech
 */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>	// include AVR program memory support
#include "util/delay.h"
#include <avr/sleep.h>

#include "bsp.h"
#include "version.h"
#include "timer_sys.h"
#include "timer.h"
#include "uart2.h"
#include "a2d.h"
#include "vt100.h"			// include vt100 terminal support
#include "modem.h"
#include "xE910.h"
#include "rtc_s35390a.h"
#include "uartDbgPrint.h"
#include "uart1_split.h"
#include "wireless.h"
#include "battery.h"
#include "leds.h"
#include "epromAT24xx.h"
#include "nmea.h"

#define MODEM_BYPASS_UART_ENABLE
//#define VERIZON_TEST

#if defined(SHORT_MESSAGE_SERVICE_TEST)
#include "ShortMessageService.h"
#endif

#include "cmdline.h"		// include cmdline function library

#include "testing.h"

// global variables
volatile uint8_t run;

// functions
void goCmdline(void);
void exitFunction(void);
void helpFunction(void);
void rtc_test(void);
void eeprom_test(void);
void radio_uhf_test(void);
void gps_test(void);
void charger_test(void);
void gsm_test(void);
void sleep_test(void);
#if defined(SHORT_MESSAGE_SERVICE_TEST)
void short_message_service_test(void);
#endif
void cmdLineMain(void)
{
	uart1_splitPortSelect(UART1_SPLIT_DBG);
	// initialize vt100 terminal
	vt100Init(uartSendStr);
	// start command line
	goCmdline();
}


void goCmdline(void)
{
	char *pBuff;
	uint8_t c;

	// print initial message

	vt100ClearScreen();
	vt100SetCursorPos(1,0);

	if((pBuff = (char *)malloc(32)) != NULL){

		if(sprintf_P(pBuff, PSTR("Logger Wireless Testing."))) UART_DBG(pBuff);
		if(sprintf_P(pBuff, PSTR("WBRD200-VER2 Card."))) UART_DBG(pBuff);
		if(sprintf_P(pBuff, PSTR("PROGRAM VER-"__WBRD200_VERSION__)))  UART_DBG(pBuff);
		if(sprintf_P(pBuff, PSTR("DATE:"__DATE__ "  TIME:"__TIME__)))  UART_DBG(pBuff);
		free(pBuff);
	}
/*
	switch(E_RESETSOURCE){
	case POWER_RST:
		TXT_DBG("POWER_RST");
		break;
	case EXTERNAL_RST:
		TXT_DBG("EXTERNAL_RST");
		break;
	case BROWNOUT_RST:
		TXT_DBG("BROWNOUT_RST");
		break;
	case WATCHDOG_RST:
		TXT_DBG("WATCHDOG_RST");
		break;
	case JTAG_RST:
		TXT_DBG("JTAG_RST");
		break;
	}*/

	// initialize cmdline system
	cmdlineInit();

	// direct cmdline output to uart (serial port)
	cmdlineSetOutputFunc(uartSendByte);

	// add commands to the command database
	cmdlineAddCommand((unsigned char *)"q",	exitFunction);
	cmdlineAddCommand((unsigned char *)"h",	helpFunction);
	cmdlineAddCommand((unsigned char *)"1",	rtc_test);
	cmdlineAddCommand((unsigned char *)"2",	eeprom_test);
	cmdlineAddCommand((unsigned char *)"3",	charger_test);
	cmdlineAddCommand((unsigned char *)"4",	radio_uhf_test);
	cmdlineAddCommand((unsigned char *)"5",	gps_test);
	cmdlineAddCommand((unsigned char *)"6",	gsm_test);
	cmdlineAddCommand((unsigned char *)"7",	sleep_test);
#if defined(MODEM_BYPASS_UART_ENABLE)
	cmdlineAddCommand((unsigned char *)"8",	modemBypassUart);
#endif

#if defined(SHORT_MESSAGE_SERVICE_TEST)
	cmdlineAddCommand((unsigned char *)"9",	short_message_service_test);
#endif
	// send a CR to cmdline input to stimulate a prompt
	cmdlineInputFunc('\r');
#if defined (MODEM_BYPASS_UART_ENABLE)  &&  defined(VERIZON_TEST)
	cmdlineInputFunc('\x38');
	cmdlineInputFunc('\x0D');
#endif
	// set state to run
	run = TRUE;

	// main loop
	while(run){
		// pass characters received on the uart (serial port) into the cmdline processor
		while(uartReceiveByte(COM_DBG, &c)) cmdlineInputFunc(c);
		// run the cmdline execution functions
		cmdlineMainLoop();
	}

	TXT_DBG("Exited program!");
}

void exitFunction(void)
{
	// to exit, we set Run to FALSE
	run = FALSE;
}

void helpFunction(void)
{
	vt100ClearScreen();
	vt100SetCursorPos(0,0);

	TXT_DBG("Available commands are:");
	TXT_DBG("h - displays available commands");
	TXT_DBG("q - exit");
	TXT_DBG("1 - rtc_test");
	TXT_DBG("2 - eeprom_test");
	TXT_DBG("3 - charger_test");
	TXT_DBG("4 - radio_uhf_test");
	TXT_DBG("5 - gps_test");
	TXT_DBG("6 - gsm_test");
	TXT_DBG("7 - sleep_test");
#if defined(MODEM_BYPASS_UART_ENABLE)
	TXT_DBG("8 - modem direct connection");
#endif
#if defined(SHORT_MESSAGE_SERVICE_TEST)
	TXT_DBG("9 - short_message_service_test");
#endif
}

// Real Time Clock S35390 Testing.
void rtc_test(void)
{
	char strTmp[64];
	char *ptrEnd;
	uint8_t argStrLen,i,dateValidation=0,timeValidation=0;

	/* parsing */
	char *ptrArgStr = (char *)cmdlineGetArgStr(1);
	if(ptrArgStr != NULL){
		if(*ptrArgStr == '9') debugON = 1;
	}

/*
 * Update time & date
 * format: DD/MM/YY HH:MM:SS
 */
	/* parsing update date: DD/MM/YY */
	ptrArgStr = (char *)cmdlineGetArgStr(1);
	if(ptrArgStr != NULL){
		ptrEnd = strstr_P(ptrArgStr,PSTR("/" ));
		if(ptrEnd != NULL){
			argStrLen = ptrEnd - ptrArgStr;
			for(i=0; i < argStrLen; i++) strTmp[i] =  *ptrArgStr++;
			strTmp[i] = '\0';
			DAY = u8Hex2Bcd(atoi(strTmp));

			ptrArgStr++;
			ptrEnd = strstr_P(ptrArgStr,PSTR("/" ));
			if(ptrEnd != NULL){
				argStrLen = ptrEnd - ptrArgStr;
				for(i=0; i < argStrLen; i++) strTmp[i] =  *ptrArgStr++;
				strTmp[i] = '\0';
				MONTH = u8Hex2Bcd(atoi(strTmp));

				ptrArgStr++;
				ptrEnd = strstr_P(ptrArgStr,PSTR(" " ));
				if(ptrEnd != NULL){
					argStrLen = ptrEnd - ptrArgStr;
					for(i=0; i < argStrLen; i++) strTmp[i] =  *ptrArgStr++;
					strTmp[i] = '\0';
					YEAR = u8Hex2Bcd(atoi(strTmp));

					dateValidation = 1;
				}
			}
		}
	}

	/* parsing update time: HH:MM:SS */
	ptrArgStr = (char *)cmdlineGetArgStr(2);
	if(ptrArgStr != NULL){
		ptrEnd = strstr_P(ptrArgStr,PSTR(":"));
		if(ptrEnd != NULL){
			argStrLen = ptrEnd - ptrArgStr;
			for(i=0; i < argStrLen; i++) strTmp[i] =  *ptrArgStr++;
			strTmp[i] = '\0';
			HOUR = u8Hex2Bcd(atoi(strTmp));

			ptrArgStr++;
			ptrEnd = strstr_P(ptrArgStr,PSTR(":" ));
			if(ptrEnd != NULL){
				argStrLen = ptrEnd - ptrArgStr;
				for(i=0; i < argStrLen; i++) strTmp[i] =  *ptrArgStr++;
				strTmp[i] = '\0';
				MINUTE = u8Hex2Bcd(atoi(strTmp));

				ptrArgStr++;
				for(i=0; *ptrArgStr; i++) strTmp[i] =  *ptrArgStr++;
				strTmp[i] = '\0';
				SECOND = u8Hex2Bcd(atoi(strTmp));

				timeValidation = 1;
			}
		}
	}

	if(dateValidation && timeValidation) rtcSetData_1(); /* Update time & date */
	else TXT_DBG("Format: DD/MM/YY HH:MM:SS - Updated not done.");

	// Display RTC time
	if(rtcGetUpdate(strTmp)) uartSendBuffer(COM_DBG, strTmp, strlen(strTmp));

	// Test RTC crystal oscillator
	TXT_DBG("Start testing RTC oscillator...");
	if(rtcCrystalFrequencyTest()){
		vt100SetAttr(32);
		TXT_DBG("RTC oscillator successful.");
		vt100SetAttr(0);
	}else{
		vt100SetAttr(31);
		vt100SetAttr(5);
		TXT_DBG("RTC oscillator failed.....");
		vt100SetAttr(0);
	}

	debugON = 0;
}


// Eeprom 24LC64 Testing.
void eeprom_test(void)
{
	uint8_t eepReadAllEnable = false, eepEraseAllEnable = false;

	/* parsing */
	char *ptrArgStr = (char *)cmdlineGetArgStr(1);

	if(ptrArgStr != NULL){
		if(*ptrArgStr == '1'){
			debugON = 1;
		}else if(*ptrArgStr == '9'){
			eepReadAllEnable = true;
		}else if(*ptrArgStr == '8'){
			eepEraseAllEnable = true;
		}
	}

	if(!(eepEraseAllEnable || eepReadAllEnable)){
		// Test Eeprom Atmel: 24LC64
		eeAt24xxInt();
		TXT_DBG("Start Testing EEprom: 24LC64/256");
		if(!eeTest24xx()){
			vt100SetAttr(32);
			TXT_DBG("EEprom is successful.");
		}else{
			vt100SetAttr(31);
			vt100SetAttr(5);
			TXT_DBG("EEprom is failed.");
		}
	}else{
		if(eepReadAllEnable){
			eepReadAll();
		}
		if(eepEraseAllEnable){
			if(eepEraseAll()) TXT_DBG("eepEraseAll...fail");
		}
	}

	vt100SetAttr(0);
	debugON = 0;
}


// Modem Telit Setting.
void gsm_test(void)
{
	uint8_t isCommunicationSuccessful = 0, isModemDetect = 0;

	/* parsing */
	char *ptrArgStr = (char *)cmdlineGetArgStr(1);
	if(ptrArgStr != NULL){
		if(*ptrArgStr == '1') debugON = 1;
	}

	ledControl(LED1, LED_OFF);
	ledControl(LED2, LED_OFF);
	vt100SetAttr(0);

	TXT_DBG("start modem gsm test.");
	// Modem power ON
	TXT_DBG("Ignition modem...");
	if(modemPwrOnProc()){
		ledControl(LED2, 2);
		ledControl(LED3, 16);
		TXT_DBG("Set modem baudrate 19200bsp.");
		if(modemSetBaudrate(COM_GSM, 19200UL)){
			ledControl(LED3, LED_OFF);
			if(!debugON) TXT_DBG("OK...");
			if(SendAtCmd_P(COM_GSM, "AT\r", "OK", 2) != NULL){
				isModemDetect = 1;	// modem detected.
				if(!xE910tSetParm(COM_GSM)){
					TXT_DBG("Start Test Communication.");
					if(!xE910CommunicationTest(COM_GSM)){
						vt100SetAttr(32);
						TXT_DBG("Test Communication Successful...");
						TXT_DBG("Set modem Successful...");
						isCommunicationSuccessful = 1;	// communication successful.
						vt100SetAttr(0);
					}
				}
			}
		}
	}


	if(!isCommunicationSuccessful){
		vt100SetAttr(31);
		vt100SetAttr(5);
		TXT_DBG("modem gsm test failed.");
	}
	vt100SetAttr(0);
	modemPwrOffProc(COM_GSM);

	debugON = 0;
}


/*
 *  ShutOffADC      shut down the ADC and prepare for power reduction
 **/
void  ShutOffADC(void)
{
    ACSR = (1<<ACD);                        // disable A/D comparator
    ADCSRA = (0<<ADEN);                     // disable A/D converter
    DIDR0 = 0x3F;                           // disable all A/D inputs (ADC0-ADC5)
    DIDR1 = 0x03;                           // disable AIN0 and AIN1
}

// RTS20 Sleep Testing.
void sleep_test(void)
{
	TXT_DBG("Starting Sleep Test.");
	TXT_DBG("Sleep Current < 50uA.");

	// sleep configuration register Status2
	rtcSleepConfig();

	//_delay_ms(500);
	timer_sysDelay_msec(500u);
	UCSR0B &= ~(BV(RXCIE)|BV(TXCIE)|BV(RXEN)|BV(TXEN));
	UCSR1B &= ~(BV(RXCIE)|BV(TXCIE)|BV(RXEN)|BV(TXEN));
	// disable TWI (two-wire interface)
	cbi(TWCR, TWEN);
	ShutOffADC();
	EIFR = 0;
	EIMSK = 0;
	PCICR = 0x00;
	PCIFR = 0;

	//_delay_ms(100);
	timer_sysDelay_msec(100u);
	// – Power Reduction Register
	PRR = 0xFF;
	// Bit 7 - PRTWI: 		Logic one to this bit shuts down the TWI
	// Bit 6 - PRTIM2: 		Logic one to this bit shuts down the Timer/Counter2 module
	// Bit 5 - PRTIM0: 		Logic one to this bit shuts down the Timer/Counter0 module
	// Bit 4 - PRUSART1: 	Logic one to this bit shuts down the USART1
	// Bit 3 - PRTIM1: 		Logic one to this bit shuts down the Timer/Counter1 module
	// Bit 2 - PRSPI: 		Logic one to this bit shuts down the Serial Peripheral Interface
	// Bit 1 - PRUSART0: 	Logic one to this bit shuts down the USART0
	// Bit 0 - PRADC: 		Logic one to this bit shuts down the ADC
	cli();

	BSP_SLEEP_CONFIG();

	_delay_ms(500);
	/// Input/Output Ports initialization for sleep

   while(1){
	   set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	   sleep_enable();
	   sleep_cpu();
	   sleep_disable();
	   PORTA |= 1<<PA5;
	   _delay_ms(250);
   }
}

void radio_uhf_test(void)
{
	uint8_t strTmp[32];
	uint8_t lenRxBuffer;

	TXT_DBG("start radio UHF test.");
	ledControl(LED3, LED_OFF); ledControl(LED2, LED_OFF); ledControl(LED1, LED_OFF);
	sbi(PORTD, RADIO_PWR_ENA); // radio UHF Power ON

	// test wireless
	uart1_splitPortSelect(UART1_SPLIT_RADIO_UHF);
	wirelessInit();
	while(1){
		ledControl(LED1, LED_ON);
		cbi(PORTD, RADIO_CTS);
		lenRxBuffer = wirelessPacketRx(COM_UHF, strTmp, 32, 2);
		if(lenRxBuffer){
			ledControl(LED2, LED_ON);
			wirelessPacketTx(COM_UHF);
			strTmp[lenRxBuffer] = '\0';
			UART_DBG((char *)strTmp);
			ledControl(LED2, LED_OFF);
		}
		sbi(PORTD, RADIO_CTS);

		ledControl(LED1, LED_OFF);

		timer_sysDelay_sec(10UL);

		if(rtcGetUpdate((char *)strTmp)) UART_DBG((char *)strTmp);
		(void)batteryVoltage();
		timer_sysDelay_sec(1UL);
	}
}


void gps_test(void)
{
	uint8_t gpsRxPacketFoud = 0;

	TXT_DBG("\r\nWelcome to GPS Test!");
	TXT_DBG("gps power ON");
	// gps power on
	sbi(PORTB, GPS_PWR_ENA);
	timer_sysDelay_sec(5u);
	sbi(PORTB, GPS_IGNITION);
	timer_sysDelay_msec(500u);
	cbi(PORTB, GPS_IGNITION);

	uart1_splitPortSelect(UART1_SPLIT_GPS);

	// begin gps packet processing loop
	while(1){

		timer_sysSetTimeout_sec(5u);
		uartFlushReceiveBuffer(COM_GPS);
		do{
			if(!uartReceiveBufferIsEmpty(COM_GPS)){
				timer_sysDelay_msec(80u);
				if(nmeaProcess(uartGetRxBuffer(COM_GPS))){
					gpsRxPacketFoud = 1;
					break;
				}
			}
		}while(timer_sysGetTimeout_sec());

		if(gpsRxPacketFoud){
			gpsRxPacketFoud = 0;
			UART_DBG((char *)nmeaGetPacketBuffer());
		}
	}
}

void charger_test(void)
{
	TXT_DBG("start charger_test.");
	batteryReadChargerMeasure();
	(void)batteryVoltage();

}

#if defined(SHORT_MESSAGE_SERVICE_TEST)
/*
 * ********  functions for test  ****************
 */

/*
 * Short Message Service String Format
 * #PHY#cmd,value#
 */

void short_message_service_test(void)
{
	t_ShortMessageService	*pSms;
	uint8_t fail = 1;
	char *res;
	char strTmp[48];

	/* parsing */
	char *ptrArgStr = (char *)cmdlineGetArgStr(1);
	if(ptrArgStr != NULL){
		if(*ptrArgStr == '1') debugON = 1;
	}

	vt100SetAttr(0);

	TXT_DBG("Ignition modem...");
	// Modem power ON
	if(!modemPwrOnProc()){
		if(modemNetworkRegistrationReport(COM_GSM, 10, 10)){// operator registration each 10 seconds retries 10 times

				if(!debugON){
					debugON = 1;
					res = SendAtCmd_P(COM_GSM, "AT+CSQ\r", "OK", 2);
					res = SendAtCmd_P(COM_GSM, "AT+COPS?\r", "OK",5); // operator status
					res = SendAtCmd_P(COM_GSM, "AT#MONI\r", "OK",5);
				}

				pSms = smsGetMessageList(COM_GSM, 5);
				if(pSms != NULL){
					if(!smsCmdProc(pSms)) fail = 0;
					if(sprintf_P(strTmp,PSTR("AT+CMGS=%s\r"), pSms->strSenderNumber)){
						res = SendAtCmd(COM_GSM,strTmp, ">", 5);
						if(res != NULL){
							res = SendAtCmd(COM_GSM,"hello\x1A", "+CMGS: ", 15);
						}
					}
					//(void)smsDeleteMessage(UART_MODEM_3G, pSms);
				}
			}


		//res = SendAtCmd_P(UART_MODEM_3G, "AT+COPS=0\r","OK", 2);
		//modemDelay_sec(5);
		timer_sysDelay_sec(5u);


		if(debugON){
			if(fail)
				TXT_DBG("SMS test fail");
			else
				TXT_DBG("SMS test successful");
		}


	}


	debugON = 0;

}
#endif
