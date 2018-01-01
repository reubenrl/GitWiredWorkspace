/*
 * ShortMessageService.h
 *
 *  Created on: Jul 28, 2016
 *      Author: phytech
 */

#ifndef COMMON_INCLUDE_SHORTMESSAGESERVICE_H_
#define COMMON_INCLUDE_SHORTMESSAGESERVICE_H_


#define MSM_IDX_LEN					4
#define MSM_BUF_LEN					32
#define MSM_SNDR_NUM_BUF_LEN		16
typedef struct{
	char strMesageIndex[MSM_IDX_LEN + 1];
	char strSenderNumber[MSM_SNDR_NUM_BUF_LEN + 1];
	char strMessageBuff[MSM_BUF_LEN + 1];
}t_ShortMessageService;

#define MSM_CMD_BUF_LEN			4
#define MSM_VALUE_BUF_LEN		28
typedef enum{
	E_SMS_CMD_APN,	/* Access Point Network		*/
	E_SMS_CMD_URL,	/* 							*/
	E_SMS_CMD_PORT,
	E_SMS_CMD_MCC,	/* Mobile Country Code		*/
	E_SMS_CMD_MNC	/* Mobile Network Code		*/
}t_eSmsCmd;

typedef struct{
	char cmdBuf[MSM_CMD_BUF_LEN + 1];
	char valueBuf[MSM_VALUE_BUF_LEN +1];
}t_smsCmd;

t_ShortMessageService * smsGetMessageList(uint8_t nUart, uint8_t timeoutSec);
uint8_t smsDeleteMessage(uint8_t nUart, t_ShortMessageService *pSMS);
uint8_t smsCmdProc(t_ShortMessageService *pSMS);

#endif /* COMMON_INCLUDE_SHORTMESSAGESERVICE_H_ */
