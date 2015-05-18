################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/config.cpp \
../src/eap_dealer.cpp \
../src/function.cpp \
../src/get_nic_addr.cpp \
../src/main.cpp \
../src/pcap_dealer.cpp \
../src/utils.cpp 

C_SRCS += \
../src/md5.c 

OBJS += \
./src/config.o \
./src/eap_dealer.o \
./src/function.o \
./src/get_nic_addr.o \
./src/main.o \
./src/md5.o \
./src/pcap_dealer.o \
./src/utils.o 

C_DEPS += \
./src/md5.d 

CPP_DEPS += \
./src/config.d \
./src/eap_dealer.d \
./src/function.d \
./src/get_nic_addr.d \
./src/main.d \
./src/pcap_dealer.d \
./src/utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	mips-openwrt-linux-g++ -I"/home/hennsun/workspace/Drcom8021x_openwrt/header" -O3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	mips-openwrt-linux-gcc -I"/home/hennsun/workspace/Drcom8021x_openwrt/header" -O3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


