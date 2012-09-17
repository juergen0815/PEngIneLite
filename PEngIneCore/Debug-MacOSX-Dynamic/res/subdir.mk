################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../res/logo-32.c 

OBJS += \
./res/logo-32.o 

C_DEPS += \
./res/logo-32.d 


# Each subdirectory must supply rules for building sources it contributes
res/%.o: ../res/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/local/include/SDL -I/usr/local/include/boost-1_37 -I/usr/X11/include -I"C:\Users\Jurgen\Workspace\PEngIne\PEngInePSP\src" -O0 -g3 -Wall -c -fmessage-length=0 -Wno-deprecated -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


