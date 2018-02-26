APP_OPTIM := release
APP_STL := gnustl_static
APP_CFLAGS += -Wall
APP_ABI := armeabi armeabi-v7a
APP_PLATFORM := android-9

NDK_TOOLCHAIN_VERSION := 4.9

APP_BUILD_SCRIPT = ./Android.mk

