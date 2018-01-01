/*
 * epromAT24xx.c
 *
 *  Created on: Nov 3, 2013
 *      Author: reuben
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <avr/io.h>
#include <util/delay.h>

#include "bsp.h"
#include "timer.h"
#include "i2c.h"
#include "uartDbgPrint.h"
#include "epromAT24xx.h"


#define RANDINT(n) (uint16_t)((double)random() / ((double)RAND_MAX + 1) * n) //(random()/(RANDOM_MAX/(n) + 1))

#define EE_ADDR_RANDOM_MAX		32

static uint16_t ai2cEERandomAddr[EE_ADDR_RANDOM_MAX];

// pages to test.
#define NUMBER_PAGE_TO_TEST		EE_ADDR_RANDOM_MAX		// 128(pages)*64(bytes) = 8192 bytes = 8 Kilobytes

void eeAt24xxInt(void)
{
	i2cInit();
}

/*
 * eepAt24xxReadByte
 * 		in 	- uint16_t eepAddr
 * 		out	- uint8_t strData[]
 */
void eepAt24xxReadByte(uint16_t eepAddr, uint8_t strData[])
{
	uint8_t strTmp[sizeof(uint16_t)], eepData;

	// read data from address
	t_unionEppAddr	unionEppAddr = (t_unionEppAddr)eepAddr;
	strTmp[0] = unionEppAddr.byte.hi;
	strTmp[1] = unionEppAddr.byte.low;
	i2cMasterSend(DEVICE_ADDR,sizeof(uint16_t), strTmp );
	i2cMasterReceive(DEVICE_ADDR, sizeof(uint8_t), &eepData);
	*strData = eepData;
}

/*
 * eepAt24xxWriteByte
 * 		in 	- uint16_t eepAddr
 * 		in	- uint8_t eepData
 * 	return:
 * 		0 - successful
 * 		1 - error
 */
uint8_t eepAt24xxWriteByte(uint16_t eepAddr, uint8_t eepData)
{
	uint8_t strTmp[sizeof(t_structEppAddrData)];

	t_structEppAddrData structEppAddrData;
	// write data to address
	structEppAddrData.unionEppAddr.word = eepAddr;
	structEppAddrData.eepData = eepData;
	strTmp[0] = structEppAddrData.unionEppAddr.byte.hi;
	strTmp[1] = structEppAddrData.unionEppAddr.byte.low;
	strTmp[2] = structEppAddrData.eepData;

	if(i2cMasterSendNI(DEVICE_ADDR, sizeof(t_structEppAddrData), strTmp)) return 1;
	 _delay_ms(5);

	 return 0;
}

/*
 * return:
 * 		0 - successful
 * 		1 - error: overrun maximum pages size
 */
uint8_t eeAt24xxReadPage(uint16_t pageNum, uint8_t aPage[])
{
	uint8_t aAddress[DEVICE_ADDR_LEN + 1];
	uint16_t  addressData;

	if(pageNum < MAX_PAGE_SIZE ){
		addressData   =  pageNum*PAGE_SIZE;
		aAddress[0]   = (uint8_t )((addressData >> 8) & 0x00FF); 		// high data address
		aAddress[1]   = (uint8_t )(addressData & 0x00FF);				// low data address
		i2cMasterSend(DEVICE_ADDR,DEVICE_ADDR_LEN,aAddress );
		i2cMasterReceive(DEVICE_ADDR, PAGE_SIZE, aPage);
		return 0;
	}else return 1;
}

/*
 * return:
 * 		0 - successful
 * 		1 - error: overrun maximum pages size
 */
uint8_t eeAt24xxWritePage(uint16_t pageNum, uint8_t aPage[])
{
	uint8_t aData[DEVICE_ADDR_LEN + PAGE_SIZE];
	uint16_t  addressData,i;

    if(pageNum < MAX_PAGE_SIZE){
    	addressData = pageNum*PAGE_SIZE;
    	aData[0] = (uint8_t )((addressData >> 8) & 0x00FF); 	// high data address
    	aData[1] = (uint8_t )(addressData & 0x00FF);			// low data address

        for(i=0; i < PAGE_SIZE; i++){
        	aData[i + DEVICE_ADDR_LEN] = aPage[i];
        }
        i2cMasterSendNI(DEVICE_ADDR, DEVICE_ADDR_LEN + PAGE_SIZE, aData);
        _delay_ms(5);
    }else
    	return 1;

    return 0;
}

/*
 * return:
 * 		0 - successful
 * 		1 - error:
 */
uint8_t eeWriteTest(uint8_t aToWrite[])
{
	uint16_t i;

	for(i=0; i<NUMBER_PAGE_TO_TEST; i++){
		if(eeAt24xxWritePage(ai2cEERandomAddr[i], aToWrite)) return 1;
	}

	return 0;
}

