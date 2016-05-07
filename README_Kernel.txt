################################################################################

1. How to Build kernel and mali
	- get Toolchain
		From android git server , codesourcery and etc ..
		 - arm-eabi-4.7
		
	- edit /common/Makefile
		edit target architecture.
		 - ARCH ?= arm
		edit "CROSS_COMPILE" to right toolchain path(You downloaded).
		  EX)  CROSS_COMPILE= $(android platform directory you download)/android/prebuilts/gcc/linux-x86/arm/arm-eabi-4.7/bin/arm-eabi-
          Ex)  CROSS_COMPILE=/usr/local/toolchain/arm-eabi-4.7/bin/arm-eabi-          // check the location of toolchain
  	
		$ make all

2. Output files
	- Kernel : common/arch/arm/boot/zImage
	- module : common/drivers/*/*.ko
	- mali : mali/*.ko

3. How to Clean	
		$ make clean
################################################################################
