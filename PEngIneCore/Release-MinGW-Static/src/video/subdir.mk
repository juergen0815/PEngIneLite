################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/video/blitter.cpp \
../src/video/render_tools.cpp 

OBJS += \
./src/video/blitter.o \
./src/video/render_tools.o 

CPP_DEPS += \
./src/video/blitter.d \
./src/video/render_tools.d 


# Each subdirectory must supply rules for building sources it contributes
src/video/%.o: ../src/video/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D_HAS_OPENGL_ -D_HAS_LUA_BIND_ -I"C:\Users\Jurgen\Workspace\PEngIne\PEngIneCore\src" -I"C:\Users\Jurgen\Workspace\PEngIne\PEngInePSP\src" -I"C:\Program Files\Lua\5.1/include" -I"C:\MinGW\include" -I"C:\MinGW\msys\1.0\include" -I"C:\MinGW\msys\1.0\include\SDL" -I"C:\MinGW\msys\1.0\include\freetype2" -I"C:\MinGW\msys\1.0\local\include" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


