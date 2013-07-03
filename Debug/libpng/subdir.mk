################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libpng/png.c \
../libpng/pngerror.c \
../libpng/pngget.c \
../libpng/pngmem.c \
../libpng/pngpread.c \
../libpng/pngread.c \
../libpng/pngrio.c \
../libpng/pngrtran.c \
../libpng/pngrutil.c \
../libpng/pngset.c \
../libpng/pngtrans.c \
../libpng/pngwio.c \
../libpng/pngwrite.c \
../libpng/pngwtran.c \
../libpng/pngwutil.c 

OBJS += \
./libpng/png.o \
./libpng/pngerror.o \
./libpng/pngget.o \
./libpng/pngmem.o \
./libpng/pngpread.o \
./libpng/pngread.o \
./libpng/pngrio.o \
./libpng/pngrtran.o \
./libpng/pngrutil.o \
./libpng/pngset.o \
./libpng/pngtrans.o \
./libpng/pngwio.o \
./libpng/pngwrite.o \
./libpng/pngwtran.o \
./libpng/pngwutil.o 

C_DEPS += \
./libpng/png.d \
./libpng/pngerror.d \
./libpng/pngget.d \
./libpng/pngmem.d \
./libpng/pngpread.d \
./libpng/pngread.d \
./libpng/pngrio.d \
./libpng/pngrtran.d \
./libpng/pngrutil.d \
./libpng/pngset.d \
./libpng/pngtrans.d \
./libpng/pngwio.d \
./libpng/pngwrite.d \
./libpng/pngwtran.d \
./libpng/pngwutil.d 


# Each subdirectory must supply rules for building sources it contributes
libpng/%.o: ../libpng/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -m64 -fPIC -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


