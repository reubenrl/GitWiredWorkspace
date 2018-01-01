/*
 * rtc_s35390a.h
 *
 *  Created on: Jan 24, 2013
 *      Author: reuben
 */

#ifndef RTC_S35390A_H_
#define RTC_S35390A_H_


#define S35390A_DEVICE_ADDR		0x60 // 01100000

// Command registers
#define CMND_STATUS1			0x00
#define CMND_STATUS2			0x01
// year data to...
#define CMND_DATA1				0x02
// hour data to...
#define CMND_DATA2				0x03
#define CMND_INT1				0x04
#define CMND_INT2				0x05
#define CMND_CLK_CORRECTION		0x06
#define CMND_FREE				0x07

typedef unsigned char IO_BYTE;

/* REGISTER BIT STRUCTURES */

typedef union{   /*  CMND_STATUS1 */
	IO_BYTE	byte;
    struct{
		IO_BYTE _POC		:1;
		IO_BYTE _BLD		:1;
		IO_BYTE _INT1		:1;
		IO_BYTE _INT2		:1;
		IO_BYTE _SC1		:1;
		IO_BYTE _SC0		:1;
		IO_BYTE __12_24		:1;
		IO_BYTE _RESET		:1;
    }bit;
}t_STATUS1_REG;

typedef union{   /*  CMND_STATUS2 */
    IO_BYTE	byte;
    struct{
    	IO_BYTE _TEST	:1;	/*  */
    	IO_BYTE _INT2AE	:1;	/* select the output mode for the INT2 */
    	IO_BYTE _INT2ME	:1;	/*  */
    	IO_BYTE _INT2FE	:1; /*  */
    	IO_BYTE __32KE	:1; /* select the output mode for the INT1  */
    	IO_BYTE _INT1AE	:1; /*  */
    	IO_BYTE _INT1ME	:1; /*  */
    	IO_BYTE _INT1FE	:1; /*  */
    }bit;
}t_STATUS2_REG;

typedef union{/* Number of months */
	 IO_BYTE	byte;
	 struct{
 		IO_BYTE		_M			:5;
// 		IO_BYTE		_unused0	:1;
// 		IO_BYTE		_unused1	:1;
// 		IO_BYTE		_unused2	:1;
 		 }bit;
}t_month;

typedef union{/* Number of days */
	IO_BYTE	byte;
	struct{
		 IO_BYTE	_D			:6;
//		 IO_BYTE	_unused7	:1;
//		 IO_BYTE	_unused8	:1;
	 }bit;
}t_day;

typedef union{/* Number of weeks */
	IO_BYTE	byte;
	struct{
		 IO_BYTE	_W			:3;
//		 IO_BYTE	_unused4	:1;
//		 IO_BYTE	_unused5	:1;
//		 IO_BYTE	_unused6	:1;
//		 IO_BYTE	_unused7	:1;
//		 IO_BYTE	_unused8	:1;
	 }bit;
}t_week;

typedef union{/* Hours */
	IO_BYTE	byte;
	struct{

		 IO_BYTE	_H			:6;
		 IO_BYTE	_am_pm		:1;
//		 IO_BYTE	_unused8	:1;
	 }bit;
}t_hour;

typedef union{/* Minute */
	IO_BYTE	byte;
	struct{
		 IO_BYTE	_M			:7;
//		 IO_BYTE	_unused8	:1;
	 }bit;
}t_minute;

typedef union{/* Second */
	IO_BYTE	byte;
	struct{
		IO_BYTE	_S			:7;
//		IO_BYTE	_unused8	:1;
	 }bit;
}t_second;

typedef struct{   /*  CMND_DATA2 */
	t_hour		_hour;
	t_minute	_minute;
	t_second	_second;
}t_DATA2_REG;

typedef struct{ /*  CMND_DATA1 */
	IO_BYTE			_year;
	t_month			_month;
	t_day			_day;
	t_week			_week;
	t_DATA2_REG		_data2_reg;
}t_DATA1_REG;

typedef union{   /*  ALARM1_WEEK */
    IO_BYTE	byte;
    struct{
    	IO_BYTE _W			:3;
    	IO_BYTE _unsused4	:1; /*  */
    	IO_BYTE _unsused5	:1;
    	IO_BYTE _unsused6	:1; /*  */
    	IO_BYTE _unsused7	:1;
    	IO_BYTE _A1WE		:1;	/*  */


    }bit;
}t_alarm1_week;

typedef union{/* ALARM1_HOUR */
	IO_BYTE	byte;
	struct{
		 IO_BYTE	_H			:6;
		 IO_BYTE	_AM_PM		:1;
		 IO_BYTE	_A1HE		:1;


	 }bit;
}t_alarm1_hour;

typedef union{/* ALARM1_MINUTE */
	IO_BYTE	byte;
	struct{
		IO_BYTE	_M			:7;
		IO_BYTE	_A1ME		:1;

	 }bit;
}t_alarm1_minute;

typedef struct{/* SET FREQUENCY */
	IO_BYTE	_SC		:3;
	IO_BYTE	_16HZ	:1;
	IO_BYTE	_8HZ	:1;
	IO_BYTE	_4HZ	:1;
	IO_BYTE	_2HZ	:1;
	IO_BYTE	_1HZ	:1;
}t_set_frequency;

typedef struct{ /* CMND_INT1 */
	union{
		t_alarm1_week		_alarm1_week;
		t_set_frequency		_set_frequency;
	}byte;
	t_alarm1_hour			_alarm1_hour;
	t_alarm1_minute			_alarm1_minute;
}t_INT1_REG;

