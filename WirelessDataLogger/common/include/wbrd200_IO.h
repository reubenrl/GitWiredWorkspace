/*
 * wbrd200_IO.h
 *
 *  Created on: Mar 6, 2017
 *      Author: phytech
 */

#ifndef COMMON_INCLUDE_WBRD200_IO_H_
#define COMMON_INCLUDE_WBRD200_IO_H_

#include <avr/io.h>

/*	PORTA	*/
#define ICHRG_MONITOR			0	// A Current charger monitor: see formula
#define BATT_VTG 				1	// A ADC0: Battery voltage: VBATT/2
#define BATT_CHRG				2	// I Panel solar: Battery charger indicator.
#define BATT_FAULT				3	// I Panel solar: Battery fault indicator
#define PORTA4_NC				4	// Not Connection
#define LED1_IO					5	// O Program status 3
#define LED2_IO					6	// O Program status 2
#define LED3_IO					7	// O Program status 1

/*	PORTB	*/
#define	GPS_PWR_ENA				0	// O GPS supply power enable: 0 - OFF; 1 - ON
#define	GPS_IGNITION			1	// O GPS ignition: 0 - ON; 1 - Hibernate State
#define	MCU_CLOCK_INT			2	// I EXINT2 - real clock time - CLOCK_INT=0: each 60sec
#define	PORTB3_NC				3	// I Low battery indicator -  if VSUP3V8_PWR_EN = 1 (modem3G)
#define	PORTB4_NC				4	// I Push button: reserve
#define	MOSI					5	// I MOS PROGRAMMER. CODE
#define	MISO					6	// I MISO PROGRAMMER. CODE
#define	SCK						7	// I SCK PROGRAMMER. CODE

/*	PORTC	*/
#define	MCU_I2C_SCL				0	// O 	I2C_SCL	TWI: two wire interface - clock
#define	MCU_I2C_SDA				1	// I/O 	I2C_SDA	TWI: two wire interface - data
#define	SAT_PWR_ON				2	// O 	Power ON/OFF satellite
#define	SAT_NTW_AVAIBLE			3	// O 	I If SAT_NTW_AVAIBLE = 0 : Satellite network available indicator
#define	SAT_PWR_ENA				4	// O 	Enable power supply 5V for satellite
#define	GSM_IGNITION  			5	// O 	Ignition GSM Modem - 1 - Pulse 5sec
#define	GSM_SHDW_ENA			6	// O 	Shutdown GSM Modem - 1 - pulse 250msec
#define	GSM_PWR_MON				7	// O 	1.8V Power monitor - GSM Modem

/*	PORTD	*/
#define	MCU_RXD0				0	// I UART0-RDX - COMMON GSM/SATELLITE
#define	MCU_TXD0				1	// O UART0-TDX - COMMON GSM/SATELLITE

									//// Split UART1
#define	MCU_RXD1				2	// I UART1-RDX
#define	MCU_TXD1				3	// O UART1-TDX
#define	MCU_UART10_SELECT		4	// O	00 - UART_RADIO_UHF
#define	MCU_UART11_SELECT		5	// O	01 - UART_GPS
									//		10 - UART_DBG
									//		11 - NONE

#define	RADIO_PWR_ENA			6	// O 	0 - Power wireless 433Mhz OFF
									// 		1 - Power wireless 433Mhz OFF
#define	RADIO_CTS				7	// O 	0 - enable receive data (uart1) from wireless 433Mhz
									//	 	1 - disable
/// Initial ports
#define DDRA_INIT 	(1<<LED3_IO)|(1<<LED2_IO)|(1<<LED1_IO)|(0<<PORTA4_NC)|(0<<BATT_FAULT)|(0<<BATT_CHRG)|(0<<BATT_VTG)|(0<<ICHRG_MONITOR)
#define PORTA_INIT 	(0<<LED3_IO)|(0<<LED2_IO)|(0<<LED1_IO)|(0<<PORTA4_NC)|(1<<BATT_FAULT)|(0<<BATT_CHRG)|(0<<BATT_VTG)|(0<<ICHRG_MONITOR)

#define DDRB_INIT 	(0<<SCK)|(0<<MISO)|(0<<MOSI)|(0<<PORTB4_NC)|(0<<PORTB3_NC)|(0<<MCU_CLOCK_INT)|(1<<GPS_IGNITION)|(1<<GPS_PWR_ENA)
#define PORTB_INIT 	(0<<SCK)|(0<<MISO)|(0<<MOSI)|(0<<PORTB4_NC)|(0<<PORTB3_NC)|(1<<MCU_CLOCK_INT)|(0<<GPS_IGNITION)|(0<<GPS_PWR_ENA)

