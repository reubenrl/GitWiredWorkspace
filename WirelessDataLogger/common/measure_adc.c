/*
 * measure_adc.c
 *
 *  Created on: Mar 6, 2017
 *      Author: phytech
 */

#include <avr/io.h>
#include "timer_sys.h"

#include "a2d.h"
#include "measure_adc.h"

uint16_t measure_adcGetSampleChannel(uint8_t numChannel)
{
	uint8_t  didr0Save, ddraSave, portaSave;
	uint16_t a2dValue;

	didr0Save = DIDR0;
	ddraSave = DDRA;
	portaSave = PORTA;

	DIDR0 |= (1<<numChannel);
	DDRA &= ~(1<<numChannel);
	PORTA &= ~(1<<numChannel);

	a2dSetReference(ADC_REFERENCE_256V);	// set internal reference
	timer_sysDelay_msec(100UL);

	//! Start a conversion
	/// returns the 10-bit value of the conversion when it is finished.
	a2dValue = a2dConvert10bit(numChannel); // flush

	a2dValue = a2dConvert10bit(numChannel);
	DIDR0 = didr0Save;
	DDRA = ddraSave;
	PORTA = portaSave;

	return a2dValue;
}

/*

uint32_t getBattVoltage(void)
{
	uint16_t battVoltageAvg;
	uint32_t u32Calc;

	DDRC |= (1<<PC7);	// PC7 as output
	PORTC |= (1<<PC7); 	// battery switch ON
	_delay_ms(500);

	battVoltageAvg = a2dTesting(PA3);

	PORTC &= ~(1<<PC7); 	// battery switch OFF

	//battVoltageAvg = battVoltageAvg/5;
	u32Calc = (uint32_t)(battVoltageAvg)*2560UL;
	u32Calc = u32Calc*47UL;
	u32Calc = u32Calc/10240UL;

	u32Calc =+ 250UL; // return milivolts + schottky diode voltage

	return u32Calc;
}*/
