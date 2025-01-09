################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/User/hts221_reg.c \
../Drivers/User/lps22hh_reg.c 

OBJS += \
./Drivers/User/hts221_reg.o \
./Drivers/User/lps22hh_reg.o 

C_DEPS += \
./Drivers/User/hts221_reg.d \
./Drivers/User/lps22hh_reg.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/User/%.o Drivers/User/%.su Drivers/User/%.cyclo: ../Drivers/User/%.c Drivers/User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F746xx -c -I../Core/Inc -I../Utilities/Fonts -I../Drivers/BSP/STM32746G-Discovery -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Drivers/User -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-User

clean-Drivers-2f-User:
	-$(RM) ./Drivers/User/hts221_reg.cyclo ./Drivers/User/hts221_reg.d ./Drivers/User/hts221_reg.o ./Drivers/User/hts221_reg.su ./Drivers/User/lps22hh_reg.cyclo ./Drivers/User/lps22hh_reg.d ./Drivers/User/lps22hh_reg.o ./Drivers/User/lps22hh_reg.su

.PHONY: clean-Drivers-2f-User

