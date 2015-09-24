=============================================================================================
大概依赖关系:
+++++++++++++

$(INSTALLED_BOOTIMAGE_TARGET) : $(INSTALLED_KERNEL_TARGET)
$(INSTALLED_KERNEL_TARGET)    : $(TARGET_PREBUILT_KERNEL)
$(TARGET_PREBUILT_KERNEL)     : $(TARGET_PREBUILT_INT_KERNEL)

TARGET_PREBUILT_INT_KERNEL := $(KERNEL_OUT)/arch/arm/boot/zImage

$(TARGET_PREBUILT_INT_KERNEL): $(KERNEL_OUT) $(KERNEL_CONFIG) $(KERNEL_HEADERS_INSTALL)
    $(MAKE) -C $(KERNEL_TOP) O=$(KERNEL_OUT) ARCH=arm CROSS_COMPILE=$(KERNEL_TOOLCHAIN)
    ${KERNEL_TOP}/build_mali_ump.sh
    ${ANDROID_BUILD_TOP}/vendor/duokan/driver/dtv/build.sh $(KERNEL_OUT) $(ANDROID_PRODUCT_OUT)/system/lib/modules
    ${ANDROID_BUILD_TOP}/external/wireless-module/broadcom/bcm43569/build_bt_wlan.sh 
    $(MAKE) -C $(KERNEL_TOP) O=$(KERNEL_OUT) ARCH=arm CROSS_COMPILE=$(KERNEL_TOOLCHAIN) modules
    $(MAKE) -C $(KERNEL_TOP) O=$(KERNEL_OUT) INSTALL_MOD_PATH=$(KERNEL_MODULES_INSTALL) INSTALL_MOD_STRIP=1 ARCH=arm CROSS_COMPILE=$(KERNEL_TOOLCHAIN) modules_install
    $(mv-modules)
    $(clean-module-folder)
    $(append-dtb)

=============================================================================================
=============================================================================================
=============================================================================================
详细依赖关系:
=============================================================================================
build/core/Makefile
++++++++++++++++++++

 464 # -----------------------------------------------------------------
 465 # the boot image, which is a collection of other images.
 466 INTERNAL_BOOTIMAGE_ARGS := \
 467     $(addprefix --second ,$(INSTALLED_2NDBOOTLOADER_TARGET)) \
 468     --kernel $(INSTALLED_KERNEL_TARGET) \
 469     --ramdisk $(INSTALLED_RAMDISK_TARGET)

 520 # MStar Android Patch Begin
 521 # Support mkimage/lzo
 522 $(INSTALLED_BOOTIMAGE_TARGET): $(MKIMAGE) $(INTERNAL_BOOTIMAGE_FILES) $(INTERNAL_SECURE_BOOT_DEPS)
 523     $(call pretty,"Target boot image: $@")
 524     $(hide) $(MKIMAGE) -A arm -O linux -T multi -C none -a $(BOARD_KERNEL_BASE) -e $(BOARD_KERNEL_BASE) -n 'MStar-linux' -d $(INSTALLED_KERNEL_TARGET):$(INSTALLED_RAMDISK_TARGET) $@
 525     $(hide) $(call assert-max-image-size,$@,$(BOARD_BOOTIMAGE_PARTITION_SIZE))
 526 ifeq ($(BUILD_WITH_SECURE_BOOT),true)
 527     $(hide) cp $(PRODUCT_OUT)/boot.img $(PRODUCT_OUT)/boot.img.tmp
 528     $(hide) $(ALIGNMENT) $(PRODUCT_OUT)/boot.img.tmp
 529     $(hide) $(SUBSECUREINFOGEN) $(PRODUCT_OUT)/secure_info_boot.bin $(PRODUCT_OUT)/boot.img.tmp $(SECURE_BOOT_RSA_PRIVATE_KEY) $(SECURE_BOOT_RSA_PUBLIC_KEY) 0  8 1 2097152 0 $(HOST_OUT_EXECUTABLES)
 530     $(hide) $(AESCRYPT2) 0 $(PRODUCT_OUT)/boot.img.tmp $(PRODUCT_OUT)/boot.img $(SECURE_BOOT_AES_KEY)
 531     $(hide) rm $(PRODUCT_OUT)/boot.img.tmp
 532 endif
 533 # MStar Android Patch End

=============================================================================================
build/core/config.mk
+++++++++++++++++++++

189 ifneq ($(strip $(TARGET_NO_KERNEL)),true)
190   INSTALLED_KERNEL_TARGET := $(PRODUCT_OUT)/kernel
191 else
192   INSTALLED_KERNEL_TARGET :=
193 endif

