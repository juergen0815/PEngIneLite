################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/wx/wx_evt_profile.cpp \
../src/wx/wx_render_canvas.cpp \
../src/wx/wx_render_profile.cpp \
../src/wx/wx_surface.cpp 

OBJS += \
./src/wx/wx_evt_profile.o \
./src/wx/wx_render_canvas.o \
./src/wx/wx_render_profile.o \
./src/wx/wx_surface.o 

CPP_DEPS += \
./src/wx/wx_evt_profile.d \
./src/wx/wx_render_canvas.d \
./src/wx/wx_render_profile.d \
./src/wx/wx_surface.d 


# Each subdirectory must supply rules for building sources it contributes
src/wx/%.o: ../src/wx/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D_HAS_SDL_ -D_HAS_SDL_IMAGE_ -D_HAS_SDL_TTF_ -D_HAS_WX_WIDGETS_ -D_HAS_GLSL_ -D_HAS_OPENGL_ -DN_HAS_EGL1_ -DN_HAS_EGL2_ -I"C:\Users\Jurgen\Workspace\PEngIneLite\PEngInePSP\src" -I"C:\Users\Jurgen\Workspace\PEngIneLite\PEngIneCore\src" -I"C:\MinGW\include" -I"C:\MinGW\msys\1.0\local\include" -I"C:\MinGW\msys\1.0\include" -I"C:\MinGW\msys\1.0\include\SDL" -I/usr/local/include/SDL -I/usr/X11/include -O0 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"  `wx-config --cxxflags`
	@echo 'Finished building: $<'
	@echo ' '


