################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libtiff/tif_aux.c \
../libtiff/tif_close.c \
../libtiff/tif_codec.c \
../libtiff/tif_color.c \
../libtiff/tif_compress.c \
../libtiff/tif_dir.c \
../libtiff/tif_dirinfo.c \
../libtiff/tif_dirread.c \
../libtiff/tif_dirwrite.c \
../libtiff/tif_dumpmode.c \
../libtiff/tif_error.c \
../libtiff/tif_extension.c \
../libtiff/tif_fax3.c \
../libtiff/tif_fax3sm.c \
../libtiff/tif_flush.c \
../libtiff/tif_getimage.c \
../libtiff/tif_jbig.c \
../libtiff/tif_jpeg.c \
../libtiff/tif_luv.c \
../libtiff/tif_lzw.c \
../libtiff/tif_next.c \
../libtiff/tif_ojpeg.c \
../libtiff/tif_open.c \
../libtiff/tif_packbits.c \
../libtiff/tif_pixarlog.c \
../libtiff/tif_predict.c \
../libtiff/tif_print.c \
../libtiff/tif_read.c \
../libtiff/tif_strip.c \
../libtiff/tif_swab.c \
../libtiff/tif_thunder.c \
../libtiff/tif_tile.c \
../libtiff/tif_unix.c \
../libtiff/tif_version.c \
../libtiff/tif_warning.c \
../libtiff/tif_write.c \
../libtiff/tif_zip.c 

OBJS += \
./libtiff/tif_aux.o \
./libtiff/tif_close.o \
./libtiff/tif_codec.o \
./libtiff/tif_color.o \
./libtiff/tif_compress.o \
./libtiff/tif_dir.o \
./libtiff/tif_dirinfo.o \
./libtiff/tif_dirread.o \
./libtiff/tif_dirwrite.o \
./libtiff/tif_dumpmode.o \
./libtiff/tif_error.o \
./libtiff/tif_extension.o \
./libtiff/tif_fax3.o \
./libtiff/tif_fax3sm.o \
./libtiff/tif_flush.o \
./libtiff/tif_getimage.o \
./libtiff/tif_jbig.o \
./libtiff/tif_jpeg.o \
./libtiff/tif_luv.o \
./libtiff/tif_lzw.o \
./libtiff/tif_next.o \
./libtiff/tif_ojpeg.o \
./libtiff/tif_open.o \
./libtiff/tif_packbits.o \
./libtiff/tif_pixarlog.o \
./libtiff/tif_predict.o \
./libtiff/tif_print.o \
./libtiff/tif_read.o \
./libtiff/tif_strip.o \
./libtiff/tif_swab.o \
./libtiff/tif_thunder.o \
./libtiff/tif_tile.o \
./libtiff/tif_unix.o \
./libtiff/tif_version.o \
./libtiff/tif_warning.o \
./libtiff/tif_write.o \
./libtiff/tif_zip.o 

C_DEPS += \
./libtiff/tif_aux.d \
./libtiff/tif_close.d \
./libtiff/tif_codec.d \
./libtiff/tif_color.d \
./libtiff/tif_compress.d \
./libtiff/tif_dir.d \
./libtiff/tif_dirinfo.d \
./libtiff/tif_dirread.d \
./libtiff/tif_dirwrite.d \
./libtiff/tif_dumpmode.d \
./libtiff/tif_error.d \
./libtiff/tif_extension.d \
./libtiff/tif_fax3.d \
./libtiff/tif_fax3sm.d \
./libtiff/tif_flush.d \
./libtiff/tif_getimage.d \
./libtiff/tif_jbig.d \
./libtiff/tif_jpeg.d \
./libtiff/tif_luv.d \
./libtiff/tif_lzw.d \
./libtiff/tif_next.d \
./libtiff/tif_ojpeg.d \
./libtiff/tif_open.d \
./libtiff/tif_packbits.d \
./libtiff/tif_pixarlog.d \
./libtiff/tif_predict.d \
./libtiff/tif_print.d \
./libtiff/tif_read.d \
./libtiff/tif_strip.d \
./libtiff/tif_swab.d \
./libtiff/tif_thunder.d \
./libtiff/tif_tile.d \
./libtiff/tif_unix.d \
./libtiff/tif_version.d \
./libtiff/tif_warning.d \
./libtiff/tif_write.d \
./libtiff/tif_zip.d 


# Each subdirectory must supply rules for building sources it contributes
libtiff/%.o: ../libtiff/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


