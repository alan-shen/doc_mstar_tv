#!/bin/bash

clear

START=`date`

source build/envsetup.sh
lunch kungfupanda-userdebug
make -j8
if [ $? != 0];then
    echo "make error"
    exit 1
fi
make sign-image -j8
if [ $? != 0];then
    echo "sign error"
    exit 1
fi
make otapackage -j8
if [ $? != 0];then
    echo "make ota error"
    exit 1
fi
make sign-image -j8
if [ $? != 0];then
    echo "sign error"
    exit 1
fi

END=`date`
echo -e "Build start @$START"
echo -e "Build end   @$END"
