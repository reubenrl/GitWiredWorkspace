/*
 * uartDbgPrint.c
 *
 *  Created on: Jun 25, 2013
 *      Author: reuben
 */

#include <stdio.h>
#include <string.h>
//#include <util/delay.h>

#include "bsp.h"
#include "uart2.h"
#include "timer_sys.h"
#include "uartDbgPrint.h"
#include "uart1_split.h"


#define STR_DBG_TEXT_LEN	80	//64
#define COM_BAUDRATE_DBG	38400UL

/*

void printDbg(const char* format, ...)
{
    char       msg[80];

    va_list    args;
    va_start(args, format);
    vsprintf(msg, format, args); // do check return value

    va_end(args);
#ifdef PRINT_DBG_CRLF
    if(strcat(msg, pCRLF) != NULL){
#endif
    if(!uartSendBuffer(COM_DBG, msg, strlen(msg))){
    	while(1);
    }
#ifdef PRINT_DBG_CRLF
    }
#endif
}
*/

void DbgPrintText_P(const char str[])
{
	char strDbgText[STR_DBG_TEXT_LEN];
	int len;
	uint8_t uartSplitPort;

	len = strlen_P(str);
	if(len < (STR_DBG_TEXT_LEN - 2)){
		if(strcpy_P(&strDbgText[0], str) != NULL){
			if(strcat_P(strDbgText, PSTR("\r\n")) != NULL){
				uartSplitPort = uart1_splitPortSelect(UART1_SPLIT_DBG);
				len = strlen(strDbgText);
				uartSendBuffer(COM_DBG, strDbgText, len);
				timer_sysDelay_msec(((len*10UL*1000UL)/COM_BAUDRATE_DBG) + 5UL);
				outb(PORTD, uartSplitPort);
			}
		}
	}

}

void DbgPrintText(char str[])
{
	char strDbgText[STR_DBG_TEXT_LEN];
	int len;
	uint8_t uartSplitPort;

	len = strlen(str);

	if(len < (STR_DBG_TEXT_LEN - 2)){
		if(strcpy(&strDbgText[0], str) != NULL){
			if(strcat_P(strDbgText, PSTR("\r\n")) != NULL){
				uartSplitPort = uart1_splitPortSelect(UART1_SPLIT_DBG);
				len = strlen(strDbgText);
				uartSendBuffer(COM_DBG, strDbgText, len);
				timer_sysDelay_msec(((len*10UL*1000UL)/COM_BAUDRATE_DBG) + 5UL);
				outb(PORTD, uartSplitPort);
			}
		}
	}

}

void DbgPrintBuff(char *pBuff, uint8_t buffLen)
{
	uint8_t uartSplitPort;

	if(buffLen < (STR_DBG_TEXT_LEN)){
		uartSplitPort = uart1_splitPortSelect(UART1_SPLIT_DBG);
		uartSendBuffer(COM_DBG, pBuff, buffLen);
		timer_sysDelay_msec(((buffLen*10UL*1000UL)/COM_BAUDRATE_DBG) + 5UL);
		outb(PORTD, uartSplitPort);
	}
}

static char strAsciiOutput[5];

char *hex2acii(uint8_t hexIn)
{
	if(sprintf(strAsciiOutput, "%02X ", hexIn)) return strAsciiOutput;
	else return NULL;
}

char hexDigit(unsigned n)
{
    if (n < 10) {
        return n + '0';
    } else {
        return (n - 10) + 'A';
    }
}

void charToHex(char c, char hex[3])
{
    hex[0] = hexDigit(c / 0x10);
    hex[1] = hexDigit(c % 0x10);
    hex[2] = '\0';
}

void DbgHex2AsciiPrint(uint8_t strHexIn[], uint8_t hexInLen)
{
	char *pHexIn;

	for(uint8_t i=0; i<hexInLen; i++){
		pHexIn = hex2acii(strHexIn[i]);
		if(pHexIn != NULL){
			DbgPrintText(strAsciiOutput);

		}
	}

}
/*

void DbgHex2AsciiPrintBuffer(uint8_t strHexIn[], uint8_t hexInLen)
{
	char *pHexValue;

	if(hexInLen < (STR_DBG_TEXT_LEN)){
		for(uint8_t i=0; i<hexInLen; i++){
			pHexValue = hex2acii(strHexIn[i]);
			if(pHexValue != NULL){
				//DbgPrintText(strAsciiOutput);
				uartSendBuffer(COM_DBG, pHexValue, 2);
			}
		}
	}
}

*/


