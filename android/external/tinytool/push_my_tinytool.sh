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

function check_android_env () {
	if test -z ${ANDROID_PRODUCT_OUT};then
		echo -e ${bred}"No android build envirment, need source first!\n"${normal}
		echo -e ${blue}"\t# source build/envsetup.sh;lunch gladiator-userdebug\n"${normal}
		echo -e ${blue}"\t# source build/envsetup.sh;lunch kungfupanda-userdebug\n"${normal}
		exit 1
	fi
}

function build_tools () {
	mmm external/tinytool
	mmm external/tinyalsa
}

function remount_device () {
	adb disconnect ${TARGET_IPADDR}

	if test ${MY_PRODUCT} == 'GL';then
		adb connect ${TARGET_IPADDR}
		sleep 3
	fi

	adb ${OPTION_ADB} root
	sleep 3

	if test ${MY_PRODUCT} == 'GL';then
		adb connect ${TARGET_IPADDR}
		sleep 3
	fi

	adb ${OPTION_ADB} remount

	if test $? -eq "0";then
		return 0
	else
		return 1
	fi
}

function wait_adb_device () {
	while true;
	do
		remount_device
		if test $? -eq "0";then
			return 0
		fi
		echo -e ${bred}"remount device error, retry!"${normal}
	done
}

SYSTEM_LIB_COPYLIST=""
SYSTEM_BIN_COPYLIST="getgpt tree getevent"
SYSTEM_BIN_AUDIO="tinyplay tinycap tinymix tinypcminfo"
function push_tiny_tools () {
	# lib
	for ilib in ${SYSTEM_LIB_COPYLIST}
	do
		echo -e ${yellow}send ${ilib}...${normal}
		adb ${OPTION_ADB} push ${ANDROID_PRODUCT_OUT}/system/lib/${ilib}    /system/lib
	done
	# binary
	for ibin in ${SYSTEM_BIN_COPYLIST}
	do
		echo -e ${yellow}send ${ibin}...${normal}
		adb ${OPTION_ADB} push ${ANDROID_PRODUCT_OUT}/system/bin/${ibin}    /system/bin
	done
	#tiny alsa
	for ialsa in ${SYSTEM_BIN_AUDIO}
	do
		echo -e ${yellow}send ${ialsa}...${normal}
		adb ${OPTION_ADB} push ${ANDROID_PRODUCT_OUT}/system/bin/${ialsa}    /system/bin
	done

	# extrenal
	adb ${OPTION_ADB} push external/tinytool/scripts/watch    /system/bin
}

#for gladiator
MY_PRODUCT=""
OPTION_ADB=""
TARGET_IPADDR="10.235.172.7"

function get_product() {
	if test ${TARGET_PRODUCT} == 'gladiator';then
		echo -e ${bcyan}"gladiator"${normal}
		MY_PRODUCT="GL"
		OPTION_ADB=" -s ${TARGET_IPADDR}:5555 "
	elif test ${TARGET_PRODUCT} == 'kungfupanda';then
		echo -e ${bcyan}"kungfupanda"${normal}
		MY_PRODUCT="KP"
		OPTION_ADB=" "
	else
		echo -e ${bcyan}"not support this product now!"${normal}
		MY_PRODUCT=""
		exit 1
	fi
}

#main

def_colors
check_android_env
get_product
#build_tools
wait_adb_device
push_tiny_tools

#if test -z $1;then
#else
#	if [ $1 == "def" ];then
#	elif [ $1 == "save" ];then
#	else
#	fi
#fi

#end of main
