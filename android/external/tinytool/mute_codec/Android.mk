# Copyright 2005 The Android Open Source Project

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	mute_codec.c

LOCAL_C_INCLUDES := \
	external/tinyalsa/include

LOCAL_MODULE:= mute_codec

#LOCAL_FORCE_STATIC_EXECUTABLE := true
#LOCAL_MODULE_PATH := $(TARGET_ROOT_OUT)
#LOCAL_UNSTRIPPED_PATH := $(TARGET_ROOT_OUT_UNSTRIPPED)

LOCAL_SHARED_LIBRARIES := \
		libcutils \
		libc \
		libtinyalsa

include $(BUILD_EXECUTABLE)