/*
 * uart1_split.c
 *
 *  Created on: Mar 6, 2017
 *      Author: phytech
 */

#include "bsp.h"
#include "uart1_split.h"


uint8_t uart1_splitPortSelect(uint8_t nSelect)
{
	uint8_t uartDbgPort, uartDbgPortSaved;

	uartDbgPortSaved = PORTD;

	uartDbgPort = uartDbgPortSaved;
	uartDbgPort &= ~(UART1_SPLIT_BITS << UART1_SPLIT_BASE_PIN) ;
	uartDbgPort |= (nSelect<<UART1_SPLIT_BASE_PIN);
	PORTD = uartDbgPort;

	return uartDbgPortSaved;
}

