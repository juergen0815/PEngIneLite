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
	g++ -D_HAS_SDL_ -D_HAS_SDL_IMAGE_ -D_HAS_SDL_TTF_ -D_HAS_WX_WIDGETS_ -D_HAS_GLSL_ -D_HAS_OPENGL_ -DN_HAS_EGL1_ -DN_HAS_EGL2_ -I"C:\Users\Jurgen\Workspace\PEngIneLite\PEngInePSP\src" -I"C:\Users\Jurgen\Workspace\PEngIneLite\PEngIneCore\src" -I"C:\MinGW\include" -I"C:\MinGW\msys\1.0\local\include" -I"C:\MinGW\msys\1.0\include" -I"C:\MinGW\msys\1.0\include\SDL" -I/usr/local/include/SDL -I/usr/X11/include -O0 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"  `wx-config --cxxflags`
	@echo 'Finished building: $<'
	@echo ' '


