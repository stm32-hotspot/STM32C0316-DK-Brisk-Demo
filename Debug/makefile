################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include IO_Drivers/subdir.mk
-include Drivers/STM32C0xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include Brisk/subdir.mk
-include B_RS485_DCDC/subdir.mk
-include B_OPTO_INERT/subdir.mk
-include B_HMI_LCD16x2/subdir.mk
-include B_FAN_OUT2/subdir.mk
-include B_ANALOG_ENV/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
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

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := STM32C0316-DK-DEMO-BRISK
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
STM32C0316-DK-DEMO-BRISK.elf \

MAP_FILES += \
STM32C0316-DK-DEMO-BRISK.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
STM32C0316-DK-DEMO-BRISK.list \


# All Target
all: main-build

# Main-build Target
main-build: STM32C0316-DK-DEMO-BRISK.elf secondary-outputs

# Tool invocations
STM32C0316-DK-DEMO-BRISK.elf STM32C0316-DK-DEMO-BRISK.map: $(OBJS) $(USER_OBJS) C:\github\hotspot\STM32C0316-DK-Brisk-Demo\STM32C031C6TX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "STM32C0316-DK-DEMO-BRISK.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m0plus -T"C:\github\hotspot\STM32C0316-DK-Brisk-Demo\STM32C031C6TX_FLASH.ld" --specs=nosys.specs -Wl,-Map="STM32C0316-DK-DEMO-BRISK.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

STM32C0316-DK-DEMO-BRISK.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "STM32C0316-DK-DEMO-BRISK.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) STM32C0316-DK-DEMO-BRISK.elf STM32C0316-DK-DEMO-BRISK.list STM32C0316-DK-DEMO-BRISK.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
