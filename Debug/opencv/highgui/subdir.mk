################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../opencv/highgui/bitstrm.cpp \
../opencv/highgui/grfmt_base.cpp \
../opencv/highgui/grfmt_bmp.cpp \
../opencv/highgui/grfmt_exr.cpp \
../opencv/highgui/grfmt_imageio.cpp \
../opencv/highgui/grfmt_jpeg.cpp \
../opencv/highgui/grfmt_jpeg2000.cpp \
../opencv/highgui/grfmt_png.cpp \
../opencv/highgui/grfmt_pxm.cpp \
../opencv/highgui/grfmt_sunras.cpp \
../opencv/highgui/grfmt_tiff.cpp \
../opencv/highgui/image.cpp \
../opencv/highgui/loadsave.cpp \
../opencv/highgui/utils.cpp \
../opencv/highgui/window.cpp \
../opencv/highgui/window_w32.cpp 

OBJS += \
./opencv/highgui/bitstrm.o \
./opencv/highgui/grfmt_base.o \
./opencv/highgui/grfmt_bmp.o \
./opencv/highgui/grfmt_exr.o \
./opencv/highgui/grfmt_imageio.o \
./opencv/highgui/grfmt_jpeg.o \
./opencv/highgui/grfmt_jpeg2000.o \
./opencv/highgui/grfmt_png.o \
./opencv/highgui/grfmt_pxm.o \
./opencv/highgui/grfmt_sunras.o \
./opencv/highgui/grfmt_tiff.o \
./opencv/highgui/image.o \
./opencv/highgui/loadsave.o \
./opencv/highgui/utils.o \
./opencv/highgui/window.o \
./opencv/highgui/window_w32.o 

CPP_DEPS += \
./opencv/highgui/bitstrm.d \
./opencv/highgui/grfmt_base.d \
./opencv/highgui/grfmt_bmp.d \
./opencv/highgui/grfmt_exr.d \
./opencv/highgui/grfmt_imageio.d \
./opencv/highgui/grfmt_jpeg.d \
./opencv/highgui/grfmt_jpeg2000.d \
./opencv/highgui/grfmt_png.d \
./opencv/highgui/grfmt_pxm.d \
./opencv/highgui/grfmt_sunras.d \
./opencv/highgui/grfmt_tiff.d \
./opencv/highgui/image.d \
./opencv/highgui/loadsave.d \
./opencv/highgui/utils.d \
./opencv/highgui/window.d \
./opencv/highgui/window_w32.d 


# Each subdirectory must supply rules for building sources it contributes
opencv/highgui/%.o: ../opencv/highgui/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -m64 -fPIC -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


