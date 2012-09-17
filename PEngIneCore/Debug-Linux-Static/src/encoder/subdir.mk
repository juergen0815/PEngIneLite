################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/encoder/enc_bmp_frames.cpp \
../src/encoder/enc_png_frames.cpp \
../src/encoder/enc_tiff_frames.cpp 

OBJS += \
./src/encoder/enc_bmp_frames.o \
./src/encoder/enc_png_frames.o \
./src/encoder/enc_tiff_frames.o 

CPP_DEPS += \
./src/encoder/enc_bmp_frames.d \
./src/encoder/enc_png_frames.d \
./src/encoder/enc_tiff_frames.d 


# Each subdirectory must supply rules for building sources it contributes
src/encoder/%.o: ../src/encoder/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D_NO_TTF_RENDER_WORKAROUND -D__LINUX__ -I"C:\Users\Jurgen\Workspace\PEngIne\PEngIneCore\src" -I"C:\Users\Jurgen\Workspace\PEngIne\PEngInePSP\src" -I/usr/include/SDL -I/usr/include/tcl8.5 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<" `freetype-config --cflags`
	@echo 'Finished building: $<'
	@echo ' '


