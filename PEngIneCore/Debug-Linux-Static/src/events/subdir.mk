################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/events/event.cpp \
../src/events/event_manager.cpp \
../src/events/event_profile.cpp \
../src/events/event_queue.cpp 

OBJS += \
./src/events/event.o \
./src/events/event_manager.o \
./src/events/event_profile.o \
./src/events/event_queue.o 

CPP_DEPS += \
./src/events/event.d \
./src/events/event_manager.d \
./src/events/event_profile.d \
./src/events/event_queue.d 


# Each subdirectory must supply rules for building sources it contributes
src/events/%.o: ../src/events/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D_NO_TTF_RENDER_WORKAROUND -D__LINUX__ -I"C:\Users\Jurgen\Workspace\PEngIne\PEngIneCore\src" -I"C:\Users\Jurgen\Workspace\PEngIne\PEngInePSP\src" -I/usr/include/SDL -I/usr/include/tcl8.5 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<" `freetype-config --cflags`
	@echo 'Finished building: $<'
	@echo ' '


