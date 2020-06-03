//
// Created by yuantong on 2018/4/17.
//

#include "GEGLContext.h"
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include "log/Log.h"
#include <string>


EGLint GEGLContext::SwapBuffer() {
    if (eglSurface == EGL_NO_SURFACE) {
        return false;
    }
    EGLBoolean b = eglSwapBuffers(eglDisplay, eglSurface);
    if (!b) {
        EGLint err = eglGetError();
        if (err == EGL_BAD_SURFACE) {
            // Recreate surface
            InitEGLSurface();
            return EGL_SUCCESS;  // Still consider glContext is valid
        } else if (err == EGL_CONTEXT_LOST || err == EGL_BAD_CONTEXT) {
            // Context has been lost!!
            isContextValid = false;
            Terminate();
            InitEGLContext();
        }
        return err;
    }
    return EGL_SUCCESS;
}

GEGLContext::GEGLContext() : nWindow(NULL),
                                         eglDisplay(EGL_NO_DISPLAY),
                                         eglSurface(EGL_NO_SURFACE),
                                         eglContext(EGL_NO_CONTEXT),
                                         width(0),
                                         height(0),
                                         isEGLInit(false),
                                         isGLESInit(false),
                                         isES3Supported(false) {}

bool GEGLContext::Init(ANativeWindow *window) {
    if (isEGLInit) {
        return true;
    }
    nWindow = window;
    InitEGLSurface();
    InitEGLContext();
    InitGLES();

    isEGLInit = true;

    return true;
}


bool GEGLContext::CheckHasExtension(const char *extension) {
    if (extension == NULL) {
        return false;
    }

    std::string extensions = std::string((char *) glGetString(GL_EXTENSIONS));
    std::string str = std::string(extension);
    str.append(" ");
    size_t pos = 0;
    return extensions.find(extension, pos) != std::string::npos;

}

bool GEGLContext::InitEGLContext() {
    const EGLint context_attribs[] = {EGL_CONTEXT_CLIENT_VERSION,
                                      2,  // Request opengl ES2.0
                                      EGL_NONE};
    eglContext = eglCreateContext(eglDisplay, eglConfig, NULL, context_attribs);

    if (eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext) == EGL_FALSE) {
        LOGW("Unable to eglMakeCurrent");
        return false;
    }

    isContextValid = true;
    return true;
}

bool GEGLContext::InitEGLSurface() {
    eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
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

    eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, nWindow, NULL);
    eglQuerySurface(eglDisplay, eglSurface, EGL_WIDTH, &width);
    eglQuerySurface(eglDisplay, eglSurface, EGL_HEIGHT, &height);

    return true;
}

void GEGLContext::InitGLES() {
    // Now we do not support GLES 3.0
    if (isGLESInit) {
        return;
    }
    glVersion = 2.0;

    isGLESInit = true;

    isES3Supported = false;
}

bool GEGLContext::Invalidate() {
    Terminate();

    isEGLInit = false;
    return true;
}

void GEGLContext::Pause() {
    if (eglSurface != EGL_NO_SURFACE) {
        eglDestroySurface(eglDisplay, eglSurface);
        eglSurface = EGL_NO_SURFACE;
    }
}

EGLint GEGLContext::Resume(ANativeWindow *window) {
    if (!isEGLInit) {
        Init(window);
        return EGL_SUCCESS;
    }

    int32_t original_widhth = width;
    int32_t original_height = height;

    // Create surface
    nWindow = window;
    eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, nWindow, NULL);
    eglQuerySurface(eglDisplay, eglSurface, EGL_WIDTH, &width);
    eglQuerySurface(eglDisplay, eglSurface, EGL_HEIGHT, &height);

    if (width != original_widhth || height != original_height) {
        // Screen resized
        LOGI("Screen resized");
    }

    if (eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext) == EGL_TRUE) {
        return EGL_SUCCESS;
    }

    EGLint err = eglGetError();
    LOGW("Unable to eglMakeCurrent %d", err);

    if (err == EGL_CONTEXT_LOST) {
        // Recreate context
        LOGI("Re-creating egl context");
        InitEGLContext();
    } else {
        // Recreate surface
        Terminate();
        InitEGLSurface();
        InitEGLContext();
    }

    return err;
}


bool GEGLContext::Resize(int32_t width, int32_t height) {
    if (this->width == width && this->height == height) {
        return true;
    }

    ClearCurrent();

    if (eglSurface != EGL_NO_SURFACE) {
        eglDestroySurface(eglDisplay, eglSurface);
    }

    eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, nWindow, NULL);
    if(eglSurface == EGL_NO_SURFACE){
        LOGE("Resize failed! create egl surface returns EGL_NO_SURFACE");
        return false;
    }

    eglQuerySurface(eglDisplay, eglSurface, EGL_WIDTH, &width);
    eglQuerySurface(eglDisplay, eglSurface, EGL_HEIGHT, &height);

    MakeCurrent();

    return true;
}

bool GEGLContext::ClearCurrent() {
    return eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE,
                          EGL_NO_CONTEXT) == EGL_TRUE;
}


bool GEGLContext::MakeCurrent() {
    return eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext) == EGL_TRUE;
}


void GEGLContext::Terminate() {
    if (eglDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (eglContext != EGL_NO_CONTEXT) {
            eglDestroyContext(eglDisplay, eglContext);
        }

        if (eglSurface != EGL_NO_SURFACE) {
            eglDestroySurface(eglDisplay, eglSurface);
        }
        eglTerminate(eglDisplay);
    }

    eglDisplay = EGL_NO_DISPLAY;
    eglContext = EGL_NO_CONTEXT;
    eglSurface = EGL_NO_SURFACE;
    nWindow = nullptr;
    isContextValid = false;
}

GEGLContext::~GEGLContext() { Terminate(); }