################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/egl1/egl1_profile.cpp \
../src/egl1/egl1_target.cpp \
../src/egl1/egl1_viewport.cpp 

OBJS += \
./src/egl1/egl1_profile.o \
./src/egl1/egl1_target.o \
./src/egl1/egl1_viewport.o 

CPP_DEPS += \
./src/egl1/egl1_profile.d \
./src/egl1/egl1_target.d \
./src/egl1/egl1_viewport.d 


# Each subdirectory must supply rules for building sources it contributes
src/egl1/%.o: ../src/egl1/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/Users/jurgen/Workspace/PEngIne/PEngInePSP/src" -I"/Users/jurgen/Workspace/PEngIne/PEngIneCore/src" -Ic:/mingw/include/SDL -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


