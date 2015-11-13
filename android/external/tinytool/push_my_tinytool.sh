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
		exit 1
	fi
}

function remount_device () {
	adb connect 10.235.172.7
	sleep 3
	adb root
	sleep 3
	adb connect 10.235.172.7
	sleep 3
	adb remount
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
	done
}

function push_tiny_tools () {
	adb push ${ANDROID_PRODUCT_OUT}/system/bin/getgpt    /system/bin
	adb push ${ANDROID_PRODUCT_OUT}/system/bin/tree      /system/bin
	adb push ${ANDROID_PRODUCT_OUT}/system/bin/getuevent /system/bin
}

#main

def_colors
check_android_env
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
