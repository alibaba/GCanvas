# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fno-exceptions -fno-unwind-tables")

add_definitions(-DFT2_BUILD_LIBRARY)
add_definitions(-DANDROID)

if ((${ANDROID_ARM_NEON}) AND (${ANDROID_ABI} MATCHES arm*))
    add_definitions(-DARM_NEON_OPT)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=softfp -mfpu=neon")
endif ()


include_directories("./src/")
include_directories("./src/gcanvas")
include_directories("./src/support")
include_directories("./src/platform/Android")
include_directories("./android/")
include_directories("./android/png")
include_directories("./android/png/thirdparty")
include_directories("./android/memory")
include_directories("./android/3d")
include_directories("./android/3d/jsc")
include_directories("./android/3d/util")
include_directories("./android/3d/view")
include_directories("./android/freetype-prebuilt/include")


set(SRC_FILES
        #root srcs
        ./src/GCanvas.cpp
        ./src/GCanvasManager.cpp
        ./src/GCanvasWeex.cpp # todo remove

        # gcanvas srcs
        ./src/gcanvas/GCanvas2dContext.cpp
        ./src/gcanvas/GCanvasState.cpp
        ./src/gcanvas/GConvert.cpp
        ./src/gcanvas/GFontStyle.cpp
        ./src/gcanvas/GFrameBufferObject.cpp
        ./src/gcanvas/GGlyphCache.cpp
        ./src/gcanvas/GPath.cpp
        ./src/gcanvas/GShader.cpp
        ./src/gcanvas/GShaderManager.cpp
        ./src/gcanvas/GStrSeparator.cpp
        ./src/gcanvas/GTexture.cpp
        ./src/gcanvas/GTreemap.cpp
        ./src/gcanvas/GWebglContext.cpp

        # platform srcs
        ./src/platform/Android/GCanvas2DContextImpl.cpp

        ./src/platform/Android/GCanvas2DContextAndroid.cpp
        ./src/platform/Android/GCanvasAndroid.cpp
        ./src/platform/Android/GFont.cpp
        ./src/platform/Android/GFontCache.cpp
        ./src/platform/Android/GFontFamily.cpp
        ./src/platform/Android/GFontManagerAndroid.cpp
        ./src/platform/Android/GFontManagerImpl.cpp

        ./src/platform/Android/GFrameBufferObjectImpl.cpp

        ./src/platform/Android/GPreCompiledShaders.cpp
        ./src/platform/Android/GSystemFontInformation.cpp
        ./src/platform/Android/GFontManagerAndroid.cpp

        # support
        ./src/support/CharacterSet.cpp
        ./src/support/Encode.cpp
        ./src/support/FileUtils.cpp
        ./src/support/GLUtil.cpp
        ./src/support/Log.cpp
        ./src/support/Util.cpp
        ./src/support/Value.cpp )


if (GCANVAS_WEEX)
    add_definitions(-DGCANVAS_WEEX)
    file(GLOB android_root_src "./android/*.cpp")
    file(GLOB native_png_src "./android/png/*.cpp")
    file(GLOB native_png_thirdparty_src "./android/png/thirdparty/*.c")
    file(GLOB native_3d_src "./android/3d/*.cpp")
    file(GLOB native_3d_jsc_src "./android/3d/jsc/*.cpp")
    file(GLOB native_3d_util_src "./android/3d/util/*.cpp")
    file(GLOB native_3d_view_src "./android/3d/view/*.cpp")
    file(GLOB native_android_manager_src "./android/manager/*.cpp")
else()
    message(" not enter the gcanvas_weex >>>>>>>>>> ")
    list(APPEND SRC_FILES ./android/GCanvasJNI.cpp)
endif()


if (GCANVAS_RUNTIME)
    add_definitions(-DGCANVAS_RUNTIME)
endif()



add_library( # Sets the name of the library.
        gcanvas
        # Sets the library as a shared library.
        SHARED
        # Provides a relative path to your source file(s).
        ${native_png_src}
        ${native_png_thirdparty_src}
        ${native_3d_src}
        ${native_3d_jsc_src}
        ${native_3d_util_src}
        ${native_3d_view_src}
        ${android_root_src}
        ${native_android_manager_src}
        ${SRC_FILES} )


find_library( # Sets the name of the path variable.
        log-lib
        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)


add_library(freetype SHARED IMPORTED)
set_target_properties(freetype PROPERTIES IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/android/freetype-prebuilt/libs/${ANDROID_ABI}/libfreetype.so)


target_link_libraries( # Specifies the target library.
        gcanvas
        GLESv1_CM
        GLESv2
        android
        EGL
        atomic
        jnigraphics
        freetype
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})

