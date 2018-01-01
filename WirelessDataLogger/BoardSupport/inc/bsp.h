/*
 * bsp.h
 *
 *  Created on: Mar 6, 2017
 *      Author: phytech
 */

#ifndef BSP_H_
#define BSP_H_

#include "bsp_config.h"
#include "global.h"

#if defined(WBRD200_VER1)
#include "wbrd200_IO.h"

#define COM_GSM			0
#define COM_SATELLITE	0
#define COM_DBG			1
#define COM_GPS			1
#define COM_UHF			1

//#define SATELLITE_MODULE_ENABLE
//#define GPS_MODULE_ENABLE

volatile uint8_t debugON;

#endif
#endif /* BSP_H_ */
