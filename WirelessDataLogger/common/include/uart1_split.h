/*
 * uart1_split.h
 *
 *  Created on: Mar 6, 2017
 *      Author: phytech
 */

#ifndef COMMON_INCLUDE_UART1_SPLIT_H_
#define COMMON_INCLUDE_UART1_SPLIT_H_


#define UART1_SPLIT_BASE_PIN		4
#define UART1_SPLIT_BITS			3

#define UART1_SPLIT_RADIO_UHF		0
#define UART1_SPLIT_GPS				1
#define UART1_SPLIT_DBG				2
#define UART1_SPLIT_NONE			3

uint8_t uart1_splitPortSelect(uint8_t nSelect);

#endif /* COMMON_INCLUDE_UART1_SPLIT_H_ */