=============================================================================================
device/mstar/gladiator/AndroidBoard.mk
++++++++++++++++++++++++++++++++++++++

  1 #----------------------------------------------------------------------
  2 # Compile Linux Kernel
  3 #----------------------------------------------------------------------
  4 ifeq ($(KERNEL_DEFCONFIG),)
  5     KERNEL_DEFCONFIG := mstar_928_defconfig
  6 endif
  7 
  8 KERNEL_TOOLCHAIN := $(ANDROID_BUILD_TOP)/prebuilts/gcc/linux-x86/arm/mstar-arm-tools/arm-2014.05/bin/arm-none-linux-gnueabi-
  9 
 10 include $(PRODUCT_KERNEL_TOP)/AndroidKernel.mk
 11 
 12 $(INSTALLED_KERNEL_TARGET): $(TARGET_PREBUILT_KERNEL) | $(ACP)
 13     $(transform-prebuilt-to-target)

=============================================================================================
=============================================================================================
=============================================================================================
kernel/AndroidKernel.mk
++++++++++++++++++++++++

#Android makefile to build kernel as a part of Android Build
PERL		= perl

-----------------------------------------------------------------------------------------
ifeq ($(TARGET_PREBUILT_KERNEL),)

$(info $(KERNEL_TOOLCHAIN))

KERNEL_TOP := $(PRODUCT_KERNEL_TOP)
KERNEL_OUT := $(ANDROID_BUILD_TOP)/$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ
KERNEL_CONFIG := $(KERNEL_OUT)/.config
TARGET_PREBUILT_INT_KERNEL := $(KERNEL_OUT)/arch/arm/boot/zImage
KERNEL_HEADERS_INSTALL := $(KERNEL_OUT)/usr
KERNEL_MODULES_INSTALL := $(ANDROID_PRODUCT_OUT)/system
KERNEL_MODULES_OUT := $(TARGET_OUT)/lib/modules
KERNEL_IMG=$(KERNEL_OUT)/arch/arm/boot/Image

DTS_NAMES ?= $(shell $(PERL) -e 'while (<>) {$$a = $$1 if /CONFIG_ARCH_((?:MSM|QSD|MPQ)[a-zA-Z0-9]+)=y/; $$r = $$1 if /CONFIG_MSM_SOC_REV_(?!NONE)(\w+)=y/; $$arch = $$arch.lc("$$a$$r ") if /CONFIG_ARCH_((?:MSM|QSD|MPQ)[a-zA-Z0-9]+)=y/} print $$arch;' $(KERNEL_CONFIG))
KERNEL_USE_OF ?= $(shell $(PERL) -e '$$of = "n"; while (<>) { if (/CONFIG_USE_OF=y/) { $$of = "y"; break; } } print $$of;' $(KERNEL_TOP)/arch/arm/configs/$(KERNEL_DEFCONFIG))

---------------------------------------------------------------
ifeq "$(KERNEL_USE_OF)" "y"
DTS_FILES = $(wildcard $(KERNEL_TOP)/arch/arm/boot/dts/$(DTS_NAME)*.dts)
DTS_FILE = $(lastword $(subst /, ,$(1)))
DTB_FILE = $(addprefix $(KERNEL_OUT)/arch/arm/boot/,$(patsubst %.dts,%.dtb,$(call DTS_FILE,$(1))))
ZIMG_FILE = $(addprefix $(KERNEL_OUT)/arch/arm/boot/,$(patsubst %.dts,%-zImage,$(call DTS_FILE,$(1))))
KERNEL_ZIMG = $(KERNEL_OUT)/arch/arm/boot/zImage
DTC = $(KERNEL_OUT)/scripts/dtc/dtc

define append-dtb
mkdir -p $(KERNEL_OUT)/arch/arm/boot;\
$(foreach DTS_NAME, $(DTS_NAMES), \
   $(foreach d, $(DTS_FILES), \
      $(DTC) -p 1024 -O dtb -o $(call DTB_FILE,$(d)) $(d); \
      cat $(KERNEL_ZIMG) $(call DTB_FILE,$(d)) > $(call ZIMG_FILE,$(d));))
endef
else

define append-dtb
endef
endif
---------------------------------------------------------------

ifeq ($(TARGET_USES_UNCOMPRESSED_KERNEL),true)
$(info Using uncompressed kernel)
TARGET_PREBUILT_KERNEL := $(KERNEL_OUT)/piggy
else
TARGET_PREBUILT_KERNEL := $(TARGET_PREBUILT_INT_KERNEL)
endif

