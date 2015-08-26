################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/blink_bin.c \
../Sources/bsp.c \
../Sources/main.c \
../Sources/swd.c 

OBJS += \
./Sources/blink_bin.o \
./Sources/bsp.o \
./Sources/main.o \
./Sources/swd.o 

C_DEPS += \
./Sources/blink_bin.d \
./Sources/bsp.d \
./Sources/main.d \
./Sources/swd.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/%.o: ../Sources/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -O3 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -D"CPU_MKL03Z32VFK4" -I"../Sources" -I"../Project_Settings/Startup_Code" -I"../SDK/platform/CMSIS/Include" -I"../SDK/platform/devices" -I"../SDK/platform/devices/MKL03Z4/include" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


