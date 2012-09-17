################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/tcl/tcl_engine.cpp \
../src/tcl/tcl_script_object.cpp \
../src/tcl/tcl_shell.cpp 

OBJS += \
./src/tcl/tcl_engine.o \
./src/tcl/tcl_script_object.o \
./src/tcl/tcl_shell.o 

CPP_DEPS += \
./src/tcl/tcl_engine.d \
./src/tcl/tcl_script_object.d \
./src/tcl/tcl_shell.d 


# Each subdirectory must supply rules for building sources it contributes
src/tcl/%.o: ../src/tcl/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D_NO_TTF_RENDER_WORKAROUND -D__LINUX__ -I"C:\Users\Jurgen\Workspace\PEngIne\PEngIneCore\src" -I"C:\Users\Jurgen\Workspace\PEngIne\PEngInePSP\src" -I/usr/include/SDL -I/usr/include/tcl8.5 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<" `freetype-config --cflags`
	@echo 'Finished building: $<'
	@echo ' '


