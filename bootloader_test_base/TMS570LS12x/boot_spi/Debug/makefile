################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/Program Files (x86)/Texas Instruments/TI ARM Code Generation Tools 5.1.0

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./system.obj" \
"./sys_startup.obj" \
"./sys_phantom.obj" \
"./sys_intvecs.obj" \
"./sys_core.obj" \
"./sw_hw_ver.obj" \
"./sci_common.obj" \
"./notification.obj" \
"./hw_spi.obj" \
"./hw_sci.obj" \
"./hw_pinmux.obj" \
"./hw_gio.obj" \
"./hw_esm.obj" \
"./bl_ymodem.obj" \
"./bl_uart.obj" \
"./bl_spi_packet.obj" \
"./bl_spi.obj" \
"./bl_main.obj" \
"./bl_led_demo.obj" \
"./bl_flash.obj" \
"./bl_dcan.obj" \
"./bl_check.obj" \
"./Fapi_UserDefinedFunctions.obj" \
-l"C:/Users/a0324020/workspace_v5_4/SafetyMCU_Bootloader/TMS570LS31x/boot_uart/../../lib/F021_API_CortexR4_BE.lib" \
-l"rtsv7R4_T_be_eabi.lib" \
"C:/Users/a0324020/workspace_v5_4/SafetyMCU_Bootloader/TMS570LS12x/bl_link.cmd" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: boot_spi.out

# Tool invocations
boot_spi.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Linker'
	"C:/Program Files (x86)/Texas Instruments/TI ARM Code Generation Tools 5.1.0/bin/armcl" -mv7R4 --code_state=32 --abi=eabi -g --diag_warning=225 --display_error_number --enum_type=packed -z -m"boot_spi.map" -i"C:/Program Files (x86)/Texas Instruments/TI ARM Code Generation Tools 5.1.0/lib" -i"C:/Program Files (x86)/Texas Instruments/TI ARM Code Generation Tools 5.1.0/include" --reread_libs --define=TMS570LS31 --warn_sections --display_error_number --rom_model --be32 -o "boot_spi.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(TMS470_EXECUTABLE_OUTPUTS__QUOTED) "boot_spi.out"
	-$(RM) "Fapi_UserDefinedFunctions.pp" "bl_check.pp" "bl_dcan.pp" "bl_flash.pp" "bl_led_demo.pp" "bl_main.pp" "bl_spi.pp" "bl_spi_packet.pp" "bl_uart.pp" "bl_ymodem.pp" "hw_esm.pp" "hw_gio.pp" "hw_pinmux.pp" "hw_sci.pp" "hw_spi.pp" "notification.pp" "sci_common.pp" "sw_hw_ver.pp" "sys_phantom.pp" "sys_startup.pp" "system.pp" 
	-$(RM) "Fapi_UserDefinedFunctions.obj" "bl_check.obj" "bl_dcan.obj" "bl_flash.obj" "bl_led_demo.obj" "bl_main.obj" "bl_spi.obj" "bl_spi_packet.obj" "bl_uart.obj" "bl_ymodem.obj" "hw_esm.obj" "hw_gio.obj" "hw_pinmux.obj" "hw_sci.obj" "hw_spi.obj" "notification.obj" "sci_common.obj" "sw_hw_ver.obj" "sys_core.obj" "sys_intvecs.obj" "sys_phantom.obj" "sys_startup.obj" "system.obj" 
	-$(RM) "sys_core.pp" "sys_intvecs.pp" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
