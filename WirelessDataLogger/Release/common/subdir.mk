################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../common/ShortMessageService.c \
../common/battery.c \
../common/epoch.c \
../common/epromAT24xx.c \
../common/i2cMax11645.c \
../common/leds.c \
../common/measure_adc.c \
../common/modem.c \
../common/nmea.c \
../common/rtc_s35390a.c \
../common/satellite.c \
../common/testing.c \
../common/timer_sys.c \
../common/uart1_split.c \
../common/uartDbgPrint.c \
../common/wireless.c \
../common/xE910.c 

OBJS += \
./common/ShortMessageService.o \
./common/battery.o \
./common/epoch.o \
./common/epromAT24xx.o \
./common/i2cMax11645.o \
./common/leds.o \
./common/measure_adc.o \
./common/modem.o \
./common/nmea.o \
./common/rtc_s35390a.o \
./common/satellite.o \
./common/testing.o \
./common/timer_sys.o \
./common/uart1_split.o \
./common/uartDbgPrint.o \
./common/wireless.o \
./common/xE910.o 

C_DEPS += \
./common/ShortMessageService.d \
./common/battery.d \
./common/epoch.d \
./common/epromAT24xx.d \
./common/i2cMax11645.d \
./common/leds.d \
./common/measure_adc.d \
./common/modem.d \
./common/nmea.d \
./common/rtc_s35390a.d \
./common/satellite.d \
./common/testing.d \
./common/timer_sys.d \
./common/uart1_split.d \
./common/uartDbgPrint.d \
./common/wireless.d \
./common/xE910.d 


# Each subdirectory must supply rules for building sources it contributes
common/%.o: ../common/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"C:\Users\phytech\Dropbox\eclipse-neon\workspace\atmel\phytologger\WirelessDataLogger\common\include" -I"C:\Users\phytech\Dropbox\eclipse-neon\workspace\atmel\phytologger\WirelessDataLogger\WBRD200-VER1\src\config" -I"C:\Users\phytech\Dropbox\eclipse-neon\workspace\atmel\phytologger\WirelessDataLogger\BoardSupport\inc" -I"C:\Users\phytech\Dropbox\eclipse-neon\workspace\atmel\phytologger\WirelessDataLogger\myAvrlib\config" -I"C:\Users\phytech\Dropbox\eclipse-neon\workspace\atmel\phytologger\WirelessDataLogger\myAvrlib\include" -Wall -O2 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega644p -DF_CPU=3686400UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


