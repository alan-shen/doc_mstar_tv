#!/bin/bash

function menuconfig () {
	make \
		-C kernel/mediatek/mt8173/3.10 \
		O=/work/kungfupanda/out/target/product/kungfupanda/obj/KERNEL_OBJ \
		ARCH=arm64 \
		CROSS_COMPILE=/work/kungfupanda/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin/aarch64-linux-android- \
		menuconfig
}

function defconfig () {
	make \
		-C kernel/mediatek/mt8173/3.10 \
		O=/work/kungfupanda/out/target/product/kungfupanda/obj/KERNEL_OBJ \
		ARCH=arm64 \
		CROSS_COMPILE=/work/kungfupanda/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin/aarch64-linux-android- \
		kungfupanda_debug_defconfig
}

menuconfig
