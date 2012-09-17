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
	gcc -D_NO_TTF_RENDER_WORKAROUND -D__LINUX__ -I"C:\Users\Jurgen\Workspace\PEngIne\PEngIneCore\src" -I"C:\Users\Jurgen\Workspace\PEngIne\PEngInePSP\src" -I/usr/include/SDL -I/usr/include/tcl8.5 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<" `freetype-config --cflags`
	@echo 'Finished building: $<'
	@echo ' '


