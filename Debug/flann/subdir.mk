################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../flann/dist.cpp \
../flann/flann.cpp \
../flann/kdtree.cpp \
../flann/logger.cpp \
../flann/nnindex.cpp \
../flann/random.cpp \
../flann/resultset.cpp \
../flann/testing.cpp 

OBJS += \
./flann/dist.o \
./flann/flann.o \
./flann/kdtree.o \
./flann/logger.o \
./flann/nnindex.o \
./flann/random.o \
./flann/resultset.o \
./flann/testing.o 

CPP_DEPS += \
./flann/dist.d \
./flann/flann.d \
./flann/kdtree.d \
./flann/logger.d \
./flann/nnindex.d \
./flann/random.d \
./flann/resultset.d \
./flann/testing.d 


# Each subdirectory must supply rules for building sources it contributes
flann/%.o: ../flann/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -m64 -fPIC -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


