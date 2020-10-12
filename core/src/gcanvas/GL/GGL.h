/**
* Created by G-Canvas Open Source Team.
* Copyright (c) 2017, Alibaba, Inc. All rights reserved.
*
* This source code is licensed under the Apache Licence 2.0.
* For the full copyright and license information, please view
* the LICENSE file in the root directory of this source tree.
*/

#ifndef GCANVAS_GGL_h
#define GCANVAS_GGL_h

#ifndef _WIN32

#ifdef ANDROID
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <GLES2/gl2ext.h>
#include <export.h>

#elif IOS
#include <OpenGLES/gltypes.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#elif  __linux__
#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>
#include <EGL/egl.h>
#include <GLES2/gl2ext.h>
#include <string.h>
#include "FontTool.hpp"
#endif

#else
#include <GLES2/gl2.h>
#endif

#endif /* GCANVAS_GGL_h */
