LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
    binder_test_service.cpp
  
LOCAL_SHARED_LIBRARIES := \
    libutils \
    libcutils \
    libbinder
  
# FIXME The duplicate audioflinger is temporary
LOCAL_C_INCLUDES := binder_test.h
  
LOCAL_MODULE:= bindertestservice
  
include $(BUILD_EXECUTABLE)
  
include $(CLEAR_VARS)
  
LOCAL_SRC_FILES:= \
    binder_test_client.cpp
  
LOCAL_SHARED_LIBRARIES := \
    libutils \
    libcutils \
    libbinder
  
# FIXME The duplicate audioflinger is temporary  
LOCAL_C_INCLUDES := binder_test.h
  
LOCAL_MODULE := bindertestclient
  
include $(BUILD_EXECUTABLE)
