cmake_minimum_required(VERSION 3.8)
cmake_policy(SET CMP0042 NEW)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11  -fno-unwind-tables -fexceptions")

if(CORE_DIR)
message("set core_dir")
set(CORE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${CORE_DIR}")
else()
message("not core_dir default local build")
set(CORE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../core/")
endif()
message(STATUS ${CORE_DIR})
project(canvas)
 
 # CMake JS
include_directories(${CMAKE_JS_INC})

include_directories("./util")
include_directories("./third_party")
include_directories("./third_party/font/freetype2/")
include_directories("./renderContext/")

include_directories("${CORE_DIR}")
include_directories("${CORE_DIR}src/")
include_directories("${CORE_DIR}src/gcanvas")
include_directories("${CORE_DIR}src/support")
include_directories("${CORE_DIR}src/platform/Android")
include_directories("${CORE_DIR}src/platform/Linux")

set(SOURCE_FILES 
        ./binding/Export.cc
        ./binding/Canvas.cc
        ./binding/CanvasRenderingContext2D.cc
        ./binding/CanvasGradient.cc
        ./binding/ImageData.cc
        ./binding/TextMetrics.cc
        ./binding/Image.cc
        ./binding/CanvasPattern.cc
        ./renderContext/GRenderContext.cc
        ./third_party/lodepng.cc
        ./util/NodeBindingUtil.cc
        ${CORE_DIR}src/GCanvas.cpp        
        ${CORE_DIR}src/GCanvasManager.cpp

        # gcanvas srcs
        ${CORE_DIR}src/gcanvas/GCanvas2dContext.cpp
        ${CORE_DIR}src/gcanvas/GCanvasState.cpp
        ${CORE_DIR}src/gcanvas/GConvert.cpp
        ${CORE_DIR}src/gcanvas/GFontStyle.cpp
        ${CORE_DIR}src/gcanvas/GFrameBufferObject.cpp
        ${CORE_DIR}src/gcanvas/GGlyphCache.cpp
        ${CORE_DIR}src/gcanvas/GPath.cpp
        ${CORE_DIR}src/gcanvas/GShader.cpp
        ${CORE_DIR}src/gcanvas/GShaderManager.cpp
        ${CORE_DIR}src/gcanvas/GStrSeparator.cpp
        ${CORE_DIR}src/gcanvas/GTexture.cpp
        ${CORE_DIR}src/gcanvas/GTreemap.cpp
 
        # # platform srcs
      
        ${CORE_DIR}src/platform/Linux/GFontManager.cpp
        ${CORE_DIR}src/platform/Linux/GFont.cpp
        ${CORE_DIR}src/platform/Linux/GFontCache.cpp
        ${CORE_DIR}src/platform/Linux/GSystemFontInformation.cpp
        ${CORE_DIR}src/platform/Linux/GFontFamily.cpp
        #linux can reuse the android GCanvas2DContextImpl/GFrameBufferObjectImpl
        ${CORE_DIR}src/platform/Android/GCanvas2DContextImpl.cpp
        ${CORE_DIR}src/platform/Android/GFrameBufferObjectImpl.cpp
        # support
        ${CORE_DIR}src/support/CharacterSet.cpp
        ${CORE_DIR}src/support/Encode.cpp
        ${CORE_DIR}src/support/FileUtils.cpp
        ${CORE_DIR}src/support/GLUtil.cpp
        ${CORE_DIR}src/support/Log.cpp
        ${CORE_DIR}src/support/Util.cpp
        ${CORE_DIR}src/support/Value.cpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

target_link_libraries(${PROJECT_NAME} 
                      ${CMAKE_JS_LIB}
                      GLESv2
                      EGL
                      atomic
                      glfw
                      freetype
                      boost_system
                      jpeg
                      curl)

# Include N-API wrappers
execute_process(COMMAND node -p "require('node-addon-api').include"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE NODE_ADDON_API_DIR
        )
string(REPLACE "\n" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
string(REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${NODE_ADDON_API_DIR})

# define NPI_VERSION
add_definitions(-DNAPI_VERSION=3)
