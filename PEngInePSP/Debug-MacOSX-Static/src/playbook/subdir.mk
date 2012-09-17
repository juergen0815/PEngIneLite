################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/playbook/pb_egl1_render_profile.cpp \
../src/playbook/pb_egl2_render_profile.cpp \
../src/playbook/pb_evt_profile.cpp 

OBJS += \
./src/playbook/pb_egl1_render_profile.o \
./src/playbook/pb_egl2_render_profile.o \
./src/playbook/pb_evt_profile.o 

CPP_DEPS += \
./src/playbook/pb_egl1_render_profile.d \
./src/playbook/pb_egl2_render_profile.d \
./src/playbook/pb_evt_profile.d 


# Each subdirectory must supply rules for building sources it contributes
src/playbook/%.o: ../src/playbook/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/Users/jurgen/Workspace/PEngIne/PEngInePSP/src" -I"/Users/jurgen/Workspace/PEngIne/PEngIneCore/src" -Ic:/mingw/include/SDL -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


