/*
 * rtc_s35390a.c
 *
 *  Created on: Jan 24, 2013
 *      Author: reuben
 */

#include <avr/io.h>
#include <avr/pgmspace.h>	// include AVR program memory support
#include <stdio.h>
#include <extint.h>

#include "bsp.h"
#include "timer_sys.h"
#include "uartDbgPrint.h"
#include "leds.h"
#include "timer.h"
#include "i2c.h"
#include "rtc_s35390a.h"

#define CLK_INT_DDR		DDRB
#define CLK_INT_PORT	PORTB
#define CLK_INT_IN		PINB
#define CLK_INT_PIN		MCU_CLOCK_INT

volatile uint8_t startCrystalTest;
volatile uint8_t rtcLowIntChk;
volatile uint8_t rtcHighIntChk;

void rtcInt(void)
{
	uint16_t saveCTimer1 = TCNT1;
	TCNT1 = 0;

	gbRTCInt = TRUE;

	if(startCrystalTest){
		if(CLK_INT_IN & (1<<CLK_INT_PIN)){ // test high pulse
			extintConfigure(EXTINT2, EXTINT_EDGE_FALLING);
			if((saveCTimer1 > 1710) && (saveCTimer1 <= 1800)) rtcHighIntChk = 1;
			else rtcHighIntChk = 0;
		}else{ // test low pulse
			extintConfigure(EXTINT2, EXTINT_EDGE_RISING);
			if((saveCTimer1 > 1710) && (saveCTimer1 <= 1800)) rtcLowIntChk = 1;
			else rtcLowIntChk = 0;
		}
		//startCrystalTest = 0;
		if(rtcLowIntChk && rtcHighIntChk) ledControl(LED3, LED_ON);
		else
			ledControl(LED3, LED_OFF);
	}
}

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
// INT2: On
// INT2 Mode: Falling Edge
// Interrupt on any change on pins PCINT0-7: Off
// Interrupt on any change on pins PCINT8-15: Off
// Interrupt on any change on pins PCINT16-23: Off
// Interrupt on any change on pins PCINT24-31: Off
//EICRA=(1<<ISC21) | (0<<ISC20) | (0<<ISC11) | (0<<ISC10) | (0<<ISC01) | (0<<ISC00);
//EIMSK=(1<<INT2) | (0<<INT1) | (0<<INT0);
//EIFR=(1<<INTF2) | (0<<INTF1) | (0<<INTF0);
//PCICR=(0<<PCIE3) | (0<<PCIE2) | (0<<PCIE1) | (0<<PCIE0);

uint8_t rtcInit(void)
{
	uint8_t u8Ret = FALSE;

	startCrystalTest = 0;

	i2cInit();
	timer_sysDelay_msec(500UL);
	// Initial Status register 1, after initialization.
	rtcReadReg(CMND_STATUS1, 1, (uint8_t *)&STATUS1_REG);	// get register
	gu8Cntrl1Reg = STATUS1_REG; // view register before

	if(POC || BLD){ // power on detected
		timer_sysDelay_msec(500UL);
		do{
			//SC0 = 1;			// b5 set
			RTC_RESET = 1;		// b7 set
			rtcWriteReg(CMND_STATUS1, 1, (uint8_t *)&STATUS1_REG);
			rtcReadReg(CMND_STATUS1, 1, (uint8_t *)&STATUS1_REG);	// get register
		}while(POC || BLD);

		u8Ret = TRUE;
	}

	do{
		_12_24 = 1;		// 24-hour expression
		RTC_INT1 = 0;
		RTC_INT2 = 0;
		rtcWriteReg(CMND_STATUS1, 1, (uint8_t *)&STATUS1_REG);
		rtcReadReg(CMND_STATUS1, 1, (uint8_t *)&STATUS1_REG);	// get register
	}while(!_12_24);

	return u8Ret;
}

#define RTC_CNFG_FRQ_50_PORCENT 	1
#define RTC_CNFG_FRQ_PER_MINUTE		2
#define RTC_SET_FRQ_USER			3
#define RTC_INT1_32K_OUT			4

#define RTC_FRQ_USER_16HZ			1

