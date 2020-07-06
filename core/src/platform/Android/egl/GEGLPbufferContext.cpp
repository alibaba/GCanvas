//
// Created by yuantong on 2018/4/17.
//

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <string>
#include <support/Log.h>

#include "GEGLContext.h"


GEGLPbufferContext::GEGLPbufferContext(EGLDisplay sharedDisplay, EGLContext sharedContext,
                                       int w, int h) : GEGLContext(sharedDisplay, sharedContext),
                                                       isGLESInit(false),
                                                       isES3Supported(false) {
    width = w;
    height = h;
}

GEGLPbufferContext::GEGLPbufferContext(int w, int h) :
        isGLESInit(false),
        isES3Supported(false) {
    width = w;
    height = h;
}


bool GEGLPbufferContext::Init(ANativeWindow *window) {
    if (isEGLInit) {
        return true;
    }
    InitEGLSurface();
    InitEGLContext();
    InitGLES();

    isEGLInit = true;

    return true;
}


bool GEGLPbufferContext::InitEGLContext() {
    EGLint version = 3;

    EGLint context_attribs[] = {EGL_CONTEXT_CLIENT_VERSION,
                                version,  // Request opengl ES Version
                                EGL_NONE};
    bool is3 = false;
    eglContext = eglCreateContext(eglDisplay, eglConfig, eglSharedContext, context_attribs);
    if (eglContext == EGL_NO_CONTEXT) {
        is3 = false;
        version = 2;
        context_attribs[1] = version;
        eglContext = eglCreateContext(eglDisplay, eglConfig, eglSharedContext, context_attribs);
    }

    glVersion = version;
    isES3Supported = is3;


    // 切换到非shared模式，因为部分设备创建的基于sharedContext的Pbuffer数量较少
    if (eglContext == EGL_NO_CONTEXT) {
        eglSharedContext = nullptr;
        eglContext = eglCreateContext(eglDisplay, eglConfig, nullptr, context_attribs);
    }

    if (eglContext == EGL_NO_CONTEXT) {
        EGLint err = eglGetError();
//        TRACE_EXCEPTION("eglCreateContext_fail",
//                        "GLPbufferContext.InitEGLContext, eglGetError=%x, isSharedContext=%i",
//                        err, (eglSharedContext == nullptr) ? 0 : 1);
        return false;
    }

    return MakeCurrent();
}


bool GEGLPbufferContext::InitEGLSurface() {
    if (eglDisplay == EGL_NO_DISPLAY) {
        eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    }

    eglInitialize(eglDisplay, 0, 0);

    const EGLint attribs[] = {EGL_RENDERABLE_TYPE,
                              EGL_OPENGL_ES2_BIT,
                              EGL_SURFACE_TYPE,
                              EGL_WINDOW_BIT,
                              EGL_ALPHA_SIZE,
                              8,
                              EGL_BLUE_SIZE,
                              8,
                              EGL_GREEN_SIZE,
                              8,
                              EGL_RED_SIZE,
                              8,
                              EGL_DEPTH_SIZE,
                              24,
                              EGL_STENCIL_SIZE,
                                  8,
            // 开启msaa
            EGL_SAMPLE_BUFFERS, 1,
            EGL_SAMPLES, 4,
                              EGL_NONE};
    colorSize = 8;
    depthSize = 24;

    EGLint num_configs;
    eglChooseConfig(eglDisplay, attribs, &eglConfig, 1, &num_configs);

    if (!num_configs) {
        // Fall back to 16bit depth buffer
        const EGLint attribs[] = {EGL_RENDERABLE_TYPE,
                                  EGL_OPENGL_ES2_BIT,
                                  EGL_SURFACE_TYPE,
                                  EGL_WINDOW_BIT,
                                  EGL_ALPHA_SIZE,
                                  8,
                                  EGL_BLUE_SIZE,
                                  8,
                                  EGL_GREEN_SIZE,
                                  8,
                                  EGL_RED_SIZE,
                                  8,
                                  EGL_DEPTH_SIZE,
                                  16,
                                  EGL_STENCIL_SIZE,
                                  8,
                                  EGL_NONE};
        eglChooseConfig(eglDisplay, attribs, &eglConfig, 1, &num_configs);
        depthSize = 16;
    }

    if (!num_configs) {
        return false;
    }

    EGLint pbufAttribs[] =
            {
                    EGL_WIDTH, width,
                    EGL_HEIGHT, height,
                    EGL_LARGEST_PBUFFER, EGL_TRUE,
                    EGL_NONE
            };

    eglSurface = eglCreatePbufferSurface(eglDisplay, eglConfig, pbufAttribs);
    if (eglSurface == EGL_NO_SURFACE) {
//        TRACE_EXCEPTION("eglCreatePbufferSurface_fail",
//                        "GLPbufferContext.%s, eglGetError=%x, w=%i,h=%i",
//                        "InitEGLSurface", eglGetError(), width, height);
        return false;
    }
    return true;
}

void GEGLPbufferContext::InitGLES() {
    if (isGLESInit) {
        return;
    }
    glVersion = 2.0;
    isGLESInit = true;
    isES3Supported = false;
}


bool GEGLPbufferContext::Invalidate() {
    Terminate();
    isEGLInit = false;
    return true;
}


void GEGLPbufferContext::Pause() {
    ClearCurrent();

    if (eglSurface != EGL_NO_SURFACE) {
        eglDestroySurface(eglDisplay, eglSurface);
        eglSurface = EGL_NO_SURFACE;
    }
}


bool GEGLPbufferContext::MakeCurrent() {
//    if (eglContext != nullptr && currentEGLContext == eglContext) {
//        return true;
//    }

    if (eglContext != nullptr) {
        EGLContext p = eglGetCurrentContext();
        if (eglContext == p) {
            return true;
        }
    }

    EGLBoolean result = eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
    if (result != EGL_TRUE) {
        EGLint error = eglGetError();
//        TRACE_EXCEPTION("eglMakeCurrent_fail", "GEGLPbufferContext, eglGetError=%x", error);
        return false;
    } else {
        currentEGLContext = eglContext;
        return true;
    }
}


/**
 * 离屏canavs 默认使用FBO，可提高drawCanvas效率
 */
bool GEGLPbufferContext::IsPreserveBackBuffer() {
    return false;
}



void GEGLPbufferContext::Terminate() {
    if (eglDisplay != EGL_NO_DISPLAY) {
        ClearCurrent();

        if (eglContext != EGL_NO_CONTEXT) {
            eglDestroyContext(eglDisplay, eglContext);
        }

        if (eglSurface != EGL_NO_SURFACE) {
            eglDestroySurface(eglDisplay, eglSurface);
        }

        if (!IsSharingContext()) {
            eglTerminate(eglDisplay);
            eglDisplay = EGL_NO_DISPLAY;
        }
    }

    eglContext = EGL_NO_CONTEXT;
    eglSurface = EGL_NO_SURFACE;
}




GEGLPbufferContext::~GEGLPbufferContext() { Terminate(); }

EGLint GEGLPbufferContext::SwapBuffer() { return EGL_SUCCESS; }