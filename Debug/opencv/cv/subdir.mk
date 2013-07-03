################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../opencv/cv/cvaccum.cpp \
../opencv/cv/cvcalibration.cpp \
../opencv/cv/cvcolor.cpp \
../opencv/cv/cvfilter.cpp \
../opencv/cv/cvfundam.cpp \
../opencv/cv/cvimgwarp.cpp \
../opencv/cv/cvmodelest.cpp \
../opencv/cv/cvsumpixels.cpp \
../opencv/cv/cvtables.cpp \
../opencv/cv/cvtemplmatch.cpp \
../opencv/cv/cvundistort.cpp \
../opencv/cv/cvutils.cpp 

OBJS += \
./opencv/cv/cvaccum.o \
./opencv/cv/cvcalibration.o \
./opencv/cv/cvcolor.o \
./opencv/cv/cvfilter.o \
./opencv/cv/cvfundam.o \
./opencv/cv/cvimgwarp.o \
./opencv/cv/cvmodelest.o \
./opencv/cv/cvsumpixels.o \
./opencv/cv/cvtables.o \
./opencv/cv/cvtemplmatch.o \
./opencv/cv/cvundistort.o \
./opencv/cv/cvutils.o 

CPP_DEPS += \
./opencv/cv/cvaccum.d \
./opencv/cv/cvcalibration.d \
./opencv/cv/cvcolor.d \
./opencv/cv/cvfilter.d \
./opencv/cv/cvfundam.d \
./opencv/cv/cvimgwarp.d \
./opencv/cv/cvmodelest.d \
./opencv/cv/cvsumpixels.d \
./opencv/cv/cvtables.d \
./opencv/cv/cvtemplmatch.d \
./opencv/cv/cvundistort.d \
./opencv/cv/cvutils.d 


# Each subdirectory must supply rules for building sources it contributes
opencv/cv/%.o: ../opencv/cv/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


