//
// Created by yuantong on 2018/4/17.
//

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <string>
#include <support/Log.h>

#include "GEGLContext.h"





EGLint GEGLWindowContext::SwapBuffer() {
    if (!IsValid()) {
        // EGL_LOGI("swapBuffer:fail:EGL invalid");
        return false;
    }
    // EGL_LOGI("swapBuffer:start");
    EGLBoolean b = eglSwapBuffers(eglDisplay, eglSurface);
    if (!b) {
        EGLint err = eglGetError();
        // mymock
        // 所有的swapBuffer fail的都trace下来 (有可能单个错误，导致数据爆多?)
        // TRACE_EXCEPTION("eglSwapBuffers_fail", "eglGetError=%x", err);
        if (err == EGL_BAD_SURFACE) { // 不要尝试恢复，任其报错
            // DestroyEGLSurfaceIf();
            // InitEGLSurface();
            return err;
        } else if (err == EGL_CONTEXT_LOST || err == EGL_BAD_CONTEXT) {
            EGL_LOGI("Recreate EGLContext on ContextLost|BadContext");
            Terminate();
            InitEGLContext();
        }
        return err;
    }
    return EGL_SUCCESS;
}


GEGLWindowContext::GEGLWindowContext() : nWindow(NULL),
                                         isGLESInit(false),
                                         isES3Supported(false) {}

GEGLWindowContext::GEGLWindowContext(EGLDisplay sharedDisplay, EGLContext sharedContext) :
                                                                                        GEGLContext(sharedDisplay, sharedContext),
                                                                                        nWindow(NULL),
                                                                                           isGLESInit(false),
                                                                                           isES3Supported(
                                                                                                   false) {}

bool GEGLWindowContext::Init(ANativeWindow *window) {
    if (isEGLInit) {
        return true;
    }
    nWindow = window;
    // 初始化eglDisplay & eglSurface(eglSurface和eglContext无关联)
    bool flag = InitEGLSurface();
    if (flag) {
        flag = InitEGLContext();
    }
    // 标记已经执行过初始化(但有可能初始化失败)
    isEGLInit = true;

    return flag;
}


bool GEGLWindowContext::InitEGLSurface() {
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

    EGLint s;
    eglGetConfigAttrib(eglDisplay, eglConfig, EGL_STENCIL_SIZE, &s);

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

    std::string scene = "InitEGLSurface";
    eglSurface = CreateWindowSurface(scene);
    return eglSurface != EGL_NO_SURFACE;
}


bool GEGLWindowContext::InitEGLContext() {
    EGL_LOGI("InitEGLContext:start");

    EGLint targetVersion = 3;
    EGLint context_attribs[] = {EGL_CONTEXT_CLIENT_VERSION,
                                targetVersion,  // Request opengl ES Version
                                EGL_NONE};
    bool is3 = false;
    eglContext = eglCreateContext(eglDisplay, eglConfig, eglSharedContext, context_attribs);
    if (eglContext == EGL_NO_CONTEXT) {
        is3 = false;
        targetVersion = 2;
        context_attribs[1] = targetVersion;
        eglContext = eglCreateContext(eglDisplay, eglConfig, eglSharedContext, context_attribs);
    }
    glVersion = targetVersion;//is3 ? 3.0f : 2.0f;
    isES3Supported = is3;

    if (eglContext == EGL_NO_CONTEXT) {
        EGLint err = eglGetError();
        // TRACE_EXCEPTION("eglCreateContext_fail", "GLWindowContext.InitEGLContext, eglGetError=%x",
        //                err);
        return false;
    }

    // 初始化EGLContext时 强制MakeCurrent
    EGLint result = MakeCurrentWithScene("InitEGLContext", false);
    EGL_LOGI("InitEGLContext:end,result=%i", (result == EGL_SUCCESS));

    return result == EGL_SUCCESS;
}


bool GEGLWindowContext::Invalidate() {
    Terminate();
    isEGLInit = false;
    return true;
}


