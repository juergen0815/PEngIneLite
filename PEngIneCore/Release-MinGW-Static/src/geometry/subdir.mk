################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/geometry/buffered_mesh.cpp \
../src/geometry/mesh.cpp \
../src/geometry/mesh_target.cpp 

OBJS += \
./src/geometry/buffered_mesh.o \
./src/geometry/mesh.o \
./src/geometry/mesh_target.o 

CPP_DEPS += \
./src/geometry/buffered_mesh.d \
./src/geometry/mesh.d \
./src/geometry/mesh_target.d 


# Each subdirectory must supply rules for building sources it contributes
src/geometry/%.o: ../src/geometry/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D_HAS_OPENGL_ -D_HAS_LUA_BIND_ -I"C:\Users\Jurgen\Workspace\PEngIne\PEngIneCore\src" -I"C:\Users\Jurgen\Workspace\PEngIne\PEngInePSP\src" -I"C:\Program Files\Lua\5.1/include" -I"C:\MinGW\include" -I"C:\MinGW\msys\1.0\include" -I"C:\MinGW\msys\1.0\include\SDL" -I"C:\MinGW\msys\1.0\include\freetype2" -I"C:\MinGW\msys\1.0\local\include" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


