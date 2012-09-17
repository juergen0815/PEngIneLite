################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/decoder/dec_audio.cpp \
../src/decoder/dec_audio_ogg.cpp \
../src/decoder/dec_audio_wav.cpp \
../src/decoder/dec_ogg_theora.cpp \
../src/decoder/dec_video.cpp \
../src/decoder/dec_video_bmp_image.cpp \
../src/decoder/dec_video_dds.cpp \
../src/decoder/dec_video_jpeg_image.cpp \
../src/decoder/dec_video_png_image.cpp \
../src/decoder/dec_video_png_sequence.cpp \
../src/decoder/dec_video_tiff_image.cpp 

OBJS += \
./src/decoder/dec_audio.o \
./src/decoder/dec_audio_ogg.o \
./src/decoder/dec_audio_wav.o \
./src/decoder/dec_ogg_theora.o \
./src/decoder/dec_video.o \
./src/decoder/dec_video_bmp_image.o \
./src/decoder/dec_video_dds.o \
./src/decoder/dec_video_jpeg_image.o \
./src/decoder/dec_video_png_image.o \
./src/decoder/dec_video_png_sequence.o \
./src/decoder/dec_video_tiff_image.o 

CPP_DEPS += \
./src/decoder/dec_audio.d \
./src/decoder/dec_audio_ogg.d \
./src/decoder/dec_audio_wav.d \
./src/decoder/dec_ogg_theora.d \
./src/decoder/dec_video.d \
./src/decoder/dec_video_bmp_image.d \
./src/decoder/dec_video_dds.d \
./src/decoder/dec_video_jpeg_image.d \
./src/decoder/dec_video_png_image.d \
./src/decoder/dec_video_png_sequence.d \
./src/decoder/dec_video_tiff_image.d 


# Each subdirectory must supply rules for building sources it contributes
src/decoder/%.o: ../src/decoder/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D_NO_TTF_RENDER_WORKAROUND -D__LINUX__ -I"C:\Users\Jurgen\Workspace\PEngIne\PEngIneCore\src" -I"C:\Users\Jurgen\Workspace\PEngIne\PEngInePSP\src" -I/usr/include/SDL -I/usr/include/tcl8.5 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<" `freetype-config --cflags`
	@echo 'Finished building: $<'
	@echo ' '


