################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libjpeg/jaricom.c \
../libjpeg/jcapimin.c \
../libjpeg/jcapistd.c \
../libjpeg/jcarith.c \
../libjpeg/jccoefct.c \
../libjpeg/jccolor.c \
../libjpeg/jcdctmgr.c \
../libjpeg/jchuff.c \
../libjpeg/jcinit.c \
../libjpeg/jcmainct.c \
../libjpeg/jcmarker.c \
../libjpeg/jcmaster.c \
../libjpeg/jcomapi.c \
../libjpeg/jcparam.c \
../libjpeg/jcprepct.c \
../libjpeg/jcsample.c \
../libjpeg/jctrans.c \
../libjpeg/jdapimin.c \
../libjpeg/jdapistd.c \
../libjpeg/jdarith.c \
../libjpeg/jdatadst.c \
../libjpeg/jdatasrc.c \
../libjpeg/jdcoefct.c \
../libjpeg/jdcolor.c \
../libjpeg/jddctmgr.c \
../libjpeg/jdhuff.c \
../libjpeg/jdinput.c \
../libjpeg/jdmainct.c \
../libjpeg/jdmarker.c \
../libjpeg/jdmaster.c \
../libjpeg/jdmerge.c \
../libjpeg/jdpostct.c \
../libjpeg/jdsample.c \
../libjpeg/jdtrans.c \
../libjpeg/jerror.c \
../libjpeg/jfdctflt.c \
../libjpeg/jfdctfst.c \
../libjpeg/jfdctint.c \
../libjpeg/jidctflt.c \
../libjpeg/jidctfst.c \
../libjpeg/jidctint.c \
../libjpeg/jmemmgr.c \
../libjpeg/jmemnobs.c \
../libjpeg/jquant1.c \
../libjpeg/jquant2.c \
../libjpeg/jutils.c 

OBJS += \
./libjpeg/jaricom.o \
./libjpeg/jcapimin.o \
./libjpeg/jcapistd.o \
./libjpeg/jcarith.o \
./libjpeg/jccoefct.o \
./libjpeg/jccolor.o \
./libjpeg/jcdctmgr.o \
./libjpeg/jchuff.o \
./libjpeg/jcinit.o \
./libjpeg/jcmainct.o \
./libjpeg/jcmarker.o \
./libjpeg/jcmaster.o \
./libjpeg/jcomapi.o \
./libjpeg/jcparam.o \
./libjpeg/jcprepct.o \
./libjpeg/jcsample.o \
./libjpeg/jctrans.o \
./libjpeg/jdapimin.o \
./libjpeg/jdapistd.o \
./libjpeg/jdarith.o \
./libjpeg/jdatadst.o \
./libjpeg/jdatasrc.o \
./libjpeg/jdcoefct.o \
./libjpeg/jdcolor.o \
./libjpeg/jddctmgr.o \
./libjpeg/jdhuff.o \
./libjpeg/jdinput.o \
./libjpeg/jdmainct.o \
./libjpeg/jdmarker.o \
./libjpeg/jdmaster.o \
./libjpeg/jdmerge.o \
./libjpeg/jdpostct.o \
./libjpeg/jdsample.o \
./libjpeg/jdtrans.o \
./libjpeg/jerror.o \
./libjpeg/jfdctflt.o \
./libjpeg/jfdctfst.o \
./libjpeg/jfdctint.o \
./libjpeg/jidctflt.o \
./libjpeg/jidctfst.o \
./libjpeg/jidctint.o \
./libjpeg/jmemmgr.o \
./libjpeg/jmemnobs.o \
./libjpeg/jquant1.o \
./libjpeg/jquant2.o \
./libjpeg/jutils.o 

C_DEPS += \
./libjpeg/jaricom.d \
./libjpeg/jcapimin.d \
./libjpeg/jcapistd.d \
./libjpeg/jcarith.d \
./libjpeg/jccoefct.d \
./libjpeg/jccolor.d \
./libjpeg/jcdctmgr.d \
./libjpeg/jchuff.d \
./libjpeg/jcinit.d \
./libjpeg/jcmainct.d \
./libjpeg/jcmarker.d \
./libjpeg/jcmaster.d \
./libjpeg/jcomapi.d \
./libjpeg/jcparam.d \
./libjpeg/jcprepct.d \
./libjpeg/jcsample.d \
./libjpeg/jctrans.d \
./libjpeg/jdapimin.d \
./libjpeg/jdapistd.d \
./libjpeg/jdarith.d \
./libjpeg/jdatadst.d \
./libjpeg/jdatasrc.d \
./libjpeg/jdcoefct.d \
./libjpeg/jdcolor.d \
./libjpeg/jddctmgr.d \
./libjpeg/jdhuff.d \
./libjpeg/jdinput.d \
./libjpeg/jdmainct.d \
./libjpeg/jdmarker.d \
./libjpeg/jdmaster.d \
./libjpeg/jdmerge.d \
./libjpeg/jdpostct.d \
./libjpeg/jdsample.d \
./libjpeg/jdtrans.d \
./libjpeg/jerror.d \
./libjpeg/jfdctflt.d \
./libjpeg/jfdctfst.d \
./libjpeg/jfdctint.d \
./libjpeg/jidctflt.d \
./libjpeg/jidctfst.d \
./libjpeg/jidctint.d \
./libjpeg/jmemmgr.d \
./libjpeg/jmemnobs.d \
./libjpeg/jquant1.d \
./libjpeg/jquant2.d \
./libjpeg/jutils.d 


# Each subdirectory must supply rules for building sources it contributes
libjpeg/%.o: ../libjpeg/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -m64 -fPIC -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


