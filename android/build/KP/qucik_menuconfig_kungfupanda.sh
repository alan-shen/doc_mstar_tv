#!/bin/bash
def_colors () {
        # 特性
        normal='\033[0m';       bold='\033[1m';         dim='\033[2m';          under='\033[4m';
        italic='\033[3m';       noitalic='\033[23m';    blink='\033[5m';
        reverse='\033[7m';      conceal='\033[8m';      nobold='\033[22m';
        nounder='\033[24m';     noblink='\033[25m';
        # 前景
        black='\033[30m';       red='\033[31m';         green='\033[32m';       yellow='\033[33m';
        blue='\033[34m';        magenta='\033[35m';     cyan='\033[36m';        white='\033[37m';
        # 背景
        bblack='\033[40m';      bred='\033[41m';
        bgreen='\033[42m';      byellow='\033[43m';
        bblue='\033[44m';       bmagenta='\033[245m';
        bcyan='\033[46m';       bwhite='\033[47m';
}

function kernel_menuconfig () {
	make \
		-C ${KERNEL_SOURCE_PATH} \
		O=${ANDROID_PRODUCT_OUT}/obj/KERNEL_OBJ \
		ARCH=${ARCH_PRODUCT} \
		CROSS_COMPILE=${CROSS_COMPILE_PATH} \
		menuconfig
}

function kernel_defconfig () {
	make \
		-C ${KERNEL_SOURCE_PATH} \
		O=${ANDROID_PRODUCT_OUT}/obj/KERNEL_OBJ \
		ARCH=${ARCH_PRODUCT} \
		CROSS_COMPILE=${CROSS_COMPILE_PATH} \
		${KERNEL_DEFCONFIG_NAME}
}

function save_config () {
	cp -rfv ${ANDROID_PRODUCT_OUT}/obj/KERNEL_OBJ/.config /${KERNEL_CONFIG_PATH}
}

function check_android_env () {
	if test -z ${ANDROID_PRODUCT_OUT};then
		echo -e ${bred}"No android build envirment, need source first!\n"${normal}
		echo -e ${blue}"\t# source build/envsetup.sh;lunch gladiator-userdebug\n"${normal}
		echo -e ${blue}"\t# source build/envsetup.sh;lunch kungfupanda-userdebug\n"${normal}
		exit 1
	fi
}

function product_env () {
	KERNEL_SOURCE_PATH="${ANDROID_BUILD_TOP}/kernel/mediatek/mt8173/3.10"
	CROSS_COMPILE_PATH="${ANDROID_BUILD_TOP}/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin/aarch64-linux-android-"
	KERNEL_CONFIG_PATH="${KERNEL_SOURCE_PATH}/arch/arm/configs/kungfupanda_debug_defconfig"
	KERNEL_DEFCONFIG_NAME="kungfupanda_debug_defconfig"
	ARCH_PRODUCT="arm64"
}

#main

def_colors
check_android_env
product_env

if test -z $1;then
	echo -e ${bred}"make menuconfig..."${normal}
	kernel_menuconfig
else
	if [ $1 == "def" ];then
		echo -e ${bred}"make defconfig ..."${normal}
		kernel_defconfig
	elif [ $1 == "save" ];then
		echo -e ${bred}"save config ..."${normal}
		save_config
	else
		echo -e ${bred}"unknown option!('def'/'save')"${normal}
	fi
fi

#end of main
