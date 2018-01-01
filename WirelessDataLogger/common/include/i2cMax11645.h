/*
 * i2cMax11645.h
 *
 *  Created on: Dec 18, 2013
 *      Author: reuben
 */

#ifndef I2CMAX11645_H_
#define I2CMAX11645_H_


#define MAX11645_DEVICE_ADDR		0x6C

#define	SETUP_RESERVA	0
#define	SETUP_RST		1
#define	SETUP_BIP_UNI	2
#define	SETUP_CLK		3
#define	SETUP_SEL0		4
#define	SETUP_SEL1		5
#define	SETUP_SEL2		6
#define	SETUP_REG		7

#define	CONFIG_SGL_DIF	0
#define	CONFIG_CS0		1
#define	CONFIG_RESERVA2	2
#define	CONFIG_RESERVA3	3
#define	CONFIG_RESERVA4	4
#define	CONFIG_SCAN0	5
#define	CONFIG_SCAN1	6
#define	CONFIG_REG		7

#define MAX11645_PWR_ON		1
#define MAX11645_PWR_OFF	0

typedef enum{
	E_INTERNAL_MAX11645,
	E_EXTERNAL_MAX11645
}t_eI2C_ADC;

//volatile t_E_I2C_ADC	E_I2C_ADC ;

uint8_t i2c_Max11645Pwr(t_eI2C_ADC eI2C_ADC,  uint8_t state);
void i2c_Max11645Setup(void);
void i2c_Max11645Config(uint8_t chnl);
uint8_t *i2c_Max11645ReadByte(uint8_t chnl);

#endif /* A2DMAX11645_H_ */
