/*
 * xE910.h
 *
 *  Created on: Jun 7, 2016
 *      Author: phytech
 */

#ifndef XE910_H_
#define XE910_H_

#include "modem.h"

#define atOffModem_ENA	((uint8_t) 0x01)
#define atOffModem_DIS	((uint8_t) 0x00)


//uint8_t xE910ModemPwrOn(void);
//uint8_t xE910ModemPwrOff(uint8_t nUart, uint8_t atOffMode);
//uint8_t xE910ModemPower(uint8_t nUart, uint8_t state,  uint8_t atOffMode);
uint8_t xE910CommunicationTest(uint8_t nUart);
uint8_t xE910tSetParm(uint8_t nUart);


uint8_t modemNetworkRegistrationReport(uint8_t nUart, uint16_t  nIntervals_sec, uint8_t nRetries);

#endif /* XE910_H_ */
