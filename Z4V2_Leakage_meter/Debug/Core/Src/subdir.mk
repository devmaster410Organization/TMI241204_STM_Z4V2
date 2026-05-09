################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/app_freertos.c \
../Core/Src/calc_leak.c \
../Core/Src/calc_volt.c \
../Core/Src/chlcd.c \
../Core/Src/hal_flash.c \
../Core/Src/main.c \
../Core/Src/modbus_reg.c \
../Core/Src/setup.c \
../Core/Src/stm32g4xx_hal_msp.c \
../Core/Src/stm32g4xx_hal_timebase_tim.c \
../Core/Src/stm32g4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32g4xx.c \
../Core/Src/tsk_calc.c \
../Core/Src/tsk_modbus.c \
../Core/Src/tsk_ui.c \
../Core/Src/tsk_usb.c \
../Core/Src/util.c \
../Core/Src/ver.c 

OBJS += \
./Core/Src/app_freertos.o \
./Core/Src/calc_leak.o \
./Core/Src/calc_volt.o \
./Core/Src/chlcd.o \
./Core/Src/hal_flash.o \
./Core/Src/main.o \
./Core/Src/modbus_reg.o \
./Core/Src/setup.o \
./Core/Src/stm32g4xx_hal_msp.o \
./Core/Src/stm32g4xx_hal_timebase_tim.o \
./Core/Src/stm32g4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32g4xx.o \
./Core/Src/tsk_calc.o \
./Core/Src/tsk_modbus.o \
./Core/Src/tsk_ui.o \
./Core/Src/tsk_usb.o \
./Core/Src/util.o \
./Core/Src/ver.o 

C_DEPS += \
./Core/Src/app_freertos.d \
./Core/Src/calc_leak.d \
./Core/Src/calc_volt.d \
./Core/Src/chlcd.d \
./Core/Src/hal_flash.d \
./Core/Src/main.d \
./Core/Src/modbus_reg.d \
./Core/Src/setup.d \
./Core/Src/stm32g4xx_hal_msp.d \
./Core/Src/stm32g4xx_hal_timebase_tim.d \
./Core/Src/stm32g4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32g4xx.d \
./Core/Src/tsk_calc.d \
./Core/Src/tsk_modbus.d \
./Core/Src/tsk_ui.d \
./Core/Src/tsk_usb.d \
./Core/Src/util.d \
./Core/Src/ver.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -c -I../Core/Inc -I../Core/ReiwaStd -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/app_freertos.cyclo ./Core/Src/app_freertos.d ./Core/Src/app_freertos.o ./Core/Src/app_freertos.su ./Core/Src/calc_leak.cyclo ./Core/Src/calc_leak.d ./Core/Src/calc_leak.o ./Core/Src/calc_leak.su ./Core/Src/calc_volt.cyclo ./Core/Src/calc_volt.d ./Core/Src/calc_volt.o ./Core/Src/calc_volt.su ./Core/Src/chlcd.cyclo ./Core/Src/chlcd.d ./Core/Src/chlcd.o ./Core/Src/chlcd.su ./Core/Src/hal_flash.cyclo ./Core/Src/hal_flash.d ./Core/Src/hal_flash.o ./Core/Src/hal_flash.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/modbus_reg.cyclo ./Core/Src/modbus_reg.d ./Core/Src/modbus_reg.o ./Core/Src/modbus_reg.su ./Core/Src/setup.cyclo ./Core/Src/setup.d ./Core/Src/setup.o ./Core/Src/setup.su ./Core/Src/stm32g4xx_hal_msp.cyclo ./Core/Src/stm32g4xx_hal_msp.d ./Core/Src/stm32g4xx_hal_msp.o ./Core/Src/stm32g4xx_hal_msp.su ./Core/Src/stm32g4xx_hal_timebase_tim.cyclo ./Core/Src/stm32g4xx_hal_timebase_tim.d ./Core/Src/stm32g4xx_hal_timebase_tim.o ./Core/Src/stm32g4xx_hal_timebase_tim.su ./Core/Src/stm32g4xx_it.cyclo ./Core/Src/stm32g4xx_it.d ./Core/Src/stm32g4xx_it.o ./Core/Src/stm32g4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32g4xx.cyclo ./Core/Src/system_stm32g4xx.d ./Core/Src/system_stm32g4xx.o ./Core/Src/system_stm32g4xx.su ./Core/Src/tsk_calc.cyclo ./Core/Src/tsk_calc.d ./Core/Src/tsk_calc.o ./Core/Src/tsk_calc.su ./Core/Src/tsk_modbus.cyclo ./Core/Src/tsk_modbus.d ./Core/Src/tsk_modbus.o ./Core/Src/tsk_modbus.su ./Core/Src/tsk_ui.cyclo ./Core/Src/tsk_ui.d ./Core/Src/tsk_ui.o ./Core/Src/tsk_ui.su ./Core/Src/tsk_usb.cyclo ./Core/Src/tsk_usb.d ./Core/Src/tsk_usb.o ./Core/Src/tsk_usb.su ./Core/Src/util.cyclo ./Core/Src/util.d ./Core/Src/util.o ./Core/Src/util.su ./Core/Src/ver.cyclo ./Core/Src/ver.d ./Core/Src/ver.o ./Core/Src/ver.su

.PHONY: clean-Core-2f-Src

