//
// Created by yuantong on 2018/10/26.
//

#include <support/Log.h>
#include "GEGLContext.h"


EGLContext currentEGLContext = nullptr;


GSharedEGLContext::GSharedEGLContext() {
    eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    assert(eglDisplay != EGL_NO_DISPLAY);
    assert(eglInitialize(eglDisplay, NULL, NULL));

    EGLConfig mEGLConfig;

    const EGLint configAttrs[] = {
            EGL_RENDERABLE_TYPE,
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
                                  EGL_DEPTH_SIZE, 24,
                                  EGL_STENCIL_SIZE, 8,
                                  EGL_SAMPLES, 4,
                                  EGL_SAMPLE_BUFFERS, 1,
                                  EGL_NONE};

    EGLint num_configs;
    eglChooseConfig(eglDisplay, configAttrs, &mEGLConfig, 1, &num_configs);

    if (!num_configs) {
        // Fall back to 16bit depth buffer
        const EGLint configAttrs[] = {EGL_RENDERABLE_TYPE,
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
                                      EGL_STENCIL_SIZE, 8,
                                      EGL_NONE};
        eglChooseConfig(eglDisplay, configAttrs, &mEGLConfig, 1, &num_configs);
    }  else {
        //LOGW("GLSharedGLContext SetupConfig OK");
    }

    assert(num_configs);

    EGLint context_attribs[] = {EGL_CONTEXT_CLIENT_VERSION,
                                3,  // Request opengl ES Version
                                EGL_NONE};
    eglContext = eglCreateContext(eglDisplay, mEGLConfig, NULL, context_attribs);
    if (eglContext == EGL_NO_CONTEXT) {
        context_attribs[1] = 2;
        eglContext = eglCreateContext(eglDisplay, mEGLConfig, NULL, context_attribs);
    }

    if (eglContext == EGL_NO_CONTEXT) {
//        TRACE_EXCEPTION("eglCreateContext_fail", "GSharedEGLContext eglError=%x", eglGetError());
    }

    assert(eglContext != EGL_NO_CONTEXT);
}


bool GSharedEGLContext::MakeCurrent() {
//    if (eglContext != nullptr && currentEGLContext == eglContext) {
//        return true;
//    }

    if (eglContext != nullptr) {
        EGLContext p = eglGetCurrentContext();
        if (eglContext == p) {
            return true;
        }
    }

    EGLBoolean result = eglMakeCurrent(eglDisplay, NULL, NULL, eglContext);
    if (result != EGL_TRUE) {
        EGLint error = eglGetError();
        EGL_LOGE("SharedEGLContext(%p) MakeCurrent:error %x", this, error);
        return false;
    } else {
        currentEGLContext = eglContext;
        return true;
    }
}


GSharedEGLContext::~GSharedEGLContext() {
    if (eglDisplay != EGL_NO_DISPLAY) {
        ClearCurrent();

        if (!eglDestroyContext(eglDisplay, eglContext)) {
            EGL_LOGE("SharedEGLContext:destroy context error");
        }
        if (!eglTerminate(eglDisplay)) {
            EGL_LOGE("SharedEGLContext:destroy display error");
        }
    }
    eglContext = EGL_NO_CONTEXT;
    eglDisplay = EGL_NO_DISPLAY;
}


EGLint GSharedEGLContext::SwapBuffer() {
    // ignore
}


bool GSharedEGLContext::IsPreserveBackBuffer() {
    return false;
};