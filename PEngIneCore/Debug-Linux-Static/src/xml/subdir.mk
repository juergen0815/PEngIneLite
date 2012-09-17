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
	g++ -D_NO_TTF_RENDER_WORKAROUND -D__LINUX__ -I"C:\Users\Jurgen\Workspace\PEngIne\PEngIneCore\src" -I"C:\Users\Jurgen\Workspace\PEngIne\PEngInePSP\src" -I/usr/include/SDL -I/usr/include/tcl8.5 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<" `freetype-config --cflags`
	@echo 'Finished building: $<'
	@echo ' '


