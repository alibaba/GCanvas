# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14  -fno-unwind-tables -fexceptions -fprofile-arcs -ftest-coverage")
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)
include_directories("../../src/")
include_directories("../../src/gcanvas")
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
        ../../src/gcanvas/GFrameBufferObject.cpp
        ../../src/gcanvas/GGlyphCache.cpp
        ../../src/gcanvas/GPath.cpp
        ../../src/gcanvas/GShader.cpp
        ../../src/gcanvas/GShaderManager.cpp
        ../../src/gcanvas/GStrSeparator.cpp
        ../../src/gcanvas/GTexture.cpp
        ../../src/gcanvas/GTreemap.cpp
 
        # # platform srcs
        ../../src/platform/Android/GCanvas2DContextImpl.cpp
       ../../src/platform/Linux/GFontManager.cpp
       ../../src/platform/Linux/GFont.cpp
       ../../src/platform/Linux/GFontCache.cpp
       ../../src/platform/Linux/GSystemFontInformation.cpp
       ../../src/platform/Linux/GFontFamily.cpp
        # ../../src/platform/Android/GCanvas2DContextAndroid.cpp
        # ../../src/platform/Android/GCanvasAndroid.cpp
        # ../../src/platform/Android/GFont.cpp
        # ../../src/platform/Android/GFontCache.cpp
        # ../../src/platform/Android/GFontFamily.cpp
        # ../../src/platform/Android/GFontManagerAndroid.cpp
        # ../../src/platform/Android/GFontManagerImpl.cpp

        ../../src/platform/Android/GFrameBufferObjectImpl.cpp

        # ../../src/platform/Android/GPreCompiledShaders.cpp
        # ../../src/platform/Android/GSystemFontInformation.cpp
        # ../../src/platform/Android/GFontManagerAndroid.cpp

        # support
        ../../src/support/CharacterSet.cpp
        ../../src/support/Encode.cpp
        ../../src/support/FileUtils.cpp
        ../../src/support/GLUtil.cpp
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

