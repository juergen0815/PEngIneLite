################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
ASM_SRCS += \
../sse/sse_matrix.asm \
../sse/sse_shaders.asm \
../sse/sse_vector.asm 

OBJS += \
./sse/sse_matrix.o \
./sse/sse_shaders.o \
./sse/sse_vector.o 


# Each subdirectory must supply rules for building sources it contributes
sse/%.o: ../sse/%.asm
	@echo 'Building file: $<'
	@echo 'Invoking: GCC Assembler'
	as -I/usr/local/include/SDL -I/usr/local/include/boost-1_37 -I/usr/X11/include -I"C:\Users\Jurgen\Workspace\PEngIne\PEngInePSP\src" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


