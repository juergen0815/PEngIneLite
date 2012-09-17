################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/xml/xml_aux.cpp \
../src/xml/xml_data_array.cpp \
../src/xml/xml_element.cpp \
../src/xml/xml_node.cpp \
../src/xml/xml_object_factory.cpp \
../src/xml/xml_parser.cpp 

OBJS += \
./src/xml/xml_aux.o \
./src/xml/xml_data_array.o \
./src/xml/xml_element.o \
./src/xml/xml_node.o \
./src/xml/xml_object_factory.o \
./src/xml/xml_parser.o 

CPP_DEPS += \
./src/xml/xml_aux.d \
./src/xml/xml_data_array.d \
./src/xml/xml_element.d \
./src/xml/xml_node.d \
./src/xml/xml_object_factory.d \
./src/xml/xml_parser.d 


# Each subdirectory must supply rules for building sources it contributes
src/xml/%.o: ../src/xml/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D_HAS_OPENGL_ -D_HAS_LUA_BIND_ -I"C:\Users\Jurgen\Workspace\PEngIne\PEngIneCore\src" -I"C:\Users\Jurgen\Workspace\PEngIne\PEngInePSP\src" -I"C:\Program Files\Lua\5.1/include" -I"C:\MinGW\include" -I"C:\MinGW\msys\1.0\include" -I"C:\MinGW\msys\1.0\include\SDL" -I"C:\MinGW\msys\1.0\include\freetype2" -I"C:\MinGW\msys\1.0\local\include" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