/*
 * return:
 * 		0 - successful
 * 		1 - error:
 */
uint8_t eeVerifyTest(uint8_t aToTest[])
{
	uint8_t aReadPage[PAGE_SIZE + 2];
	uint16_t i,j;

	for(i=0; i<NUMBER_PAGE_TO_TEST; i++){
		if(eeAt24xxReadPage(ai2cEERandomAddr[i], aReadPage)) return 1;
		for(j=0; j<PAGE_SIZE;j++){
			if(aReadPage[j] != aToTest[j]){
				if(debugON){
					UART_DGB_HEX2ASCII((uint8_t *)&i, 1);
					UART_DGB_HEX2ASCII(aReadPage, PAGE_SIZE);
				}
				return 1;
			}
		}
	}

	return 0;
}

void eepReadAll(void)
{
	uint8_t eepPageBuf[PAGE_SIZE],x,y;
	char strTmp[16], *pTmp;
	uint16_t pageCounter, eepAddr, eepMaxPageSize;

	t_enumDeviceMemorySize enumDeviceMemorySize = eepScanDeviceMemorySize();
	switch(enumDeviceMemorySize){
	case ENUM_EEP_DEVICE_MEMORY_32KB:
		eepMaxPageSize = AT24C256_MAX_PAGE_SIZE;
		break;
	case ENUM_EEP_DEVICE_MEMORY_64KB:
		eepMaxPageSize = AT24C512_MAX_PAGE_SIZE;
		break;
	default:
		DbgPrintText_P(PSTR("ENUM_EEP_DEVICE_MEMORY_NONE"));
		return;
	}

	if(sprintf_P(strTmp, PSTR("PageSize:%d"), eepMaxPageSize))  DbgPrintText(strTmp);

	for(pageCounter=0; pageCounter<eepMaxPageSize; pageCounter++){
		if(eeAt24xxReadPage(pageCounter, eepPageBuf)) return;
		eepAddr = pageCounter*64;
		for(y=0;y<PAGE_SIZE/16;y++){
			if(!sprintf_P(strTmp, PSTR("%04X:"), (uint16_t)( eepAddr + y*16))) return;
			DbgPrintBuff(strTmp, strlen(strTmp));
			for(x=0;x<PAGE_SIZE/4;x++){
				pTmp = hex2acii(eepPageBuf[x]);
				if(pTmp != NULL){
					DbgPrintBuff(pTmp, 3);
				}else return;
			}
			if(sprintf_P(strTmp, PSTR("\r\n"))) DbgPrintBuff(strTmp, 2);
		}
	}
}


uint8_t eepEraseAll(void)
{
	uint16_t eepMaxPageSize,x,y;
	uint8_t eepStr[64];
	char strTmp[24];

	t_enumDeviceMemorySize enumDeviceMemorySize = eepScanDeviceMemorySize();
		switch(enumDeviceMemorySize){
		case ENUM_EEP_DEVICE_MEMORY_32KB:
			eepMaxPageSize = AT24C256_MAX_PAGE_SIZE;
			break;
		case ENUM_EEP_DEVICE_MEMORY_64KB:
			eepMaxPageSize = AT24C512_MAX_PAGE_SIZE;
			break;
		default:
			DbgPrintText_P(PSTR("ENUM_EEP_DEVICE_MEMORY_NONE"));
			return 1;
		}


		for(x=0;x<64;x++) eepStr[x] = 0xFF;
		if(sprintf_P(strTmp, PSTR("Erase %d Pages:"), eepMaxPageSize))  DbgPrintText(strTmp);
		for(y=0;y<eepMaxPageSize;y++){
			if(sprintf_P(strTmp, PSTR("%d"), y+1))  DbgPrintText(strTmp);
			if(eeAt24xxWritePage(y, eepStr)) return 1;
		}

		return 0;
}

/*
 * Search device memory size
 * return:
 * 		t_enumDeviceMemorySize:
 * 								ENUM_EEP_DEVICE_MEMORY_NONE
 * 								ENUM_EEP_DEVICE_MEMORY_32KB
 * 								ENUM_EEP_DEVICE_MEMORY_64KB
 */
