################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/video/blitter.cpp \
../src/video/render_tools.cpp 

OBJS += \
./src/video/blitter.o \
./src/video/render_tools.o 

CPP_DEPS += \
./src/video/blitter.d \
./src/video/render_tools.d 


# Each subdirectory must supply rules for building sources it contributes
src/video/%.o: ../src/video/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D_NO_TTF_RENDER_WORKAROUND -D__LINUX__ -I"C:\Users\Jurgen\Workspace\PEngIne\PEngIneCore\src" -I"C:\Users\Jurgen\Workspace\PEngIne\PEngInePSP\src" -I/usr/include/SDL -I/usr/include/tcl8.5 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<" `freetype-config --cflags`
	@echo 'Finished building: $<'
	@echo ' '


