LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := freetype-prebuilt
LOCAL_SRC_FILES := libs/$(TARGET_ARCH_ABI)/libfreetype.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(PREBUILT_SHARED_LIBRARY)