typedef union{   /*  ALARM2_WEEK */
    IO_BYTE	byte;
    struct{
    	IO_BYTE _W			:3;
    	IO_BYTE _unsused4	:1; /*  */
    	IO_BYTE _unsused5	:1;
    	IO_BYTE _unsused6	:1; /*  */
    	IO_BYTE _unsused7	:1;
    	IO_BYTE _A2WE		:1;	/*  */
    }bit;
}t_alarm2_week;

typedef union{/* ALARM2_HOUR */
	IO_BYTE	byte;
	struct{
		IO_BYTE	_H			:6;
		IO_BYTE	_AM_PM		:1;
		IO_BYTE	_A2HE		:1;
	 }bit;
}t_alarm2_hour;

typedef union{/* ALARM2_MINUTE */
	IO_BYTE	byte;
	struct{
		IO_BYTE	_M			:7;
		IO_BYTE	_A2ME		:1;
	 }bit;
}t_alarm2_minute;


typedef union{   /*  CMND_INT2 */
	union{
		t_alarm2_week		_alarm2_week;
		t_set_frequency		_set_frequency;
	}byte;
	t_alarm2_hour			_alarm2_hour;
	t_alarm2_minute			_alarm2_minute;

 }t_INT2_REG;

typedef struct{   /*  CMND_CLOCK_CORRECTION */
    IO_BYTE	byte;
 }t_CLOCK_CORRECTION_REG;

typedef struct{   /*  CMND_FREE */
    IO_BYTE	byte;
 }t_FREE_REG;


volatile t_STATUS1_REG		_status1;			/*  STATUS 1 */
#define STATUS1_REG			_status1.byte
#define POC					_status1.bit._POC
#define BLD					_status1.bit._BLD
#define RTC_INT1			_status1.bit._INT1
#define RTC_INT2			_status1.bit._INT2
#define SC0					_status1.bit._SC0
#define SC1					_status1.bit._SC1
#define _12_24				_status1.bit.__12_24
#define RTC_RESET			_status1.bit._RESET

volatile t_STATUS2_REG		_status2;			/*  STATUS 2 */
#define STATUS2_REG			_status2.byte
#define TEST				_status2.bit._TEST
#define INT2AE				_status2.bit._INT2AE
#define INT2ME				_status2.bit._INT2ME
#define INT2FE				_status2.bit._INT2FE
#define _32KE				_status2.bit.__32KE
#define INT1AE				_status2.bit._INT1AE
#define INT1ME				_status2.bit._INT1ME
#define INT1FE				_status2.bit._INT1FE

volatile t_DATA1_REG		_data1;			/* DATA 1  */
#define DATA1_REG			_data1
#define YEAR				_data1._year
#define MONTH				_data1._month.bit._M
#define DAY					_data1._day.bit._D
#define WEEK				_data1._week.bit._W
#define HOUR				_data1._data2_reg._hour.bit._H
#define MINUTE				_data1._data2_reg._minute.bit._M
#define SECOND				_data1._data2_reg._second.bit._S
#define AM_PM				_data._data2_reg._hour.bit._am_pm

#define DATA2_REG			_data1._data2_reg		/*  DATA 2 */

volatile t_INT1_REG					_int1;	 /* INT1  */
#define INT1_REG					_int1
#define INT1_REG_ALARM				_int1.byte._alarm1_week
#define INT1_REG_FRQCY_16HZ			_int1.byte._set_frequency._16HZ

/*
#define TBQ0						_reg_tmr_b_freq_ctrl.bit._TBQ0
#define TBQ1						_reg_tmr_b_freq_ctrl.bit._TBQ1
#define TBQ2						_reg_tmr_b_freq_ctrl.bit._TBQ2
#define UNSUSED0					_reg_tmr_b_freq_ctrl.bit._UNSUSED0
#define TBW0						_reg_tmr_b_freq_ctrl.bit._TBW0
#define TBW1						_reg_tmr_b_freq_ctrl.bit._TBW1
#define TBW2						_reg_tmr_b_freq_ctrl.bit._TBW2
#define UNUSED1						_reg_tmr_b_freq_ctrl.bit._UNSUSED1

volatile REG_TMR_B_REG_STR			_reg_tmr_b_reg;		  REGISTER TIMER B
#define REG_TMR_B_REG				_reg_tmr_b_reg.byte
*/

/*

typedef struct
{
	unsigned char 	Year;
	unsigned char 	Mounth;
	unsigned char 	Day;
	unsigned char 	DayOfWeek;
}t_sDate;

typedef struct
{
	unsigned char 	Hour;
	unsigned char 	Minute;
	unsigned char 	Second;
}t_sTime;

typedef struct
{
	unsigned char 	Hour;
	unsigned char 	Minute;
}t_sAlarm;

typedef struct
{
	t_sDate  		sDate;
    t_sTime 		sTime;
    t_sAlarm 		sAlarm;
}t_sRTC_Data;
*/


volatile uint8_t gu8Cntrl1Reg;
volatile uint8_t gbRTCInt;
volatile uint16_t gPulseTimeOut;

uint8_t rtcInit(void);
void rtcReadReg(uint8_t u8Reg, uint8_t u8LenData, uint8_t* pu8Data);
void rtcWriteReg(uint8_t u8Reg, uint8_t u8LenData, uint8_t* pu8Data);

int rtcGetUpdate(char str[]);
void rtcSleepConfig(void);

uint8_t u8SwapBits(uint8_t u8DataIn);
//t_sRTC_Data* RTCGetData(void);
void rtcSetData_1(void);
void rtcSetData_2(void);
uint8_t u8Hex2Bcd(uint8_t u8HexValue);
uint8_t u8Bcd2Hex(uint8_t u8BcdValue);

uint8_t rtcCrystalFrequencyTest(void);


#endif /* RTC_S35390A_H_ */
