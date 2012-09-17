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
	g++ -D_HAS_SDL_ -D_HAS_SDL_IMAGE_ -D_HAS_SDL_TTF_ -D_HAS_WX_WIDGETS_ -D_HAS_OPENGL_ -DN_HAS_EGL1_ -DN_HAS_EGL2_ -U_HAS_OPENGL_ -I"C:\My Documents\workspace\pe\PEngInePSP\src" -I"C:\My Documents\workspace\pe\PEngIneCore\src" -I"C:\MinGW\include" -I"C:\MinGW\msys\1.0\local\include" -I"C:\MinGW\msys\1.0\include" -I"C:\MinGW\msys\1.0\include\SDL" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"  `wx-config --cxxflags`
	@echo 'Finished building: $<'
	@echo ' '