#define DDRC_INIT 	(0<<GSM_PWR_MON)|(1<<GSM_SHDW_ENA)|(1<<GSM_IGNITION)|(1<<SAT_PWR_ENA)|(0<<SAT_NTW_AVAIBLE)|(1<<SAT_PWR_ON)|(1<<MCU_I2C_SDA)|(1<<MCU_I2C_SCL)
#define PORTC_INIT 	(1<<GSM_PWR_MON)|(0<<GSM_SHDW_ENA)|(0<<GSM_IGNITION)|(0<<SAT_PWR_ENA)|(1<<SAT_NTW_AVAIBLE)|(0<<SAT_PWR_ON)|(1<<MCU_I2C_SDA)|(1<<MCU_I2C_SCL)

#define DDRD_INIT 	(1<<RADIO_CTS)|(1<<RADIO_PWR_ENA)|(1<<MCU_UART11_SELECT)|(1<<MCU_UART10_SELECT)|(1<<MCU_TXD1)|(0<<MCU_RXD1)|(1<<MCU_TXD0)|(0<<MCU_RXD0)
#define PORTD_INIT 	(0<<RADIO_CTS)|(0<<RADIO_PWR_ENA)|(0<<MCU_UART11_SELECT)|(0<<MCU_UART10_SELECT)|(0<<MCU_TXD1)|(0<<MCU_RXD1)|(0<<MCU_TXD0)|(0<<MCU_RXD0)


/// Configuration sleep ports.
#define DDRA_SLEEP 		(1<<LED3_IO)|(1<<LED2_IO)|(1<<LED1_IO)|(0<<PORTA4_NC)|(0<<BATT_FAULT)|(0<<BATT_CHRG)|(0<<BATT_VTG)|(0<<ICHRG_MONITOR)
#define PORTA_SLEEP 	(0<<LED3_IO)|(0<<LED2_IO)|(0<<LED1_IO)|(0<<PORTA4_NC)|(0<<BATT_FAULT)|(0<<BATT_CHRG)|(0<<BATT_VTG)|(0<<ICHRG_MONITOR)

#define DDRB_SLEEP 		(0<<SCK)|(0<<MISO)|(0<<MOSI)|(0<<PORTB4_NC)|(0<<PORTB3_NC)|(0<<MCU_CLOCK_INT)|(1<<GPS_IGNITION)|(1<<GPS_PWR_ENA)
#define PORTB_SLEEP 	(0<<SCK)|(0<<MISO)|(0<<MOSI)|(0<<PORTB4_NC)|(0<<PORTB3_NC)|(0<<MCU_CLOCK_INT)|(0<<GPS_IGNITION)|(0<<GPS_PWR_ENA)

#define DDRC_SLEEP 		(0<<GSM_PWR_MON)|(1<<GSM_SHDW_ENA)|(1<<GSM_IGNITION)|(1<<SAT_PWR_ENA)|(0<<SAT_NTW_AVAIBLE)|(1<<SAT_PWR_ON)|(1<<MCU_I2C_SDA)|(1<<MCU_I2C_SCL)
#define PORTC_SLEEP 	(0<<GSM_PWR_MON)|(0<<GSM_SHDW_ENA)|(0<<GSM_IGNITION)|(0<<SAT_PWR_ENA)|(0<<SAT_NTW_AVAIBLE)|(0<<SAT_PWR_ON)|(1<<MCU_I2C_SDA)|(1<<MCU_I2C_SCL)

#define DDRD_SLEEP 		(0<<RADIO_CTS)|(1<<RADIO_PWR_ENA)|(1<<MCU_UART11_SELECT)|(1<<MCU_UART10_SELECT)|(1<<MCU_TXD1)|(0<<MCU_RXD1)|(1<<MCU_TXD0)|(0<<MCU_RXD0)
#define PORTD_SLEEP 	(0<<RADIO_CTS)|(0<<RADIO_PWR_ENA)|(1<<MCU_UART11_SELECT)|(1<<MCU_UART10_SELECT)|(0<<MCU_TXD1)|(0<<MCU_RXD1)|(0<<MCU_TXD0)|(0<<MCU_RXD0)

#define BSP_INIT()			\
					{		\
						DDRA = DDRA_INIT; PORTA = PORTA_INIT; DDRB = DDRB_INIT; PORTB = PORTB_INIT; 		\
						DDRC = DDRC_INIT; PORTC = PORTC_INIT; DDRD = DDRD_INIT; PORTD = PORTD_INIT; 		\
					}

#define BSP_SLEEP_CONFIG()	\
					{		\
						DDRA = DDRA_SLEEP; PORTA = PORTA_SLEEP; DDRB = DDRB_SLEEP; PORTB = PORTB_SLEEP;		\
						DDRC = DDRC_SLEEP; PORTC = PORTC_SLEEP; DDRD = DDRD_SLEEP; PORTD = PORTD_SLEEP;		\
					}


typedef enum{
	POWER_RST=0,
	EXTERNAL_RST,
	BROWNOUT_RST,
	WATCHDOG_RST,
	JTAG_RST
}t_eResetSource;

#endif /* COMMON_INCLUDE_WBRD200_IO_H_ */
