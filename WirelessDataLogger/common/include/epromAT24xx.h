/*
 * epromAT24xx.h
 *
 *  Created on: Nov 3, 2013
 *      Author: reuben
 */

#ifndef EPROMAT24XX_H_
#define EPROMAT24XX_H_

#define AT24C64_DEVICE_ADDR     	0xA0
#define AT24C64_PAGE_SIZE      		32UL
#define AT24C64_MAX_MEMORY_LEN		0x2000	/* 8 KBytes */
#define AT24C256_PAGE_SIZE			64UL
#define AT24C256_MAX_MEMORY_LEN		0x7FFF	/* 32 KBytes */

#define AT24C512_PAGE_SIZE			64UL


#define AT24C512_MAX_MEMORY_LEN		0xFFFF	/* 64 KBytes */

#define AT24C64_MAX_PAGE_SIZE      (64UL*1024UL)/8UL/AT24C64_PAGE_SIZE
#define AT24C256_MAX_PAGE_SIZE     (256UL*1024UL)/8UL/AT24C256_PAGE_SIZE
#define AT24C512_MAX_PAGE_SIZE     (512UL*1024UL)/8UL/AT24C512_PAGE_SIZE


#define DEVICE_ADDR			AT24C64_DEVICE_ADDR
#define DEVICE_ADDR_LEN		2
#define PAGE_SIZE			AT24C256_PAGE_SIZE
#define MAX_MEMORY_LEN		AT24C256_MAX_MEMORY_LEN
#define MAX_PAGE_SIZE		AT24C256_MAX_PAGE_SIZE

typedef enum{
	ENUM_EEP_DEVICE_MEMORY_NONE = 0,
	ENUM_EEP_DEVICE_MEMORY_32KB,
	ENUM_EEP_DEVICE_MEMORY_64KB
}t_enumDeviceMemorySize;

typedef union{
	uint16_t word;
	struct{
		uint8_t low;
		uint8_t hi;
	}byte;
}t_unionEppAddr;

typedef struct{
	t_unionEppAddr	unionEppAddr;
	uint8_t eepData;
}t_structEppAddrData;

#define EEP_DEVICE_MEMORY_SIZE_32KB		AT24C256_MAX_MEMORY_LEN
#define EEP_DEVICE_MEMORY_SIZE_64KB		AT24C512_MAX_MEMORY_LEN

t_enumDeviceMemorySize eepScanDeviceMemorySize(void);

void eeAt24xxInt(void);
uint8_t eeAt24xxReadPage(uint16_t pageNum, uint8_t aPage[]);
uint8_t eeAt24xxWritePage(uint16_t pageNum, uint8_t aPage[]);

uint8_t eeTest24xx(void);

void eepReadAll(void);
uint8_t eepEraseAll(void);

#endif /* EPROMAT24XX_H_ */
