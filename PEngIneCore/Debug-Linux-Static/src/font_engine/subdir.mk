################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/font_engine/FT_font_asset.cpp \
../src/font_engine/FT_font_factory.cpp \
../src/font_engine/bm_font_asset.cpp \
../src/font_engine/bm_font_factory.cpp \
../src/font_engine/font_asset.cpp \
../src/font_engine/font_manager.cpp \
../src/font_engine/font_tools.cpp \
../src/font_engine/sfont_asset.cpp \
../src/font_engine/tt_font_asset.cpp \
../src/font_engine/tt_font_factory.cpp 

OBJS += \
./src/font_engine/FT_font_asset.o \
./src/font_engine/FT_font_factory.o \
./src/font_engine/bm_font_asset.o \
./src/font_engine/bm_font_factory.o \
./src/font_engine/font_asset.o \
./src/font_engine/font_manager.o \
./src/font_engine/font_tools.o \
./src/font_engine/sfont_asset.o \
./src/font_engine/tt_font_asset.o \
./src/font_engine/tt_font_factory.o 

CPP_DEPS += \
./src/font_engine/FT_font_asset.d \
./src/font_engine/FT_font_factory.d \
./src/font_engine/bm_font_asset.d \
./src/font_engine/bm_font_factory.d \
./src/font_engine/font_asset.d \
./src/font_engine/font_manager.d \
./src/font_engine/font_tools.d \
./src/font_engine/sfont_asset.d \
./src/font_engine/tt_font_asset.d \
./src/font_engine/tt_font_factory.d 


# Each subdirectory must supply rules for building sources it contributes
src/font_engine/%.o: ../src/font_engine/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D_NO_TTF_RENDER_WORKAROUND -D__LINUX__ -I"C:\Users\Jurgen\Workspace\PEngIne\PEngIneCore\src" -I"C:\Users\Jurgen\Workspace\PEngIne\PEngInePSP\src" -I/usr/include/SDL -I/usr/include/tcl8.5 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<" `freetype-config --cflags`
	@echo 'Finished building: $<'
	@echo ' '


