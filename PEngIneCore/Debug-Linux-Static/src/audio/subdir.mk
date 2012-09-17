################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/audio/audio_manager.cpp \
../src/audio/audio_profile.cpp \
../src/audio/sound.cpp \
../src/audio/sound_node.cpp \
../src/audio/sound_surface.cpp \
../src/audio/sound_target.cpp 

OBJS += \
./src/audio/audio_manager.o \
./src/audio/audio_profile.o \
./src/audio/sound.o \
./src/audio/sound_node.o \
./src/audio/sound_surface.o \
./src/audio/sound_target.o 

CPP_DEPS += \
./src/audio/audio_manager.d \
./src/audio/audio_profile.d \
./src/audio/sound.d \
./src/audio/sound_node.d \
./src/audio/sound_surface.d \
./src/audio/sound_target.d 


# Each subdirectory must supply rules for building sources it contributes
src/audio/%.o: ../src/audio/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D_NO_TTF_RENDER_WORKAROUND -D__LINUX__ -I"C:\Users\Jurgen\Workspace\PEngIne\PEngIneCore\src" -I"C:\Users\Jurgen\Workspace\PEngIne\PEngInePSP\src" -I/usr/include/SDL -I/usr/include/tcl8.5 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<" `freetype-config --cflags`
	@echo 'Finished building: $<'
	@echo ' '


