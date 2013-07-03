################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../opencv/cxcore/cxalloc.cpp \
../opencv/cxcore/cxarithm.cpp \
../opencv/cxcore/cxarray.cpp \
../opencv/cxcore/cxconvert.cpp \
../opencv/cxcore/cxcopy.cpp \
../opencv/cxcore/cxdatastructs.cpp \
../opencv/cxcore/cxdrawing.cpp \
../opencv/cxcore/cxdxt.cpp \
../opencv/cxcore/cximage.cpp \
../opencv/cxcore/cxlapack.cpp \
../opencv/cxcore/cxmathfuncs.cpp \
../opencv/cxcore/cxmatmul.cpp \
../opencv/cxcore/cxmatrix.cpp \
../opencv/cxcore/cxpersistence.cpp \
../opencv/cxcore/cxrand.cpp \
../opencv/cxcore/cxstat.cpp \
../opencv/cxcore/cxsystem.cpp \
../opencv/cxcore/cxtables.cpp 

OBJS += \
./opencv/cxcore/cxalloc.o \
./opencv/cxcore/cxarithm.o \
./opencv/cxcore/cxarray.o \
./opencv/cxcore/cxconvert.o \
./opencv/cxcore/cxcopy.o \
./opencv/cxcore/cxdatastructs.o \
./opencv/cxcore/cxdrawing.o \
./opencv/cxcore/cxdxt.o \
./opencv/cxcore/cximage.o \
./opencv/cxcore/cxlapack.o \
./opencv/cxcore/cxmathfuncs.o \
./opencv/cxcore/cxmatmul.o \
./opencv/cxcore/cxmatrix.o \
./opencv/cxcore/cxpersistence.o \
./opencv/cxcore/cxrand.o \
./opencv/cxcore/cxstat.o \
./opencv/cxcore/cxsystem.o \
./opencv/cxcore/cxtables.o 

CPP_DEPS += \
./opencv/cxcore/cxalloc.d \
./opencv/cxcore/cxarithm.d \
./opencv/cxcore/cxarray.d \
./opencv/cxcore/cxconvert.d \
./opencv/cxcore/cxcopy.d \
./opencv/cxcore/cxdatastructs.d \
./opencv/cxcore/cxdrawing.d \
./opencv/cxcore/cxdxt.d \
./opencv/cxcore/cximage.d \
./opencv/cxcore/cxlapack.d \
./opencv/cxcore/cxmathfuncs.d \
./opencv/cxcore/cxmatmul.d \
./opencv/cxcore/cxmatrix.d \
./opencv/cxcore/cxpersistence.d \
./opencv/cxcore/cxrand.d \
./opencv/cxcore/cxstat.d \
./opencv/cxcore/cxsystem.d \
./opencv/cxcore/cxtables.d 


# Each subdirectory must supply rules for building sources it contributes
opencv/cxcore/%.o: ../opencv/cxcore/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -m64 -fPIC -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


