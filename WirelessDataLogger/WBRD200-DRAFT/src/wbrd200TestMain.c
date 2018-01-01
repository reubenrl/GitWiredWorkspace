/*
 * wbrd200TestMain.c
 *
 *  Created on: Mar 6, 2017
 *      Author: phytech
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "bsp.h"
#include "timer.h"
#include "leds.h"
#include "uartDbgPrint.h"
#include "a2d.h"
#include "uart2.h"
#include "rtc_s35390a.h"

#include "timer_sys.h"
#include "testing.h"
#include "modem.h"

//#define MODEM_BYPASS_UART_ENABLE
void resetGetStatus(t_eResetSource *p_eResetSource);
void resetSourceDspl(t_eResetSource *p_eResetSource);

int main(void)
{
	t_eResetSource eResetSource;

	resetGetStatus(&eResetSource);

	BSP_INIT();

	ledIni();
	a2dInit();
	uartInit();
	uartSetBaudRate(COM_GSM, 19200);
	uartSetBaudRate(COM_DBG, 38400);
	timer_sysInit();;
	//timer0Init();
	//timer0SetPrescaler(TIMER_CLK_DIV1024); // 8 bit overflow = 14.0625Hz = 71.11 msec
	//timerAttach(TIMER0OVERFLOW_INT, ledTasking);
	timer0CompareAInit(ledTasking);


	resetSourceDspl(&eResetSource);
	// Real Time Clock S35390 Testing.
	if(rtcInit())	TXT_DBG("Power on detected.");
	ledControl(LED3,LED_TIME_PERIOD_1600MS);
/*
#if defined(MODEM_BYPASS_UART_ENABLE)
	modemBypassUart();
#endif
*/
	cmdLineMain();

	return 0;
}

void resetGetStatus(t_eResetSource *p_eResetSource)
{
	uint8_t restStatus;

	restStatus = MCUSR;
	MCUSR = 0;

	if(restStatus & (1<<POWER_RST)){
		*p_eResetSource = POWER_RST;
	}else if(restStatus & (1<<EXTERNAL_RST)){
		*p_eResetSource = EXTERNAL_RST;
	}else if(restStatus & (1<<BROWNOUT_RST)){
		*p_eResetSource = BROWNOUT_RST;
	}else if(restStatus & (1<<WATCHDOG_RST)){
		*p_eResetSource = WATCHDOG_RST;
	}else if(restStatus & (1<<JTAG_RST)){
		*p_eResetSource = JTAG_RST;
	}

}

void resetSourceDspl(t_eResetSource *p_eResetSource)
{
	if(*p_eResetSource == POWER_RST) TXT_DBG("POWER_RST");
	else if(*p_eResetSource == EXTERNAL_RST) TXT_DBG("EXTERNAL_RST");
	else if(*p_eResetSource == BROWNOUT_RST) TXT_DBG("BROWNOUT_RST");
	else if(*p_eResetSource == WATCHDOG_RST) TXT_DBG("WATCHDOG_RST");
	else if(*p_eResetSource == JTAG_RST) TXT_DBG("JTAG_RST");
}

