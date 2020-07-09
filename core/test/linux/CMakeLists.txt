# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14  -fno-unwind-tables -fexceptions -fprofile-arcs -ftest-coverage")
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)
include_directories("../../src/")
include_directories("../../src/gcanvas")
include_directories("../../src/gcanvas/GL/")
include_directories("../../src/support")
include_directories("./util/")
include_directories("../../src/platform/Android")
include_directories("../../src/platform/Linux")
include_directories(".")
include_directories("./freetype2/")

set(SRC_FILES
        #root srcs
        # ./mainOnScreen.cpp
        ./mainOffScreen.cpp
        ./GBenchMark.cpp
        ./util/lodepng.cc
        ./util/util.cc
        ./util/testCases.cc
        ../../src/GCanvas.cpp        
        ../../src/GCanvasManager.cpp
        ../../src/GCanvasWeex.cpp # todo remove

        # gcanvas srcs
        ../../src/gcanvas/GCanvas2dContext.cpp
        ../../src/gcanvas/GCanvasState.cpp
        ../../src/gcanvas/GConvert.cpp
        ../../src/gcanvas/GFontStyle.cpp
        ../../src/gcanvas/GFontManager.cpp
        ../../src/gcanvas/GL/GFrameBufferObject.cpp
        ../../src/gcanvas/GGlyphCache.cpp
        ../../src/gcanvas/GPath.cpp
        ../../src/gcanvas/GPath2D.cpp
        ../../src/gcanvas/GPathStroker.cpp
        ../../src/gcanvas/GL/GShader.cpp
        ../../src/gcanvas/GL/GShaderManager.cpp
        ../../src/gcanvas/GStrSeparator.cpp
        ../../src/gcanvas/GL/GTexture.cpp
        ../../src/gcanvas/GTreemap.cpp
 
        # # platform srcs

       ../../src/platform/Linux/GFontManagerLinux.cpp
       ../../src/platform/Linux/GFontLinux.cpp
       ../../src/platform/Linux/GFontCacheLinux.cpp
        ../../src/platform/Android/GFrameBufferObjectImpl.cpp
        ../../src/platform/Android/GFreeTypeWrap.cpp
        ../../src/platform/Android/GCanvas2DContextImpl.cpp


        # support
        ../../src/support/CharacterSet.cpp
        ../../src/support/Encode.cpp
        ../../src/support/FileUtils.cpp
        ../../src/gcanvas/GL/GLUtil.cpp
        ../../src/support/Log.cpp
        ../../src/support/Util.cpp
        ../../src/support/Value.cpp )



ADD_EXECUTABLE(gcanvasTest ${SRC_FILES} )



target_link_libraries( # Specifies the target library.
        gcanvasTest
        GLESv2
        EGL
        atomic
        freetype
        glfw
        boost_system
        )
project(gcanvasTest)