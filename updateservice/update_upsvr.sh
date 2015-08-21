#!/bin/bash

echo "==============================> Wait for adb connect ..."
adb wait-for-devices;
adb devices;

echo "============================== Install Shared Lib  =============================="
adb push out/target/product/gladiator/system/lib/libpostupdate.so		/system/lib/libpostupdate.so
adb push out/target/product/gladiator/system/lib/libupdate.so			/system/lib/libupdate.so
adb push out/target/product/gladiator/system/lib/libupdateservice.so	/system/lib/libupdateservice.so
echo "============================== Install Exe         =============================="
adb push out/target/product/gladiator/system/bin/updateservice			/system/bin/updateservice
adb push out/target/product/gladiator/system/bin/runupdate				/system/bin/runupdate
echo "============================== Install Ext4_Utils & Libsparse ==================="
adb push out/target/product/gladiator/system/lib/libsparse.so			/system/lib/libsparse.so
adb push out/target/product/gladiator/system/bin/simg2img				/system/bin/simg2img
adb push out/target/product/gladiator/system/bin/img2simg				/system/bin/img2simg
adb push out/target/product/gladiator/system/lib/libext4_utils.so		/system/lib/libext4_utils.so
adb push out/target/product/gladiator/system/bin/make_ext4fs			/system/bin/make_ext4fs
adb push out/target/product/gladiator/system/bin/ext2simg				/system/bin/ext2simg
adb push out/target/product/gladiator/system/bin/ext4fixup				/system/bin/ext4fixup
echo "================================================================================="