---------------------------------------------------------------

define mv-modules
mdpath=`find $(KERNEL_MODULES_OUT) -type f -name modules.dep`;\
if [ "$$mdpath" != "" ];then\
mpath=`dirname $$mdpath`;\
ko=`find $$mpath/kernel -type f -name *.ko`;\
for i in $$ko; do mv $$i $(KERNEL_MODULES_OUT)/; done;\
fi
endef

---------------------------------------------------------------

define clean-module-folder
mdpath=`find $(KERNEL_MODULES_OUT) -type f -name modules.dep`;\
if [ "$$mdpath" != "" ];then\
mpath=`dirname $$mdpath`; rm -rf $$mpath;\
fi
endef

---------------------------------------------------------------
$(KERNEL_OUT):
	mkdir -p $(KERNEL_OUT)

$(KERNEL_CONFIG): $(KERNEL_OUT)
	$(MAKE) -C $(KERNEL_TOP) O=$(KERNEL_OUT) ARCH=arm CROSS_COMPILE=$(KERNEL_TOOLCHAIN) $(KERNEL_DEFCONFIG)

$(KERNEL_OUT)/piggy : $(TARGET_PREBUILT_INT_KERNEL)
	$(hide) gunzip -c $(KERNEL_OUT)/arch/arm/boot/compressed/piggy.gzip > $(KERNEL_OUT)/piggy

$(TARGET_PREBUILT_INT_KERNEL): $(KERNEL_OUT) $(KERNEL_CONFIG) $(KERNEL_HEADERS_INSTALL)
	$(MAKE) -C $(KERNEL_TOP) O=$(KERNEL_OUT) ARCH=arm CROSS_COMPILE=$(KERNEL_TOOLCHAIN)
	${KERNEL_TOP}/build_mali_ump.sh
	${ANDROID_BUILD_TOP}/vendor/duokan/driver/dtv/build.sh $(KERNEL_OUT) $(ANDROID_PRODUCT_OUT)/system/lib/modules
	${ANDROID_BUILD_TOP}/external/wireless-module/broadcom/bcm43569/build_bt_wlan.sh 
	$(MAKE) -C $(KERNEL_TOP) O=$(KERNEL_OUT) ARCH=arm CROSS_COMPILE=$(KERNEL_TOOLCHAIN) modules
	$(MAKE) -C $(KERNEL_TOP) O=$(KERNEL_OUT) INSTALL_MOD_PATH=$(KERNEL_MODULES_INSTALL) INSTALL_MOD_STRIP=1 ARCH=arm CROSS_COMPILE=$(KERNEL_TOOLCHAIN) modules_install
	$(mv-modules)
	$(clean-module-folder)
	$(append-dtb)

$(PRODUCT_OUT)/obj/KERNEL_OBJ/usr: $(KERNEL_HEADERS_INSTALL)
$(KERNEL_HEADERS_INSTALL): $(KERNEL_OUT) $(KERNEL_CONFIG)
	$(MAKE) -C $(KERNEL_TOP) O=$(KERNEL_OUT) ARCH=arm CROSS_COMPILE=$(KERNEL_TOOLCHAIN) headers_install
	@cp -a $(KERNEL_TOP)/include/mstar $(KERNEL_OUT)/usr/include/mstar

kerneltags: $(KERNEL_OUT) $(KERNEL_CONFIG)
	$(MAKE) -C $(KERNEL_TOP) O=$(KERNEL_OUT) ARCH=arm CROSS_COMPILE=$(KERNEL_TOOLCHAIN) tags

kernelconfig: $(KERNEL_OUT) $(KERNEL_CONFIG)
	env KCONFIG_NOTIMESTAMP=true \
	     $(MAKE) -C $(KERNEL_TOP) O=$(KERNEL_OUT) ARCH=arm CROSS_COMPILE=$(KERNEL_TOOLCHAIN) menuconfig
	env KCONFIG_NOTIMESTAMP=true \
	     $(MAKE) -C $(KERNEL_TOP) O=$(KERNEL_OUT) ARCH=arm CROSS_COMPILE=$(KERNEL_TOOLCHAIN) savedefconfig
	cp $(KERNEL_OUT)/defconfig $(KERNEL_TOP)/arch/arm/configs/$(KERNEL_DEFCONFIG)

endif
-----------------------------------------------------------------------------------------

=============================================================================================
=============================================================================================
=============================================================================================
