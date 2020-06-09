# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fno-exceptions -funwind-tables")

add_definitions(-DFT2_BUILD_LIBRARY)
add_definitions(-DANDROID)

if ((${ANDROID_ARM_NEON}) AND (${ANDROID_ABI} MATCHES arm*))
    add_definitions(-DARM_NEON_OPT)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=softfp -mfpu=neon")
endif ()


#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
#set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=address")
#set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} -fsanitize=address")


include_directories("./src/")
include_directories("./src/gcanvas")
include_directories("./src/gcanvas/GL")
include_directories("./src/support")
include_directories("./src/platform/Android")
include_directories("./src/commandbuffer/")
include_directories("./src/webgl/")
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
        ./src/gcanvas/GL/GFrameBufferObject.cpp
        ./src/gcanvas/GGlyphCache.cpp
        ./src/gcanvas/GPath.cpp
        ./src/gcanvas/GPath2D.cpp
        ./src/gcanvas/GPathStroker.cpp
        ./src/gcanvas/GL/GShader.cpp
        ./src/gcanvas/GL/GShaderManager.cpp
        ./src/gcanvas/GStrSeparator.cpp
        ./src/gcanvas/GL/GTexture.cpp
        ./src/gcanvas/GTreemap.cpp
        ./src/gcanvas/GWebglContext.cpp
        ./src/gcanvas/GFontManager.cpp

        # platform srcs
        ./src/platform/Android/GCanvas2DContextImpl.cpp
        ./src/platform/Android/GCanvas2DContextAndroid.cpp
        ./src/platform/Android/GCanvasAndroid.cpp
        ./src/platform/Android/GFreeTypeWrap.cpp
        ./src/platform/Android/GFont.cpp
        ./src/platform/Android/GFontCache.cpp
        ./src/platform/Android/GFontFamily.cpp
        ./src/platform/Android/GFontManagerAndroid.cpp
        ./src/platform/Android/GFrameBufferObjectImpl.cpp
        ./src/platform/Android/GPreCompiledShaders.cpp
        ./src/platform/Android/GSystemFontInformation.cpp
        ./src/platform/Android/GFontManagerAndroid.cpp
    

        ./src/platform/Android/egl/GEGLPbufferContext.cpp
        ./src/platform/Android/egl/GEGLWindowContext.cpp
        ./src/platform/Android/egl/GSharedEGLContext.cpp


        # support
        ./src/support/CharacterSet.cpp
        ./src/support/Encode.cpp
        ./src/support/FileUtils.cpp
        ./src/gcanvas/GL/GLUtil.cpp
        ./src/support/Log.cpp
        ./src/support/Util.cpp
        ./src/support/Value.cpp

        ./src/commandbuffer/GCommandBuffer.cpp
        ./src/commandbuffer/GCommandDecoder.cpp
        ./src/webgl/GCommandDecoderWebGL.cpp
        ./src/webgl/GWebGLRenderContextInner.cpp
        ./src/webgl/GWebGLRenderContext.cpp
        )


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



#target_compile_options(gcanvas PUBLIC -fsanitize=address -fno-omit-frame-pointer)
#set_target_properties(gcanvas PROPERTIES LINK_FLAGS -fsanitize=address)


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


# ---------------------------- distribute headers  ----------------------------
if (GCANVAS_DISTRIBUTE_HEADER)
    set(DIST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/distribution)
    message ("DIST_DIR=${DIST_DIR}")
    function(copy_header_under_dir)
        set(DIR ${ARGV0})
        set(PAR ${ARGV1})
        message("copy_header_under_dir called:${DIR} - ${PAR}")
        file(GLOB TARGET_HEADERS "./${PAR}/${DIR}/*.h")
        file (GLOB TARGET_HEADERS_2 "./${PAR}/${DIR}/*.hpp")
        list (APPEND TARGET_HEADERS ${TARGET_HEADERS_2})

        foreach(header_file ${TARGET_HEADERS})
            get_filename_component(FN ${header_file} NAME)
            message("copy header file: ${header_file}, ${FN}")
            add_custom_command(TARGET gcanvas POST_BUILD
                    COMMAND "${CMAKE_COMMAND}" -E
                    copy "${header_file}" "${DIST_DIR}/gcanvas_core/include/${DIR}/${FN}"
                    COMMENT "Copying ${FN} to output directory")
        endforeach(header_file)
    endfunction()

    copy_header_under_dir("." "src")
    copy_header_under_dir("gcanvas" "src")
    copy_header_under_dir("gcanvas/GL" "src")
    copy_header_under_dir("commandbuffer" "src")
    copy_header_under_dir("webgl" "src")
    copy_header_under_dir("support" "src")
    copy_header_under_dir("platform/Android" "src")
    copy_header_under_dir("platform/Android/egl" "src")


    copy_header_under_dir("freetype-prebuilt/include" "android")
    copy_header_under_dir("freetype-prebuilt/include/freetype" "android")
    copy_header_under_dir("freetype-prebuilt/include/freetype/config" "android")
    copy_header_under_dir("freetype-prebuilt/include/freetype/internal" "android")


    # 生成freetyp-prebuilt
#    copy_header_under_dir("freetype-prebuilt/include")
#    copy_header_under_dir("freetype-prebuilt/include/freetype")
#    copy_header_under_dir("freetype-prebuilt/include/freetype/config")
#    copy_header_under_dir("freetype-prebuilt/include/freetype/internal")

endif()
