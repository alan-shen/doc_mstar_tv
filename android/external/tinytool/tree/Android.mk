# Copyright 2005 The Android Open Source Project

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	color.c  \
	hash.c \
	html.c  \
	strverscmp.c  \
	tree.c  \
	unix.c  \
	xml.c

LOCAL_MODULE:= tree

LOCAL_CFLAGS:= -O4 -Wall  -DLINUX -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
LOCAL_LDFLAGS:= -s

#LOCAL_FORCE_STATIC_EXECUTABLE := true
#LOCAL_STATIC_LIBRARIES := libcutils libc
LOCAL_SHARED_LIBRARIES := libcutils libc

include $(BUILD_EXECUTABLE)