t_enumDeviceMemorySize eepScanDeviceMemorySize(void)
{
	t_enumDeviceMemorySize	enumDeviceMemorySize = ENUM_EEP_DEVICE_MEMORY_NONE;
	uint8_t  eepData, eepDataAddr0, eepDataAddr64;

	// read data from address 0x0000 and save in to eepDataAddr0
	eepAt24xxReadByte(0x0000, &eepDataAddr0);
	// write data 0x00 in address 0x0000
	if(!eepAt24xxWriteByte(0x0000, 0x00)){
		// read data from address 0x8000 and save in to eepDataAddr64
		eepAt24xxReadByte(0x8000 , &eepDataAddr64);
		// write data 0x80 in address 0x8000
		if(!eepAt24xxWriteByte(0x8000, 0x80)){
		// read data from address 0x0000
			eepAt24xxReadByte(0x0000, &eepData);
			if(!eepData){
				enumDeviceMemorySize = ENUM_EEP_DEVICE_MEMORY_64KB;
				(void)eepAt24xxWriteByte(EEP_DEVICE_MEMORY_SIZE_32KB + 1u, eepDataAddr64);
			}else{
				enumDeviceMemorySize = ENUM_EEP_DEVICE_MEMORY_32KB;
			}

		}

		(void)eepAt24xxWriteByte(0x0000, eepDataAddr0);
	}// write address at 0x0000

	return ( enumDeviceMemorySize);
}

/*
 * return:
 * 		0 - successful
 * 		1 - error:
 */
uint8_t eeTest24xx(void)
{
	uint8_t strTmp[PAGE_SIZE];
	uint16_t *pTmpBuf;
	uint8_t cntIdx,idx,i, sawIdx;


	//scan memory size
	t_enumDeviceMemorySize eeMemorySize = eepScanDeviceMemorySize();
	if( eeMemorySize == ENUM_EEP_DEVICE_MEMORY_32KB){
		TXT_DBG("EEPROM Memory Size:32Kb.");
	}else if(eeMemorySize == ENUM_EEP_DEVICE_MEMORY_64KB){
		TXT_DBG("EEPROM Memory Size:64Kb.");
	}else TXT_DBG("EEPROM Memory Size:Unknown.");

	for(idx=0;idx < EE_ADDR_RANDOM_MAX;idx++){ // create randomly numbers
		if( eeMemorySize == ENUM_EEP_DEVICE_MEMORY_32KB){
			ai2cEERandomAddr[idx] = (uint16_t)(RANDINT(0x3FFF) & 0x00003FFF)/64;
			}else if(eeMemorySize == ENUM_EEP_DEVICE_MEMORY_64KB){
				ai2cEERandomAddr[idx] = (uint16_t)(RANDINT(0x7FFF) & 0x00007FFF)/64;
			}else ai2cEERandomAddr[idx] = 0x00000;
	}

	for(i=0;i<8;i++){// again 8 times to avoid same numbers on the randomly array
		sawIdx = 1;
		for(idx=0;idx < EE_ADDR_RANDOM_MAX-1;idx++){
			pTmpBuf = &ai2cEERandomAddr[idx] + sawIdx;
			for(cntIdx=1; cntIdx <  EE_ADDR_RANDOM_MAX - sawIdx; cntIdx++){
				if(ai2cEERandomAddr[idx] == *pTmpBuf++){
					if( eeMemorySize == ENUM_EEP_DEVICE_MEMORY_32KB){
						ai2cEERandomAddr[idx + cntIdx] = (uint16_t)(RANDINT(0x3FFF) & 0x00003FFF)/64;
						}else if(eeMemorySize == ENUM_EEP_DEVICE_MEMORY_64KB){
							ai2cEERandomAddr[idx + cntIdx] = (uint16_t)(RANDINT(0x7FFFF) & 0x000007FF)/64;
						}else ai2cEERandomAddr[idx] = 0x00000;
					//ai2cEERandomAddr[idx + cntIdx] = (uint16_t)(RANDINT(0x3FFF) & 0x00003FFF)/64;
				}
			}
		}
	}
/*

	for(idx=0;idx < EE_ADDR_RANDOM_MAX;idx++){
		if(sprintf_P(strTmp, PSTR("[%d]%04X"), idx, ai2cEERandomAddr[idx]) != NULL) UART_DBG(strTmp);
	}
*/

	memset(strTmp, 0xAA, PAGE_SIZE);
	if(debugON) TXT_DBG("EEPROM Write Page(64bytes) 0xAA.");
	if(!eeWriteTest(strTmp)){
		TXT_DBG("Verify eeprom.");
		if(eeVerifyTest(strTmp)) return 1;
	}

	memset(strTmp, 0x55, PAGE_SIZE);
	if(debugON) TXT_DBG("EEPROM Write Page(64bytes) 0x55.");
	if(!eeWriteTest(strTmp)){
		TXT_DBG("Verify eeprom.");
		if(!eeVerifyTest(strTmp)){
			if(debugON)  TXT_DBG("Delete EEPROM.");
			memset(strTmp, 0xFF, PAGE_SIZE);
			if(!eeWriteTest(strTmp)) return 0;
		}
	}

	return 1;
}



