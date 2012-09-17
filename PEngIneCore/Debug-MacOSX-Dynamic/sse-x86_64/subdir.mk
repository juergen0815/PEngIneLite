################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
ASM_SRCS += \
../sse-x86_64/sse_matrix.asm \
../sse-x86_64/sse_shaders.asm \
../sse-x86_64/sse_vector.asm 

OBJS += \
./sse-x86_64/sse_matrix.o \
./sse-x86_64/sse_shaders.o \
./sse-x86_64/sse_vector.o 


# Each subdirectory must supply rules for building sources it contributes
sse-x86_64/%.o: ../sse-x86_64/%.asm
	@echo 'Building file: $<'
	@echo 'Invoking: GCC Assembler'
	as -I/usr/local/include/SDL -I/usr/local/include/boost-1_37 -I/usr/X11/include -I"C:\Users\Jurgen\Workspace\PEngIne\PEngInePSP\src" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


