/*
 * myi2c.c
 *
 *  Created on: May 31, 2016
 *      Author: phytech
 */
#include <avr/io.h>
#include "i2c.h"

void myi2cSetBitrate(uint16_t bitrateKHz);

// functions
void myi2cInit(void)
{
	// set pull-up resistors on I2C bus pins
	// TODO: should #ifdef these
	sbi(PORTC, 0);	// i2c SCL on ATmega163,323,16,32,etc
	sbi(PORTC, 1);	// i2c SDA on ATmega163,323,16,32,etc
	// set i2c bit rate to 100KHz
	myi2cSetBitrate(100);
	// enable TWI (two-wire interface)
	sbi(TWCR, TWEN);
	// disable TWI interrupt
	cbi(TWCR, TWIE);
}

void myi2cSetBitrate(uint16_t bitrateKHz)
{
	uint8_t bitrate_div;
	// set i2c bitrate
	// SCL freq = F_CPU/(16+2*TWBR))
	// calculate bitrate division
	bitrate_div = ((F_CPU/1000l)/bitrateKHz);
	if(bitrate_div >= 16) bitrate_div = (bitrate_div-16)/2;
	TWBR = bitrate_div;
}

i/*nline void myi2cSendStart(void)
{
	// send start condition
	outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWSTA));
}

inline void myi2cSendStop(void)
{
	// transmit stop condition
	// leave with TWEA on for slave receiving
	outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA)|BV(TWSTO));
}

inline void myi2cWaitForComplete(void)
{
	// wait for i2c interface to complete operation
	while( !(inb(TWCR) & BV(TWINT)) );
}

inline void myi2cSendByte(u08 data)
{
	// save data to the TWDR
	outb(TWDR, data);
	// begin send
	outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
}

inline void myi2cReceiveByte(u08 ackFlag)
{
	// begin receive over i2c
	if( ackFlag )
	{
		// ackFlag = TRUE: ACK the recevied data
		outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
	}
	else
	{
		// ackFlag = FALSE: NACK the recevied data
		outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
	}
}

inline u08 myi2cGetReceivedByte(void)
{
	// retieve received data byte from i2c TWDR
	return( inb(TWDR) );
}

inline u08 myi2cGetStatus(void)
{
	// retieve current i2c status from i2c TWSR
	return( inb(TWSR) );
}
*/

uint8_t myi2cMasterSend(uint8_t deviceAddr, uint8_t length, uint8_t* data)
{
	uint8_t retval = I2C_OK;

	// send start condition
	i2cSendStart();
	i2cWaitForComplete();

	// send device address with write
	i2cSendByte( deviceAddr & 0xFE );
	i2cWaitForComplete();

	// check if device is present and live
	if( inb(TWSR) == TW_MT_SLA_ACK)
	{
		// send data
		while(length)
		{
			i2cSendByte( *data++ );
			i2cWaitForComplete();
			length--;
		}
	}
	else
	{
		// device did not ACK it's address,
		// data will not be transferred
		// return error
		retval = I2C_ERROR_NODEV;
	}

	// transmit stop condition
	// leave with TWEA on for slave receiving
	i2cSendStop();
	while( !(inb(TWCR) & BV(TWSTO)) );

	return retval;
}


uint8_t myi2cMasterReceive(uint8_t deviceAddr, uint8_t length, uint8_t *data)
{
	uint8_t retval = I2C_OK;

	// disable TWI interrupt
	cbi(TWCR, TWIE);

	// send start condition
	i2cSendStart();
	i2cWaitForComplete();

	// send device address with read
	i2cSendByte( deviceAddr | 0x01 );
	i2cWaitForComplete();

	// check if device is present and live
	if( inb(TWSR) == TW_MR_SLA_ACK)
	{
		// accept receive data and ack it
		while(length > 1)
		{
			myi2cReceiveByte(TRUE);
			myi2cWaitForComplete();
			*data++ = myi2cGetReceivedByte();
			// decrement length
			length--;
		}

		// accept receive data and nack it (last-byte signal)
		myi2cReceiveByte(FALSE);
		myi2cWaitForComplete();
		*data++ = myi2cGetReceivedByte();
	}
	else
	{
		// device did not ACK it's address,
		// data will not be transferred
		// return error
		retval = I2C_ERROR_NODEV;
	}

	// transmit stop condition
	// leave with TWEA on for slave receiving
	myi2cSendStop();

	// enable TWI interrupt
	sbi(TWCR, TWIE);

	return retval;
}
