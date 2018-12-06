LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := gcanvas

# Collect all source files
MY_SRC_LIST := $(wildcard $(LOCAL_PATH)/*.cpp)
MY_SRC_LIST += $(wildcard $(LOCAL_PATH)/*.hpp)
MY_SRC_LIST += $(wildcard $(LOCAL_PATH)/gcanvas/*.cpp)
MY_SRC_LIST += $(wildcard $(LOCAL_PATH)/support/*.cpp)
MY_SRC_LIST += $(wildcard $(LOCAL_PATH)/png/*.cpp)
MY_SRC_LIST += $(wildcard $(LOCAL_PATH)/png/thirdparty/*.c)
MY_SRC_LIST += $(wildcard $(LOCAL_PATH)/3d/*.cpp)
MY_SRC_LIST += $(wildcard $(LOCAL_PATH)/3d/jsc/*.cpp)
MY_SRC_LIST += $(wildcard $(LOCAL_PATH)/3d/util/*.cpp)
MY_SRC_LIST += $(wildcard $(LOCAL_PATH)/3d/view/*.cpp)
MY_SRC_LIST += $(wildcard $(LOCAL_PATH)/platform/Android/*.cpp)

# Collect all header files
MY_INCLUDE_LIST += $(wildcard $(LOCAL_PATH)/*.h)
MY_INCLUDE_LIST += $(wildcard $(LOCAL_PATH)/gcanvas/*.h)
MY_INCLUDE_LIST += $(wildcard $(LOCAL_PATH)/png/*.h)
MY_INCLUDE_LIST += $(wildcard $(LOCAL_PATH)/png/thirdparty/*.h)
MY_INCLUDE_LIST += $(wildcard $(LOCAL_PATH)/support/*.h)
MY_INCLUDE_LIST += $(wildcard $(LOCAL_PATH)/memory/*.h)
MY_INCLUDE_LIST += $(wildcard $(LOCAL_PATH)/3d/*.h)
MY_INCLUDE_LIST += $(wildcard $(LOCAL_PATH)/3d/jsc/*.h)
MY_INCLUDE_LIST += $(wildcard $(LOCAL_PATH)/3d/util/*.h)
MY_INCLUDE_LIST += $(wildcard $(LOCAL_PATH)/3d/view/*.h)

# Source files to be compiled
LOCAL_SRC_FILES := $(patsubst $(LOCAL_PATH)/%, %, $(MY_SRC_LIST))

# Header files to be included
LOCAL_INCLUDE_FILES := $(patsubst $(LOCAL_PATH)/%, %, $(MY_INCLUDE_LIST))

LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/gcanvas/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/gcanvas/android/

LOCAL_LDLIBS := -Wl,--gc-sections -lGLESv1_CM -lGLESv2 -ldl -llog -landroid -lEGL -ljnigraphics -latomic

# use -Os insead of -O3 if the size is sensitive -fvisibility=hidden
LOCAL_CFLAGS += -Os -ffunction-sections -fdata-sections -DFT2_BUILD_LIBRARY -D_STLP_USE_NEWALLOC -D_STLP_USE_STATIC_LIB -D_STLP_NO_MOVE_SEMANTIC
LOCAL_CPPFLAGS += ${LOCAL_CFLAGS} -std=c++14 -frtti
#LOCAL_LDFLAGS += -Wl,--gc-sections

# enable neon for arm
ifeq ($(ENABLE_NEON),1)
ifeq ($(TARGET_ARCH),arm)
	LOCAL_CFLAGS += -DARM_NEON_OPT -mfloat-abi=softfp -mfpu=neon
endif
endif

# FreeType prebuilt
LOCAL_SHARED_LIBRARIES := freetype-prebuilt


LOCAL_STATIC_LIBRARIES := cpufeatures
include $(BUILD_SHARED_LIBRARY)

include $(LOCAL_PATH)/freetype-prebuilt/Android.mk

$(call import-module,android/cpufeatures)




