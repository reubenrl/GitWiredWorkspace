################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../WBRD200-VER1/src/wbrd200TestMain.c 

OBJS += \
./WBRD200-VER1/src/wbrd200TestMain.o 

C_DEPS += \
./WBRD200-VER1/src/wbrd200TestMain.d 


# Each subdirectory must supply rules for building sources it contributes
WBRD200-VER1/src/%.o: ../WBRD200-VER1/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"C:\Users\phytech\Dropbox\eclipse-neon\workspace\atmel\phytologger\WirelessDataLogger\common\include" -I"C:\Users\phytech\Dropbox\eclipse-neon\workspace\atmel\phytologger\WirelessDataLogger\WBRD200-VER1\src\config" -I"C:\Users\phytech\Dropbox\eclipse-neon\workspace\atmel\phytologger\WirelessDataLogger\BoardSupport\inc" -I"C:\Users\phytech\Dropbox\eclipse-neon\workspace\atmel\phytologger\WirelessDataLogger\myAvrlib\config" -I"C:\Users\phytech\Dropbox\eclipse-neon\workspace\atmel\phytologger\WirelessDataLogger\myAvrlib\include" -Wall -O2 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega644p -DF_CPU=3686400UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


