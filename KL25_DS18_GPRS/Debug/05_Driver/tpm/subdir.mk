################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../05_Driver/tpm/tpm.c 

OBJS += \
./05_Driver/tpm/tpm.o 

C_DEPS += \
./05_Driver/tpm/tpm.d 


# Each subdirectory must supply rules for building sources it contributes
05_Driver/tpm/%.o: ../05_Driver/tpm/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -I"E:/BOBO_KDS/KL25_DS18_GPRS/08_Source" -I"E:/BOBO_KDS/KL25_DS18_GPRS/07_Soft_Component" -I"E:/BOBO_KDS/KL25_DS18_GPRS/07_Soft_Component/common" -I"E:/BOBO_KDS/KL25_DS18_GPRS/07_Soft_Component/printf" -I"E:/BOBO_KDS/KL25_DS18_GPRS/07_Soft_Component/gprs" -I"E:/BOBO_KDS/KL25_DS18_GPRS/04_Linker_File" -I"E:/BOBO_KDS/KL25_DS18_GPRS/05_Driver/uart" -I"E:/BOBO_KDS/KL25_DS18_GPRS/05_Driver/tpm" -I"E:/BOBO_KDS/KL25_DS18_GPRS/06_App_Component" -I"E:/BOBO_KDS/KL25_DS18_GPRS/06_App_Component/light" -I"E:/BOBO_KDS/KL25_DS18_GPRS/06_App_Component/delay" -I"E:/BOBO_KDS/KL25_DS18_GPRS/06_App_Component/DS18B20" -I"E:/BOBO_KDS/KL25_DS18_GPRS/06_App_Component/OLED" -I"E:/BOBO_KDS/KL25_DS18_GPRS/05_Driver" -I"E:/BOBO_KDS/KL25_DS18_GPRS/03_MCU" -I"E:/BOBO_KDS/KL25_DS18_GPRS/02_CPU" -I"E:/BOBO_KDS/KL25_DS18_GPRS/01_Doc" -I"E:/BOBO_KDS/KL25_DS18_GPRS/05_Driver/gpio" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


