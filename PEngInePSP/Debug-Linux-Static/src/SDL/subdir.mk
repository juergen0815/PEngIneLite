################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/SDL/sdl_color.cpp \
../src/SDL/sdl_event_profile.cpp \
../src/SDL/sdl_render_profile.cpp \
../src/SDL/sdl_render_target2D.cpp \
../src/SDL/sdl_surface.cpp \
../src/SDL/sdl_video_image.cpp \
../src/SDL/sdl_video_image_sequence.cpp \
../src/SDL/sdl_xdnd_handler.cpp 

OBJS += \
./src/SDL/sdl_color.o \
./src/SDL/sdl_event_profile.o \
./src/SDL/sdl_render_profile.o \
./src/SDL/sdl_render_target2D.o \
./src/SDL/sdl_surface.o \
./src/SDL/sdl_video_image.o \
./src/SDL/sdl_video_image_sequence.o \
./src/SDL/sdl_xdnd_handler.o 

CPP_DEPS += \
./src/SDL/sdl_color.d \
./src/SDL/sdl_event_profile.d \
./src/SDL/sdl_render_profile.d \
./src/SDL/sdl_render_target2D.d \
./src/SDL/sdl_surface.d \
./src/SDL/sdl_video_image.d \
./src/SDL/sdl_video_image_sequence.d \
./src/SDL/sdl_xdnd_handler.d 


# Each subdirectory must supply rules for building sources it contributes
src/SDL/%.o: ../src/SDL/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/jurgen/Workspace/PEngIne/PEngInePSP/src" -I"/home/jurgen/Workspace/PEngIne/PEngIneCore/src" -I/usr/include/SDL -I/usr/include/tcl8.5 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"  `wx-config --cxxflags`
	@echo 'Finished building: $<'
	@echo ' '


