################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/render/color.cpp \
../src/render/drawable.cpp \
../src/render/logo-32.cpp \
../src/render/rectangle.cpp \
../src/render/render_container.cpp \
../src/render/render_manager.cpp \
../src/render/render_node.cpp \
../src/render/render_param.cpp \
../src/render/render_profile.cpp \
../src/render/render_surface.cpp \
../src/render/render_target.cpp \
../src/render/tex_surface.cpp 

OBJS += \
./src/render/color.o \
./src/render/drawable.o \
./src/render/logo-32.o \
./src/render/rectangle.o \
./src/render/render_container.o \
./src/render/render_manager.o \
./src/render/render_node.o \
./src/render/render_param.o \
./src/render/render_profile.o \
./src/render/render_surface.o \
./src/render/render_target.o \
./src/render/tex_surface.o 

CPP_DEPS += \
./src/render/color.d \
./src/render/drawable.d \
./src/render/logo-32.d \
./src/render/rectangle.d \
./src/render/render_container.d \
./src/render/render_manager.d \
./src/render/render_node.d \
./src/render/render_param.d \
./src/render/render_profile.d \
./src/render/render_surface.d \
./src/render/render_target.d \
./src/render/tex_surface.d 


# Each subdirectory must supply rules for building sources it contributes
src/render/%.o: ../src/render/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D_NO_TTF_RENDER_WORKAROUND -D__LINUX__ -I"C:\Users\Jurgen\Workspace\PEngIne\PEngIneCore\src" -I"C:\Users\Jurgen\Workspace\PEngIne\PEngInePSP\src" -I/usr/include/SDL -I/usr/include/tcl8.5 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<" `freetype-config --cflags`
	@echo 'Finished building: $<'
	@echo ' '


