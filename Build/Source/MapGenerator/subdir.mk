################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Source/MapGenerator/MapGenerator.cpp \
../Source/MapGenerator/RecoderWriter.cpp 

OBJS += \
./Source/MapGenerator/MapGenerator.o \
./Source/MapGenerator/RecoderWriter.o 

CPP_DEPS += \
./Source/MapGenerator/MapGenerator.d \
./Source/MapGenerator/RecoderWriter.d 


# Each subdirectory must supply rules for building sources it contributes
Source/MapGenerator/%.o: ../Source/MapGenerator/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-openwrt-linux-muslgnueabi-g++ -I$(SENAVICOMMON_PATH)/Source -I$(STAGING_DIR)/usr/include/allwinner -I$(STAGING_DIR)/usr/include/allwinner/include/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


