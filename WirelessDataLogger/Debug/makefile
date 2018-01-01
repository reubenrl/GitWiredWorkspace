################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include myAvrlib/subdir.mk
-include common/subdir.mk
-include WBRD200-VER1/src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
WirelessDataLogger.lss \
WirelessDataLogger.lss \

FLASH_IMAGE += \
WirelessDataLogger.hex \

SIZEDUMMY += \
sizedummy \
sizedummy \

AVRDUDEDUMMY += \
avrdudedummy \


# All Target
all: WirelessDataLogger.elf secondary-outputs

# Tool invocations
WirelessDataLogger.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,WirelessDataLogger.map -mmcu=atmega644p -o "WirelessDataLogger.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

WirelessDataLogger.lss: WirelessDataLogger.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S WirelessDataLogger.elf  >"WirelessDataLogger.lss"
	@echo 'Finished building: $@'
	@echo ' '

WirelessDataLogger.hex: WirelessDataLogger.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex WirelessDataLogger.elf  "WirelessDataLogger.hex"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: WirelessDataLogger.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega644p WirelessDataLogger.elf
	@echo 'Finished building: $@'
	@echo ' '

avrdudedummy: WirelessDataLogger.elf
	@echo 'Invoking: AVRDude'
	C:\WinAVR-20100110\bin\avrdude -pm644p -cavrispmkII -Pusb -Uflash:w:WirelessDataLogger.hex:a
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(FLASH_IMAGE)$(ELFS)$(OBJS)$(ASM_DEPS)$(AVRDUDEDUMMY)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(LSS)$(C_DEPS) WirelessDataLogger.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(SIZEDUMMY) $(AVRDUDEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
