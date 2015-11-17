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

function setup_env () {
	source build/envsetup.sh
	lunch kungfupanda-userdebug
}

function def_full_build () {
	make -j8
	if test $? -eq "0";then
		echo -e ${bblue}"=====================================full build success"${normal}
	else
		echo -e ${bred}"=====================================full build error"${normal}
    	exit 1
	fi
}

function def_sign_image () {
	make sign-image -j8
	if test $? -eq "0";then
		echo -e ${bblue}"=====================================sign success"${normal}
	else
		echo -e ${bred}"=====================================sign error"${normal}
    	exit 1
	fi
}

function def_make_ota () {
	make otapackage -j8
	if test $? -eq "0";then
		echo -e ${bblue}"=====================================make ota success"${normal}
	else
		echo -e ${bred}"=====================================make ota error"${normal}
    	exit 1
	fi
}

clear
START=`date`

def_colors
setup_env
def_full_build
def_sign_image
def_make_ota
def_sign_image

END=`date`
echo -e "${cyan}Build start${normal}${yellow} @$START ${normal}"
echo -e "${cyan}Build end  ${normal}${yellow} @$END   ${normal}"
