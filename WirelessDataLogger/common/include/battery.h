/*
 * battery.h
 *
 *  Created on: Mar 30, 2016
 *      Author: phytech
 */

#ifndef BATTERY_H_
#define BATTERY_H_



#define H_PROG					988UL
#define R_PROG_3_01K			3010UL	// 3.01K - Maximum current 400mA
#define R_PROG_6_04K			6040UL	// 6.04K - Maximum current 200mA
#define R_PROG					R_PROG_3_01K
#define MAX_CHRG_CURRENT_400	400UL
#define MAX_CHRG_CURRENT_200	200UL

uint8_t batteryVoltage(void);
uint32_t batteryReadVoltageMeasure(void);
void  batteryReadChargerMeasure(void);


#endif /* BATTERY_H_ */