void rtcConfigFrequencyInt(uint8_t cnfFrqMsg, uint8_t cnfUserFrq)
{

	sbi(CLK_INT_PORT, CLK_INT_PIN);	// pullup
	cbi(CLK_INT_DDR, CLK_INT_PIN);	// configure clock interrupt 0 as input
	gbRTCInt = FALSE;
	gPulseTimeOut = 0;

	rtcReadReg(CMND_STATUS1, 1, (uint8_t *)&STATUS1_REG);	// get register
	rtcReadReg(CMND_STATUS2, 1, (uint8_t *)&STATUS2_REG);	// get register
	// Configure register Status2
	TEST 	= 0;
	INT2AE 	= 0;
	INT2ME 	= 0;
	INT2FE	= 0;
	_32KE	= 0;

	switch(cnfFrqMsg){
	case RTC_CNFG_FRQ_50_PORCENT:
		// Minute-periodical interrupt output
		INT1AE  = 0;
		INT1ME	= 1;
		INT1FE	= 1;
		break;
	case RTC_CNFG_FRQ_PER_MINUTE:
		// Per-minute edge interrupt
		INT1ME	= 1;
		INT1FE	= 0;
		break;
	case RTC_SET_FRQ_USER:
		// Configure register Status2
		// Output of user-set frequency
		INT1AE  = 0;
		INT1ME	= 0;
		INT1FE	= 1;

		switch(cnfUserFrq){
		case RTC_FRQ_USER_16HZ:
			// Configure register Status1
			// 16Hz frequency select
			INT1_REG_FRQCY_16HZ = 1;
			break;
		default:
			break;
		}
		break;

	case RTC_INT1_32K_OUT:	// Output Modes for INT1 Pin
		_32KE = 1;			// 32.768 kHz output
		break;

	default:
		return;
	}

	// Update registers
	rtcWriteReg(CMND_INT1, 1, (uint8_t *)&INT1_REG);
	rtcWriteReg(CMND_STATUS2, 1, (uint8_t *)&STATUS2_REG);
}

void rtcTestTimer1(void)
{
	gPulseTimeOut = 0;
	cbi(EIMSK, EXTINT2);	// disable EXTINT2
}

volatile uint16_t rtcPulseCounter;

void extInt2Counter(void)
{
	rtcPulseCounter++;
}

void extInt2Ini(void)
{
	extintConfigure(EXTINT2, EXTINT_EDGE_FALLING);
	extintDetach(EXTINT2);
	extintAttach(EXTINT2,extInt2Counter);

}

uint8_t rtcCrystalFrequencyTest(void)
{
	char bufferTmp[48];
	uint8_t ret=0;
	// save backup register
	uint8_t tmr0IESave 	= TIMSK0;
	uint8_t tmr2IESave 	= TIMSK2;
	uint8_t adcIESave 	= ADCSRA;
	uint8_t twiIESave 	= TWCR;
	uint8_t uart0IESave = UCSR0B;
	uint8_t uart1IESave = UCSR1B;

	//rtcConfigFrequencyInt(RTC_SET_FRQ_USER, RTC_FRQ_USER_16HZ);
	rtcConfigFrequencyInt(RTC_INT1_32K_OUT, 0);
	TXT_DBG("Waiting 2 second...");
	timer_sysDelay_msec(250UL);
	//_delay_ms(250);

	TIMSK0 	= 0;
	TIMSK2 	= 0;
	ADCSRA 	= 0;
	TWCR 	= 0;
	UCSR0B 	= 0;
	UCSR1B 	= 0;

	extInt2Ini();
	// initialize timer 1
	TCCR1B |= (1 << WGM12); // Configure timer 1 for CTC mode
	// formula:
	// Target Timer Count = ((1 / Target Frequency) / (Prescale / Input Frequency)) - 1
	// Set CTC compare value to 0.5Hz at 36686400Hz AVR clock, with a prescaler of 1024
	//OCR1A   = (F_CPU/(1024UL/2UL)) - 1UL;
	// Set CTC compare value to 1Hz at 36686400Hz AVR clock, with a prescaler of 1024
	OCR1A   = (F_CPU/1024UL) - 1UL;
	// set prescaler
	timer1SetPrescaler( TIMER_CLK_DIV1024 );
	timerAttach(TIMER1OUTCOMPAREA_INT, rtcTestTimer1);
	TCNT1 = 0;				// reset TCNT1
	sbi(TIFR1, OCF1A);
	sbi(TIMSK1, OCIE1A);	// enable CTC interrupt
	sbi(EIFR, INTF2);
	sbi(EIMSK, EXTINT2);	// enable EXTINT2
	sei();
	rtcPulseCounter = 0;
	gPulseTimeOut = 1;
	// wait for 5 seconds.
	while(gPulseTimeOut);
	cbi(TIMSK1, OCIE1A);	// disable CTC interrupt
	// reposit backup register
	TIMSK0 	= tmr0IESave;
	TIMSK2 	= tmr2IESave;
	ADCSRA 	= adcIESave;
	TWCR 	= twiIESave;
	UCSR0B 	= uart0IESave;
	UCSR1B 	= uart1IESave;

	timer_sysDelay_msec(250UL);
	//_delay_ms(250);
	if(debugON)  TXT_DBG("Desirable crystal value 32768Hz/0.5%.");
	if(sprintf_P(&bufferTmp[0],PSTR("Measure: RTC_CRYSTAL_FRQ = %uHz."), rtcPulseCounter))	UART_DBG(bufferTmp);
	if(rtcPulseCounter){
		if((rtcPulseCounter >= 32687) && (rtcPulseCounter <= 32849)) ret = 1;
	}
	return ret;
}

