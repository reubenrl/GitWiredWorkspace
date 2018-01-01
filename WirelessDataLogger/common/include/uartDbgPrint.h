/*
 * uartDbgPrint.h
 *
 *  Created on: Jun 25, 2013
 *      Author: reuben
 */

#ifndef UARTDBGPRINT_H_
#define UARTDBGPRINT_H_

#include <avr/pgmspace.h>

void DbgPrintText(char str[]);
void DbgPrintBuff(char *pBuff, uint8_t buffLen);
void DbgPrintText_P(const char str[]);
void DbgHex2AsciiPrint(uint8_t strHexIn[], uint8_t hexInLen);
/* uart print from program flash data */
#define TXT_DBG(t) 	DbgPrintText_P(PSTR(t))
/* uart print from program flash data */
#define UART_DBG_P(t)	TXT_DBG(t)
/* uart print from ram data */
#define UART_DBG(t) 	DbgPrintText(t)
/* uart print hex to ascii values */
#define UART_DGB_HEX2ASCII(str,n)	 DbgHex2AsciiPrint(str, n);

char *hex2acii(uint8_t hexIn);

#endif /* UARTDBGPRINT_H_ */
