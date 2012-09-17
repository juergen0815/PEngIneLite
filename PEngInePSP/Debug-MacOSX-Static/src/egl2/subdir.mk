################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/egl2/egl2_profile.cpp \
../src/egl2/egl2_target.cpp \
../src/egl2/egl2_viewport.cpp 

OBJS += \
./src/egl2/egl2_profile.o \
./src/egl2/egl2_target.o \
./src/egl2/egl2_viewport.o 

CPP_DEPS += \
./src/egl2/egl2_profile.d \
./src/egl2/egl2_target.d \
./src/egl2/egl2_viewport.d 


# Each subdirectory must supply rules for building sources it contributes
src/egl2/%.o: ../src/egl2/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/Users/jurgen/Workspace/PEngIne/PEngInePSP/src" -I"/Users/jurgen/Workspace/PEngIne/PEngIneCore/src" -Ic:/mingw/include/SDL -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


