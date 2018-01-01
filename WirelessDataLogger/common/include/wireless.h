/*
 * wireless.h
 *
 *  Created on: Mar 15, 2017
 *      Author: phytech
 */

#ifndef WIRELESS_H_
#define WIRELESS_H_

void wirelessInit(void);
uint8_t wirelessPacketRx(uint8_t nUart, uint8_t strOutput[], uint8_t strOutputLen, uint8_t timeout_sec);
void wirelessPacketTx(uint8_t nUart);

#endif /* WIRELESS_H_ */