void rtcReadReg(uint8_t u8Reg, uint8_t u8LenData, uint8_t* pu8Data)
{
	i2cMasterReceive((S35390A_DEVICE_ADDR | (u8Reg << 1)), u8LenData, pu8Data);

}

void rtcWriteReg(uint8_t u8Reg, uint8_t u8LenData, uint8_t* pu8Data)
{
	i2cMasterSend((S35390A_DEVICE_ADDR | (u8Reg << 1)), u8LenData, pu8Data);

}


// Year to Seconds.
void rtcSetData_1(void)
{
	uint8_t i, aDataBuf[8];
	uint8_t *p_u8Data = (uint8_t *)&DATA1_REG;

	 for(i=0; i < 7; i++)
		aDataBuf[i] = u8SwapBits(p_u8Data[i]);

	rtcWriteReg(CMND_DATA1, 7, aDataBuf);
}

// Hours to Seconds.
void rtcSetData_2(void)
{
	uint8_t i, aDataBuf[8];
	uint8_t *p_u8Data = (uint8_t *)&DATA2_REG;

	for(i=0; i < 3; i++)
		aDataBuf[i] = u8SwapBits(p_u8Data[i]);

	rtcWriteReg(CMND_DATA2,3,aDataBuf);
}

int rtcGetUpdate(char str[])
{
	int n;

	rtcReadReg(CMND_DATA1, 7, (uint8_t *)&DATA1_REG );
	uint8_t *pReg = (uint8_t *)&DATA1_REG;
	for(uint8_t i=0; i < 7; i++) pReg[i] = u8SwapBits(pReg[i]);

	n = sprintf_P(str, PSTR("Date:%x/%x/%x  "), DAY, MONTH, YEAR);
	n += sprintf_P((char *)(str + n),PSTR("Time:%x:%x:%x\r\n"),HOUR,MINUTE,SECOND);

	return n;
}

void rtcSleepConfig(void)
{
	// sleep configuration register Status2
	TEST 	= 0;
	// INT2 disable
	INT2AE 	= 0;
	INT2ME 	= 0;
	INT2FE	= 0;
	_32KE	= 0;
	// Minute-periodical interrupt 2
	INT1AE	= 1;
	INT1ME	= 1;
	INT1FE	= 1;
	// Update register Status2
	rtcWriteReg(CMND_STATUS2, 1, (unsigned char *)&STATUS2_REG);
}


uint8_t u8SwapBits(uint8_t u8DataIn)
{
	uint8_t  u8Carry, u8Data;
    char i;

    for(i=u8Data=0; i < 8; i++)
    {
        u8Carry = (u8DataIn & (1 << i)) ? 1 : 0;
        u8Data <<= 1;
        u8Data |= u8Carry;
    }

    return u8Data;
}

// Hex to BCD for values less than 0x64.
uint8_t u8Hex2Bcd(uint8_t u8HexValue)
{
	uint8_t u8BcdValue, i;

    for(i=0; u8HexValue >= 10; i += 1)
        u8HexValue -= 10;

    u8BcdValue = ((uint8_t)(i << 4) | (uint8_t)(u8HexValue & 0x0F));

    return u8BcdValue;
}

uint8_t u8Bcd2Hex(uint8_t u8BcdValue)
{
	uint8_t u8HexValue;

    u8HexValue = (u8BcdValue >> 4)*10;
    u8HexValue += (u8BcdValue & 0x0F);

    return u8HexValue;
}

