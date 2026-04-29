################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/ReiwaStd/crc16.c \
../Core/ReiwaStd/fifo.c \
../Core/ReiwaStd/key.c \
../Core/ReiwaStd/strcnv.c \
../Core/ReiwaStd/uart_drvsmpl.c 

OBJS += \
./Core/ReiwaStd/crc16.o \
./Core/ReiwaStd/fifo.o \
./Core/ReiwaStd/key.o \
./Core/ReiwaStd/strcnv.o \
./Core/ReiwaStd/uart_drvsmpl.o 

C_DEPS += \
./Core/ReiwaStd/crc16.d \
./Core/ReiwaStd/fifo.d \
./Core/ReiwaStd/key.d \
./Core/ReiwaStd/strcnv.d \
./Core/ReiwaStd/uart_drvsmpl.d 


# Each subdirectory must supply rules for building sources it contributes
Core/ReiwaStd/%.o Core/ReiwaStd/%.su Core/ReiwaStd/%.cyclo: ../Core/ReiwaStd/%.c Core/ReiwaStd/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -c -I../Core/Inc -I../Core/ReiwaStd -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-ReiwaStd

clean-Core-2f-ReiwaStd:
	-$(RM) ./Core/ReiwaStd/crc16.cyclo ./Core/ReiwaStd/crc16.d ./Core/ReiwaStd/crc16.o ./Core/ReiwaStd/crc16.su ./Core/ReiwaStd/fifo.cyclo ./Core/ReiwaStd/fifo.d ./Core/ReiwaStd/fifo.o ./Core/ReiwaStd/fifo.su ./Core/ReiwaStd/key.cyclo ./Core/ReiwaStd/key.d ./Core/ReiwaStd/key.o ./Core/ReiwaStd/key.su ./Core/ReiwaStd/strcnv.cyclo ./Core/ReiwaStd/strcnv.d ./Core/ReiwaStd/strcnv.o ./Core/ReiwaStd/strcnv.su ./Core/ReiwaStd/uart_drvsmpl.cyclo ./Core/ReiwaStd/uart_drvsmpl.d ./Core/ReiwaStd/uart_drvsmpl.o ./Core/ReiwaStd/uart_drvsmpl.su

.PHONY: clean-Core-2f-ReiwaStd