/**
 * 暂停，将触发删除EGL Surface
 * FIXME 命名有点问题, 外部实际用来DestroyEGLSurface
 */
void GEGLWindowContext::Pause() {
    DestroyEGLSurfaceIf();
    nWindow = nullptr;
}


void GEGLWindowContext::DestroyEGLSurfaceIf() {
    // 1.先clear current，后续gl操作都不起作用(避免gl报错)
    ClearCurrent();

    // 2.destroy eglSurface(未current的surface将立即删除)
    if (eglSurface != EGL_NO_SURFACE) {
        EGLBoolean result = eglDestroySurface(eglDisplay, eglSurface);
        eglSurface = EGL_NO_SURFACE;
        EGL_LOGI("DestroySurface:eglDestroySurface()=%i", result);
    }
}


EGLint GEGLWindowContext::Resume(ANativeWindow *window) {
    EGL_LOGI("Resume:window=%p", window);
    if (!isEGLInit) {
        Init(window);
        return EGL_SUCCESS;
    }

    // 如果egl surface有效，有可能是脏数据，先destroy surface(保证resume执行成功)
    if (eglSurface != EGL_NO_SURFACE) {
        DestroyEGLSurfaceIf();
    }

    int32_t original_width = width;
    int32_t original_height = height;

    // 更新window
    nWindow = window;

    std::string scene = "Resume";
    eglSurface = CreateWindowSurface(scene);
    if (width != original_width || height != original_height) {
        EGL_LOGI("Resume:Surface resized");
    }

    EGLint result = MakeCurrentWithScene(scene.data());
    if (result == EGL_SUCCESS) {
        EGL_LOGI("Resume:Success:eglSurface=%p", eglSurface);
        return result;
    }

    // makeCurrent失败，有可能是context有问题
    EGL_LOGI("Resume:MakeCurrent Fail");
    if (result == EGL_CONTEXT_LOST) {
        // Recreate context
        EGL_LOGI("Recreating egl context");
        return InitEGLContext();
    } else {
        // Recreate surface
        EGL_LOGI("Recreating Surface when Resume Fail");
        Terminate();
        InitEGLSurface();
        bool flag = InitEGLContext();
        return flag;
    }
}


bool GEGLWindowContext::Resize(int32_t newWidth, int32_t newHeight) {
    // 只要是Resize调用，即使尺寸一样，也强制触发Resize逻辑(有可能Surface已经发生变化)
    EGL_LOGI("Resize:w=%i,h=%i", newWidth, newHeight);
    // 1.Destroy EGLSurface
    DestroyEGLSurfaceIf();

    // 2.Create New EGLSurface
    std::string scene = "Resize";
    eglSurface = CreateWindowSurface(scene);
    if (eglSurface == EGL_NO_SURFACE) {
        return false;
    }

    // 3.MakeCurrent
    EGLint result = MakeCurrentWithScene(scene.data());
    EGL_LOGI("Resize:finish,makeCurrent result=%i", result);

    return result == EGL_SUCCESS;
}


bool GEGLWindowContext::MakeCurrent() {
    return MakeCurrentWithScene("Render") == EGL_SUCCESS;
}


// surface 状态变更
EGLint GEGLWindowContext::MakeCurrentWithScene(const char *scene, bool check) {
//    if (eglContext != nullptr && currentEGLContext == eglContext) {
//        return true;
//    }

    // 判断是否当前
    if (check && eglContext != nullptr) {
        EGLContext p = eglGetCurrentContext();
        EGLSurface surf = eglGetCurrentSurface(EGL_DRAW);
        if (eglContext == p && surf == eglSurface) {
            return EGL_SUCCESS;
        }
    }

    // 当View Destroy时，先执行DestroySurface()，然后执行canvas core的回收，这时会需要调用gl接口，
    // 这种情况下 MakeCurrent 能正常工作吗?
    // 1.preserve backbuffer不可以，非pbb可以
    EGLBoolean result = eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
    // EGL_LOGI("eglMakeCurrent: surface=%p, context=%p, eglGetError=%x", eglSurface, eglContext, eglGetError());
    if (result != EGL_TRUE) {
        EGLint error = eglGetError();
        // TRACE_EXCEPTION("eglMakeCurrent_fail", "GLWindowContext.%s,eglGetError=%x", scene, error);
        return error;
    } else {
        currentEGLContext = eglContext;
        return EGL_SUCCESS;
    }
}


