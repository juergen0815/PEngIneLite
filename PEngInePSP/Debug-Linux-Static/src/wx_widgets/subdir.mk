################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/wx_widgets/wx_evt_profile.cpp \
../src/wx_widgets/wx_render_canvas.cpp \
../src/wx_widgets/wx_render_profile.cpp \
../src/wx_widgets/wx_surface.cpp 

OBJS += \
./src/wx_widgets/wx_evt_profile.o \
./src/wx_widgets/wx_render_canvas.o \
./src/wx_widgets/wx_render_profile.o \
./src/wx_widgets/wx_surface.o 

CPP_DEPS += \
./src/wx_widgets/wx_evt_profile.d \
./src/wx_widgets/wx_render_canvas.d \
./src/wx_widgets/wx_render_profile.d \
./src/wx_widgets/wx_surface.d 


# Each subdirectory must supply rules for building sources it contributes
src/wx_widgets/%.o: ../src/wx_widgets/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/jurgen/Workspace/PEngIne/PEngInePSP/src" -I"/home/jurgen/Workspace/PEngIne/PEngIneCore/src" -I/usr/include/SDL -I/usr/include/tcl8.5 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"  `wx-config --cxxflags`
	@echo 'Finished building: $<'
	@echo ' '


