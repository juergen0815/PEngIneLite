################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/opengl/gl_profile.cpp \
../src/opengl/gl_target.cpp \
../src/opengl/gl_viewport.cpp 

OBJS += \
./src/opengl/gl_profile.o \
./src/opengl/gl_target.o \
./src/opengl/gl_viewport.o 

CPP_DEPS += \
./src/opengl/gl_profile.d \
./src/opengl/gl_target.d \
./src/opengl/gl_viewport.d 


# Each subdirectory must supply rules for building sources it contributes
src/opengl/%.o: ../src/opengl/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D_HAS_SDL_ -D_HAS_SDL_IMAGE_ -D_HAS_SDL_TTF_ -D_HAS_WX_WIDGETS_ -D_HAS_GLSL_ -D_HAS_OPENGL_ -DN_HAS_EGL1_ -DN_HAS_EGL2_ -I"C:\Users\Jurgen\Workspace\PEngIne\PEngInePSP\src" -I"C:\Users\Jurgen\Workspace\PEngIne\PEngIneCore\src" -I"C:\MinGW\include" -I"C:\MinGW\msys\1.0\local\include" -I"C:\MinGW\msys\1.0\include" -I"C:\MinGW\msys\1.0\include\SDL" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"  `wx-config --cxxflags`
	@echo 'Finished building: $<'
	@echo ' '


