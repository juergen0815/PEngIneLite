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
	g++ -D_HAS_SDL_ -D_HAS_SDL_IMAGE_ -D_HAS_SDL_TTF_ -D_HAS_WX_WIDGETS_ -D_HAS_OPENGL_ -DN_HAS_EGL1_ -DN_HAS_EGL2_ -U_HAS_OPENGL_ -I"C:\Users\Jurgen\Workspace\PEngIne\PEngInePSP\src" -I"C:\Users\Jurgen\Workspace\PEngIne\PEngIneCore\src" -I"C:\MinGW\include" -I"C:\MinGW\msys\1.0\local\include" -I"C:\MinGW\msys\1.0\include" -I"C:\MinGW\msys\1.0\include\SDL" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"  `wx-config --cxxflags`
	@echo 'Finished building: $<'
	@echo ' '


