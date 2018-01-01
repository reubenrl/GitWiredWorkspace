/*
 * ShortMessageService.c
 *
 *  Created on: Jul 28, 2016
 *      Author: phytech
 */

#include "bsp.h"
#if defined(SHORT_MESSAGE_SERVICE_TEST)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "uart2.h"
#include "uartDbgPrint.h"
#include "modem.h"
#include "ShortMessageService.h"

/*
 * Message format:
 * #PHY#DATAn#
 * -------------------
 * |	DATAn		 |
 * |-----------------|
 * | CMD | , | VALUE |
 * -------------------
 * CMD $ VALUE data type: ASCII String.
 * CMD:
 * 		"apn" 	- Access Point Network; The name of a gateway between a mobile network and another computer network.
 * 		"url" 	- Uniform Resource Locator; names and addresses that refer to objects on the World Wide Web.
 * 		"port" 	- Server "listens to" or expects to receive from a Web client.
 * 		"mcc" 	- Mobile Country Code
 * 		"mnc"	- Mobile Network Code
 */

static t_ShortMessageService	SMS;
static t_smsCmd 	smsCmd;

/*
 * return:
 * 		NOT NULL	- successful
 * 		NULL		- fail
 */
char * smsInit(uint8_t nUart)
{
	char *res;
	/*
	 * Message Format - +CMGF
	 * <mode>
	 * 			0 - PDU mode, as defined in GSM 3.40 and GSM 3.41 (factory default)
	 * 			1 - text mode
	 */
	res = SendAtCmd_P(nUart,"AT+CMGF=1\r", "OK", 2);

	return res;
}

/*
 * return:
 * 		0 - successful
 * 		1 - fail
 */
uint8_t smsSenderNumParsing(char strInput[], t_ShortMessageService *pSMS)
{/* +CMGL: 1,"REC READ","1511",,"14/03/14,13:31:57-28" */
	char *pBegin, *pEnd;
	uint8_t senderNumLen, i;

	pBegin = strstr_P(strInput, PSTR(","));
	if(pBegin != NULL){// first
		pBegin += 1;
		pBegin = strstr_P(pBegin, PSTR(","));
		if(pBegin != NULL){// second
			pBegin += 1;
			pEnd = strstr_P(pBegin, PSTR(","));
			if(pEnd != NULL){// third
				senderNumLen = pEnd - pBegin;
				if((senderNumLen) < MSM_SNDR_NUM_BUF_LEN){
					for(i=0;i<senderNumLen;i++)	pSMS->strSenderNumber[i] = *pBegin++;
					pSMS->strSenderNumber[i] = '\0';
					return 0;
				}
			}
		}

	}

	return 1;
}

/*
 * Get list of unread messages
 * List Messages - +CMGL
 * 0 - new message
 * 1 - read message
 * 2 - stored message not yet sent
 * 3 - stored message already sent
 * 4 - all messages.
 *
 * +CMGL: <index>,<stat>,<alpha>,<length><CR><LF><pdu>[<CR><LF>
 */

/*
 * Read Short Message Service List: +CMGL
 * return:
 * 		NOT NULL 	- successful
 * 		NULL 		- fail
 */

t_ShortMessageService * smsGetMessageList(uint8_t nUart, uint8_t timeoutSec)
{
	char *res, *pTmp;
	uint8_t indexFail = 1;

	if(smsInit(nUart) != NULL){

		res = SendAtCmd_P(nUart, "AT+CMGL=\"ALL\"\r", "OK", 0); // Read list of new messages
		if(modemGetAtCmdResponse(nUart, timeoutSec)){
			res = modemGetResponseBuff(nUart);
			res = strstr_P( res, PSTR("+CMGL: "));
			if(res != NULL){
				if(!smsSenderNumParsing(res, &SMS)) UART_DBG(SMS.strSenderNumber);
				res += 7; // "+CMGL: "
				pTmp = strstr_P( res , PSTR(","));
				if(pTmp != NULL){// read index
					if((pTmp - res) < MSM_IDX_LEN){// right index length
						*pTmp = '\0';
						if(strcpy(SMS.strMesageIndex, res) != NULL){
							if(debugON) UART_DBG(SMS.strMesageIndex);
							indexFail = 0;
							res = pTmp + 1;
						}
					}
				}

				if(!indexFail){
					res = strstr_P( res, PSTR("#PHY#"));
					if(res != NULL){
						res += 5; // "#PHY#"
						pTmp = strstr_P( res , PSTR("#"));
						if(pTmp != NULL){// read data
							if((pTmp - res) < MSM_BUF_LEN){// right data length
								*pTmp = '\0';
								if(strcpy(SMS.strMessageBuff, res) != NULL){
									if(debugON) UART_DBG(SMS.strMessageBuff);
									return &SMS;
								}
							}
						}
					}
				}
			}
		}
	}

	return NULL;
}

/*
 * Delete Short Message Service: +CMGD
 * return:
 * 		0 - successful
 * 		1 - fail
 */

uint8_t smsDeleteMessage(uint8_t nUart, t_ShortMessageService *pSMS)
{
	char strTmp[32];

	if( sprintf_P(strTmp, PSTR("AT+CMGD=%s\r"), pSMS->strMesageIndex) )
		if(SendAtCmd(nUart, strTmp, "OK", 2) != NULL) return 0;

	return 1;
}

/*
 * return:
 * 		NOT NULL 	- successful
 * 		NULL		- fail
 */
t_smsCmd * smsCmdParsing(t_ShortMessageService *pSMS)
{
	char *pBegin, *pEnd;

	pBegin = pSMS->strMessageBuff;
	// parsing Short Message Service command
	pEnd = strstr_P(pBegin, PSTR(",") );
	if(pEnd != NULL){// get cmd
		*pEnd = '\0';
		if(strcpy(smsCmd.cmdBuf, pBegin) != NULL){// parsing Short Message Service value
			pBegin = pEnd + 1;
			// get value
			if( strcpy(smsCmd.valueBuf, pBegin) != NULL) return &smsCmd;
		}
	}

	return NULL;
}

/*
 * return:
 * 		0 - successful
 * 		1 - fail
 */
uint8_t smsCmdProc(t_ShortMessageService *pSMS)
{
	t_eSmsCmd eSmsCmd;
	t_smsCmd *p_smsCmd;

	p_smsCmd = smsCmdParsing(pSMS);

	if(p_smsCmd != NULL){
		UART_DBG(p_smsCmd->cmdBuf);
		if((atoi(p_smsCmd->cmdBuf)) == atoi("apn")){
			eSmsCmd = E_SMS_CMD_APN;
		}else if((atoi(p_smsCmd->cmdBuf)) == atoi("url")){
			eSmsCmd = E_SMS_CMD_URL;
		}else if((atoi(p_smsCmd->cmdBuf)) == atoi("port")){
			eSmsCmd = E_SMS_CMD_PORT;
		}else if((atoi(p_smsCmd->cmdBuf)) == atoi("mcc")){
			eSmsCmd = E_SMS_CMD_MCC;
		}else if((atoi(p_smsCmd->cmdBuf)) == atoi("mnc")){
			eSmsCmd = E_SMS_CMD_MNC;
		}else return 1;
	}else return 1;

	UART_DBG(p_smsCmd->valueBuf);
	return 0;
}
#endif
