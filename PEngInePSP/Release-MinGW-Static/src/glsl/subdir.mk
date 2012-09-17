################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/glsl/glsl_profile.cpp \
../src/glsl/glsl_program.cpp \
../src/glsl/glsl_target.cpp \
../src/glsl/glsl_viewport.cpp 

OBJS += \
./src/glsl/glsl_profile.o \
./src/glsl/glsl_program.o \
./src/glsl/glsl_target.o \
./src/glsl/glsl_viewport.o 

CPP_DEPS += \
./src/glsl/glsl_profile.d \
./src/glsl/glsl_program.d \
./src/glsl/glsl_target.d \
./src/glsl/glsl_viewport.d 


# Each subdirectory must supply rules for building sources it contributes
src/glsl/%.o: ../src/glsl/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D_HAS_SDL_ -D_HAS_SDL_IMAGE_ -D_HAS_SDL_TTF_ -D_HAS_WX_WIDGETS_ -D_HAS_GLSL_ -D_HAS_OPENGL_ -DN_HAS_EGL1_ -DN_HAS_EGL2_ -I"C:\Users\Jurgen\Workspace\PEngIne\PEngInePSP\src" -I"C:\Users\Jurgen\Workspace\PEngIne\PEngIneCore\src" -I"C:\MinGW\include" -I"C:\MinGW\msys\1.0\local\include" -I"C:\MinGW\msys\1.0\include" -I"C:\MinGW\msys\1.0\include\SDL" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"  `wx-config --cxxflags`
	@echo 'Finished building: $<'
	@echo ' '


