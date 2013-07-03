################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../api.cpp \
../config.cpp \
../dictionary.cpp \
../fasthessian.cpp \
../integral.cpp \
../ipoint.cpp \
../opensurf.cpp \
../surf.cpp \
../topsurf.cpp 

OBJS += \
./api.o \
./config.o \
./dictionary.o \
./fasthessian.o \
./integral.o \
./ipoint.o \
./opensurf.o \
./surf.o \
./topsurf.o 

CPP_DEPS += \
./api.d \
./config.d \
./dictionary.d \
./fasthessian.d \
./integral.d \
./ipoint.d \
./opensurf.d \
./surf.d \
./topsurf.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -m64 -fPIC -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


