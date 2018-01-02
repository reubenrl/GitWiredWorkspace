################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../myAvrlib/a2d.c \
../myAvrlib/buffer.c \
../myAvrlib/cmdline.c \
../myAvrlib/extint.c \
../myAvrlib/i2c.c \
../myAvrlib/i2ceeprom.c \
../myAvrlib/timer.c \
../myAvrlib/uart2.c \
../myAvrlib/vt100.c 

OBJS += \
./myAvrlib/a2d.o \
./myAvrlib/buffer.o \
./myAvrlib/cmdline.o \
./myAvrlib/extint.o \
./myAvrlib/i2c.o \
./myAvrlib/i2ceeprom.o \
./myAvrlib/timer.o \
./myAvrlib/uart2.o \
./myAvrlib/vt100.o 

C_DEPS += \
./myAvrlib/a2d.d \
./myAvrlib/buffer.d \
./myAvrlib/cmdline.d \
./myAvrlib/extint.d \
./myAvrlib/i2c.d \
./myAvrlib/i2ceeprom.d \
./myAvrlib/timer.d \
./myAvrlib/uart2.d \
./myAvrlib/vt100.d 


# Each subdirectory must supply rules for building sources it contributes
myAvrlib/%.o: ../myAvrlib/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"D:\GitWiredWorkspace\WirelessDataLogger\common\include" -I"D:\GitWiredWorkspace\WirelessDataLogger\WBRD200-VER1\src\config" -I"D:\GitWiredWorkspace\WirelessDataLogger\BoardSupport\inc" -I"D:\GitWiredWorkspace\WirelessDataLogger\myAvrlib\config" -I"D:\GitWiredWorkspace\WirelessDataLogger\myAvrlib\include" -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega644p -DF_CPU=3686400UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


