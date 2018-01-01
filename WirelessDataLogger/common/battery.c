/*
 * battery.c
 *
 *  Created on: Mar 30, 2016
 *      Author: phytech
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "bsp.h"
#include "a2d.h"
#include "uartDbgPrint.h"
#include "leds.h"
#include "measure_adc.h"
#include "battery.h"

/*
 * return:
 * 		0	No error
 * 		1	Battery faulty
 * 		2	Low battery
 * 		3	Over voltage battery
 */
uint8_t batteryVoltage(void)
{
	char tmpBuf[32];
	uint8_t	retError = 0;

	uint32_t u32BattValue;

	if(!(PINA & (1<<BATT_FAULT))){
		TXT_DBG("Battery Faulty!");
		retError = 1;
	}

	if(!(PINA & (1<<BATT_CHRG))) TXT_DBG("Battery Charging!");

	u32BattValue = batteryReadVoltageMeasure();
	if(sprintf_P(tmpBuf, PSTR("Battery Voltage: %lumV."), u32BattValue)) UART_DBG(tmpBuf);

	if(u32BattValue < 3300u){
		TXT_DBG("Low threshold battery...Need to protect.");
		TXT_DBG("wireless module OFF!");
		cbi(PORTD, RADIO_PWR_ENA);

		ledControl(LED1, LED_OFF);
		ledControl(LED2, LED_OFF);
		ledControl(LED3, LED_OFF);
		retError = 2;

	}

	return retError;
}

/*
 * return:
 * 			u32Calc - battery voltage [mV]
 */
uint32_t batteryReadVoltageMeasure(void)
{
	uint32_t u32Calc;

	// adc value converted to milivolts.
	u32Calc = (uint32_t)measure_adcGetSampleChannel(BATT_VTG);
	u32Calc *= 2560UL;	// reference voltage = 2560mV.
	u32Calc /= 1024UL;	// a2d full scale = 1024 = 10bits
	// network resistor: 100K+100K
	// Vi = Vo*2
	u32Calc *= 2u;

	return u32Calc;
}

void batteryReadChargerMeasure(void)
{
	char tmpBuf[32];
	unsigned short a2dValue;
	uint32_t u32Calc;

	// adc value converted to milivolts.
	a2dValue = measure_adcGetSampleChannel(ICHRG_MONITOR);
	u32Calc = a2dValue*2560UL;	// reference voltage = 2560mV.
	u32Calc = u32Calc/1024UL;	// a2d full scale = 1024 = 10bits
	//if( sprintf_P(tmpBuf, PSTR("ADC measure: %dmV."), (uint16_t)u32Calc )) UART_DBG(tmpBuf);
	/*
	 * Ichg = (Hprog*Vprog)/Rprog
	 * Hprog = 988
	 * Rprog = 3010
	 */
	u32Calc = (u32Calc*H_PROG)/R_PROG;	// Charge Current Monitor

	if(sprintf_P(tmpBuf, PSTR("Charge Current Monitor: %dmA."), u32Calc)) UART_DBG(tmpBuf);
/*
	u32Calc = (u32Calc*100)/MAX_CHRG_CURRENT_200;	// Battery Charge Monitor Percent
	if(sprintf_P(tmpBuf, PSTR("Battery Charge: %d%%."), u32Calc)) UART_DBG(tmpBuf);*/

}
