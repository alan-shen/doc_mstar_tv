# Copyright 2005 The Android Open Source Project

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES:= iostat.c

LOCAL_MODULE:= iostat

LOCAL_SHARED_LIBRARIES :=   \
    libutils                \
    libc                    \
    libcutils               \
    liblog

LOCAL_C_INCLUDES +=

LOCAL_SHARED_LIBRARIES := \
        libc \
        libcutils \
        libutils

include $(BUILD_EXECUTABLE)
