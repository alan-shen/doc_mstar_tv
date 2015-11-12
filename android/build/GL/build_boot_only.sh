#!/bin/bash

local_path=`pwd`

image_path="../images"

mkdir -p $image_path

# export java home environment
export JAVA_HOME=/usr/lib/jvm/java-7-openjdk-amd64
export JAVADOC_HOME=/usr/lib/jvm/java-7-openjdk-amd64

export ENV_PRODUCT_PACKAGES="AnalogTVPlayer MiTVAppStore MiTVDesktop mitv_framework MiTVMediaExplorer MiTVPinyinIME MiTVSettings build_info duokantv"

clear
source ./build/envsetup.sh

lunch gladiator-userdebug

target_device=$(get_build_var TARGET_DEVICE)

# build Android
function build_boot()
{
	make bootimage -j8 -l8
	if [ $? -ne 0 ] ;
	then
		echo "build boot failed"
		exit 1
	fi
	./development/scripts/releasebootimage.sh
	if [ $? -ne 0 ] ;
	then
		echo "release android image failed"
		exit 1
	fi

	cp out/target/product/$target_device/boot.img $image_path
}

function copy_to_tftproot()
{
	cp -rf  ../images/scripts               /tftpboot
	cp -rfv ../images/boot.img              /tftpboot
	cp -rfv ../images/boot1.img             /tftpboot
	cp -rfv ../images/secure_info_boot.bin  /tftpboot
	cp -rfv ../images/secure_info_boot1.bin /tftpboot
}

build_boot
copy_to_tftproot
