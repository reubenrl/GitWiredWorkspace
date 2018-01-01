/*
 * i2cMax11645.c
 *
 *  Created on: Dec 17, 2013
 *      Author: reuben
 */
#if defined (RTS20_REV41_IO) || defined (RTS22_VER20_IO) || defined (RTS22_VER30_IO)
#include <avr/io.h>
#include <util/delay.h>
#include "uartDbgPrint.h"

#include "hardware_io.h"
#include "i2c.h"
#include "i2cMax11645.h"

/*
 * MAX11645
 * Low-Power, 1-/2-Channel, I2C, 12-Bit ADCs
 * PART (MAX11645EUA+)
 * TEMP RANGE (-40C to +85C)
 * PACKAGE (8 uMAX)
 * I2C SLAVE ADDRESS (0110110)
 */

static uint8_t a2dValueBuf[4];

/*
 * return:
 * 		0 - successful
 * 		1 - fail
 */

uint8_t i2c_Max11645Pwr(t_eI2C_ADC eI2C_ADC,  uint8_t state)
{
	if(eI2C_ADC == E_INTERNAL_MAX11645){
		if(state){// ON
			sbi(PORTC, ADC_ON);
		}else{//OFF
			cbi(PORTC, ADC_ON);
		}
	}else if(eI2C_ADC == E_EXTERNAL_MAX11645){
		if(state){// ON
				sbi(PORTC, ADC_ON_EXT);
		}else{//OFF
			cbi(PORTC, ADC_ON_EXT);
		}
	}else return 1;

	_delay_ms(500);

	return 0;
}


// functions
void i2c_Max11645Setup(void)
{
	uint8_t reg = 0;

	reg = 	(1<<SETUP_REG)|										// setup byte
			(0<<SETUP_SEL2)|(1<<SETUP_SEL1)|(0<<SETUP_SEL0)|	// External reference
			(0<<SETUP_CLK)| 									// internal clock
			(0<<SETUP_BIP_UNI)|									// unipolar
			(0<<SETUP_RST)|										// reset the configuration register to default.
			(0<<SETUP_RESERVA);

	i2cMasterSend(MAX11645_DEVICE_ADDR, 1, &reg); 	// Write setup.
}

void i2c_Max11645Config(uint8_t chnl)
{
	uint8_t reg = 0;
	reg = 	(0<<CONFIG_REG)|						// configuration byte
			(1<<CONFIG_SCAN1)|(1<<CONFIG_SCAN0)|	// Converts the input selected by CS0.
			(0<<CONFIG_RESERVA4)|(0<<CONFIG_RESERVA3)|(0<<CONFIG_RESERVA2)|
			(0<<CONFIG_CS0)|						// Channel select.
			(1<<CONFIG_SGL_DIF);					// single-ended

	if(chnl) reg |= (1<<CONFIG_CS0);

	i2cMasterSend(MAX11645_DEVICE_ADDR, 1, &reg); 	// Write setup.
}

uint8_t *i2c_Max11645ReadByte(uint8_t chnl)
{
	i2c_Max11645Config(chnl);
	i2cMasterReceive(MAX11645_DEVICE_ADDR, 3, a2dValueBuf);

	return (&a2dValueBuf[1]);
}
#endif
