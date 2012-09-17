################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/playbook/pb_egl1_render_profile.cpp \
../src/playbook/pb_evt_profile.cpp 

OBJS += \
./src/playbook/pb_egl1_render_profile.o \
./src/playbook/pb_evt_profile.o 

CPP_DEPS += \
./src/playbook/pb_egl1_render_profile.d \
./src/playbook/pb_evt_profile.d 


# Each subdirectory must supply rules for building sources it contributes
src/playbook/%.o: ../src/playbook/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D_HAS_SDL_ -D_HAS_SDL_IMAGE_ -D_HAS_SDL_TTF_ -D_HAS_WX_WIDGETS_ -D_HAS_OPENGL_ -DN_HAS_EGL1_ -DN_HAS_EGL2_ -U_HAS_OPENGL_ -I"C:\My Documents\workspace\pe\PEngInePSP\src" -I"C:\My Documents\workspace\pe\PEngIneCore\src" -I"C:\MinGW\include" -I"C:\MinGW\msys\1.0\local\include" -I"C:\MinGW\msys\1.0\include" -I"C:\MinGW\msys\1.0\include\SDL" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"  `wx-config --cxxflags`
	@echo 'Finished building: $<'
	@echo ' '


