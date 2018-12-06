APP_OPTIM := release
APP_STL := gnustl_static
APP_CFLAGS += -Wall -Os
APP_ABI := armeabi-v7a
APP_PLATFORM := android-14

NDK_TOOLCHAIN_VERSION := 4.9

APP_BUILD_SCRIPT = ./Android.mk

