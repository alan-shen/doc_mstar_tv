#!/bin/bash

function kernel_menuconfig () {
	make \
		-C /work/gladiator/android/kernel \
		O=/work/gladiator/android/out/target/product/gladiator/obj/KERNEL_OBJ \
		ARCH=arm \
		CROSS_COMPILE=/work/gladiator/android/prebuilts/gcc/linux-x86/arm/mstar-arm-tools/arm-2014.05/bin/arm-none-linux-gnueabi- \
		menuconfig
}

function kernel_defconfig () {
	make \
		-C /work/gladiator/android/kernel \
		O=/work/gladiator/android/out/target/product/gladiator/obj/KERNEL_OBJ \
		ARCH=arm \
		CROSS_COMPILE=/work/gladiator/android/prebuilts/gcc/linux-x86/arm/mstar-arm-tools/arm-2014.05/bin/arm-none-linux-gnueabi- \
		mstar_928_defconfig
}

export KCONFIG_NOTIMESTAMP=true

kernel_menuconfig
