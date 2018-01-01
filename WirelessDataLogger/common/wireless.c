/*
 * wireless.c
 *
 *  Created on: Mar 15, 2017
 *      Author: phytech
 */

#include "bsp.h"

#include "timer.h"
#include "uart2.h"
#include "uartDbgPrint.h"
#include "timer_sys.h"
#include "wireless.h"

void wirelessInit(void)
{

	uartSetBaudRate(COM_UHF, 38400);
}

/* nUart 		- input
 * strOutput 	- output
 * strOutputLen - input
 * timeout_sec	- input
 * return:
 * 		0 		- no rx buffer received
 * 		Other 	- rx buffer received
 */
uint8_t wirelessPacketRx(uint8_t nUart, uint8_t strOutput[], uint8_t strOutputLen, uint8_t timeout_sec)
{
	uint8_t c;
	uint8_t idx;

	timer_sysSetTimeout_sec(timeout_sec);
	uartFlushReceiveBuffer(nUart);
	idx = 0;

	do{
		if(!uartReceiveBufferIsEmpty(nUart)){
			do{
				if(uartReceiveByte(nUart, &c)){
					if(idx < strOutputLen) strOutput[idx++] = c;
					else
						return idx;

					timer_sysSetTimeout_msec(100u);
				}
			}while(timer_sysGetTimeout_msec());

			return idx;
		}
	}while(timer_sysGetTimeout_sec());

	return idx;
}

/*

uint16_t logger_interfaceGetBuffer(uint8_t strIn[], uint16_t nLenBuff, uint8_t nTimeout_sec)
{
	uint16_t idx;
	uint8_t c;

	logger_interfaceInit();
	uartFlushReceiveBuffer(UART_LOGGER);
	uartRxTimeout_sec = nTimeout_sec;
	idx = 0;
	do{
		if(!uartReceiveBufferIsEmpty(UART_LOGGER)){
			//uartRxTimeout_msec = 250u;
			//uartRxTimeout_msec = 200u;
			//while(uartRxTimeout_msec);
			do{
				if(uartReceiveByte(UART_LOGGER, &c)){
					if(idx <= nLenBuff ){
						strIn[idx++] = c;
					}else return idx;

					uartRxTimeout_msec = 100u;
				}//else return idx;
			}while(uartRxTimeout_msec && uartRxTimeout_sec);

			return idx;
		}
	}while(uartRxTimeout_sec);

	return 0;
}
*/

void wirelessPacketTx(uint8_t nUart)
{
	char c = 'U';

	uartSendByte(nUart, c);
}
