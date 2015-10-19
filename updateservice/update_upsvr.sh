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

def_wait_device () {
	echo -e ${blue}">>>>> Waiting for adb device ready..."${normal}
	adb wait-for-devices;
	adb devices;
	echo -e ${blue}">>>>> READY <<<<<"${normal}
}

#PRODUCT_OUT="/home/shenpengru/out/target/product/"
PRODUCT_OUT="/work/inception/android/out/target/product/gladiator"
#SYSTEM_BIN_COPYLIST="updateservice runupdate simg2img img2simg make_ext4fs ext2simg ext4fixup"
#SYSTEM_LIB_COPYLIST="libpostupdate.so libupdate.so libupdateservice.so libsparse.so libext4_utils.so"
SYSTEM_BIN_COPYLIST="updateservice runupdate mi_md5chk mi_md5list"
SYSTEM_LIB_COPYLIST="libpostupdate.so libupdate.so libupdateservice.so libmitvmd5.so"
#SYSTEM_BIN_COPYLIST="updateservice updatetest runupdate mitv_updater mitvcopy"
#SYSTEM_LIB_COPYLIST="libupdateservice.so"

main () {
	def_colors
	def_wait_device

	echo -e ${green}"\n>>>>> Copying binarys..."${normal}
	for ibin in ${SYSTEM_BIN_COPYLIST}
	do
		echo -e ${yellow}"${ibin}"${normal}
		adb push ${PRODUCT_OUT}/system/bin/${ibin} /system/bin/${ibin}
	done

	echo -e ${green}"\n>>>>> Copying libs..."${normal}
	for ilib in ${SYSTEM_LIB_COPYLIST}
	do
		echo -e ${yellow}"${ilib}"${normal}
		adb push ${PRODUCT_OUT}/system/lib/${ilib} /system/lib/${ilib}
	done
}

copy_to_udisk () {
	def_colors

	mkdir -p $1/system/bin
	mkdir -p $1/system/lib

	echo -e ${green}"\n>>>>> Copying binarys..."${normal}
	for ibin in ${SYSTEM_BIN_COPYLIST}
	do
		echo -e ${yellow}"${ibin}"${normal}
		cp -rfv ${PRODUCT_OUT}/system/bin/${ibin} $1/system/bin/${ibin}
	done

	echo -e ${green}"\n>>>>> Copying libs..."${normal}
	for ilib in ${SYSTEM_LIB_COPYLIST}
	do
		echo -e ${yellow}"${ilib}"${normal}
		cp -rfv ${PRODUCT_OUT}/system/lib/${ilib} $1/system/lib/${ilib}
	done
}

if test ! -z $1;then
	copy_to_udisk $1
else
	main
    #adb push ${PRODUCT_OUT}/system/bin/mitv_updater /tmp/mitv_updater
fi