EGLSurface GEGLWindowContext::CreateWindowSurface(std::string &scene) {
    if (nWindow == nullptr) { // 稳定性保护，避免nWindow为null时crash
        // TRACE_EXCEPTION("CreateWindowSurface_fail", "Scene=%s,window is null", scene.data());
        return EGL_NO_SURFACE;
    }

    EGL_LOGI("CreateWindowSurface:start:eglDisplay=%p,nWindow=%p,scene=%s", eglDisplay, nWindow,
             scene.data());
    EGLSurface createdSurface = eglCreateWindowSurface(eglDisplay, eglConfig, nWindow, NULL);
    if (createdSurface == EGL_NO_SURFACE) { // 创建surface失败
        EGLint error = eglGetError();
        EGL_LOGI("CreateWindowSurface:fail:eglSurface=%p,eglError=%x", createdSurface, error);
        if (error == EGL_BAD_ALLOC && scene == "resume") {
            // resume情况下surface可能已经绑定在nWindow上，此时不需要上报错误
        } else if (error != EGL_SUCCESS) { // 注意：重复创建eglSurface会报EGL_BAD_ALLOC错误
//            TRACE_EXCEPTION("eglCreateWindowSurface_fail", "Scene.%s, eglGetError=%x",
//                            scene.data(), error);
        }
        return EGL_NO_SURFACE;
    } else { // 创建surface成功
        eglQuerySurface(eglDisplay, createdSurface, EGL_WIDTH, &width);
        eglQuerySurface(eglDisplay, createdSurface, EGL_HEIGHT, &height);

        if (usePreserveBackBuffer) {
            eglGetError(); // 清空error?
            eglSurfaceAttrib(eglDisplay, createdSurface, EGL_SWAP_BEHAVIOR, EGL_BUFFER_PRESERVED);
            EGLint error = eglGetError();
            if (error != EGL_SUCCESS) {
//                TRACE_EXCEPTION("eglPreserveBackBuffer_fail",
//                                "GLWindowContext.%s, eglGetError=%x",
//                                scene.c_str(), eglGetError());
            }
        }
        EGL_LOGI("CreateWindowSurface:success:eglSurface=%p,preserveBB=%i,egl size(%i,%i)", createdSurface,
                 usePreserveBackBuffer, width, height);
    }

    return createdSurface;
}


bool GEGLWindowContext::IsPreserveBackBuffer() {
    EGLint value;

    eglGetError();
    eglQuerySurface(eglDisplay, eglSurface, EGL_SWAP_BEHAVIOR, &value);

    EGLint error = eglGetError();
    if (error != EGL_SUCCESS) {
//        TRACE_EXCEPTION("QueryPreserveBackBuffer_fail", "GLWindowContext, eglGetError=%x", error);
    }

    return error == EGL_SUCCESS && value == EGL_BUFFER_PRESERVED;
}


void GEGLWindowContext::Terminate() {
    if (eglDisplay != EGL_NO_DISPLAY) {
        if (eglContext != EGL_NO_CONTEXT) {
            eglDestroyContext(eglDisplay, eglContext);
        }

        DestroyEGLSurfaceIf();
        if (!IsSharingContext()) {
            eglTerminate(eglDisplay);
            eglDisplay = EGL_NO_DISPLAY;
        }
    }

    eglContext = EGL_NO_CONTEXT;
    eglSurface = EGL_NO_SURFACE;

    nWindow = nullptr;
}


GEGLWindowContext::~GEGLWindowContext() {
    Terminate();
}
