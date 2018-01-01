/*
 * modem.h
 *
 *  Created on: Dec 10, 2015
 *      Author: reuben
 */

#ifndef MODEM_H_
#define MODEM_H_


#include "global.h"

#define AT_RESP_OK					"OK"
#define modem_timeout_sec(n)		n*14	//1000/(1/((3686400/1024)/256))
#define UART_DEBUG_BAUDRATE			19200UL
#define UART_MODEM_SAT_BAUDRATE		19200UL
//#define UART_MODEM_3G				0
//#define UART_DEBUG					1
//#define UART_MODEM_SAT				0
//#define UART_WIRELESS				1
#define PWR_MODEM_ON				1
#define PWR_MODEM_OFF				0

#define MODEM_AT_OFFMODE			1
#define MODEM_KEY_OFFMODE			0



typedef struct{
	uint8_t numOperatorsFound;
	char strMobileCountryCode[8][9];
}t_mccOperatorList;


typedef enum{
	E_2G,
	E_3G
}t_E_AccessTechnology;

typedef enum{
	UNKNOW_GENERATION,
	GENERATION_2,
	GENERATION_3,
	GENERATION_4
}t_e_MODEN_GENERATION;

typedef enum{
	MODEL_MODEM_UNKNOW=0,
	MODEL_MODEM_LE910_SVL
}t_e_MODEL_MODEM;

#define APN_LOCAL		"\"internet\""
#define APN_MOVISTAR	"\"internetm2m.air.com\""
#define APN_VERIZON		"\"jtm2m\""

#define APN_CNFG			APN_MOVISTAR

void modemInit(void);

void modemPwrOffProc(uint8_t nUart);
uint8_t modemPwrOnProc(void);
//void modemIgnitionPowerSwitch(uint8_t nStatus);

char *modemChkAtCmdResponse(uint8_t nUart, const char str_cmd_response[], uint8_t timeout_sec);
char * modemGetResponseBuff(uint8_t nUart);
char *modemSendAtCmd(uint8_t nUart, const char str_cmd[], const char str_cmd_response[], uint8_t timeout_sec);
uint8_t modemGetAtCmdResponse(uint8_t nUart, uint8_t timeout_sec);

uint8_t modemSearchBaudrate(uint8_t nUart, uint32_t *modemBaudrateFound);
uint8_t modemSetBaudrate(uint8_t nUart, uint32_t nModemBaudrate);
//void modemDelay_msec(unsigned int delay_msec);
//void modemDelay_sec(unsigned int delay_sec);

char *modemSendAtCmd_P(uint8_t nUart, const char str_cmd[], const char str_cmd_response[], uint8_t timeout_sec);
#define SendAtCmd_P(n,c,r,t) 			modemSendAtCmd_P(n, PSTR(c), PSTR(r), t)
#define SendAtCmd(n,c,r,t)				modemSendAtCmd(n,c,r,t)


uint8_t modemOperatorDiscovery(uint8_t nUart, t_mccOperatorList *p_mccOperatorList);
uint8_t modemOperatorSelection(uint8_t nUart, uint8_t roamingSim);

void modemBypassUart(void);

#endif /* MODEM_H_ */
