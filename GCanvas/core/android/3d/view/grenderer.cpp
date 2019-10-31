//
// Created by yuantong on 2017/8/1.
//

#include <GCanvasManager.h>
#include <android/bitmap.h>
#include <support/Util.h>
#include "grenderer.h"



GRenderer::GRenderer(std::string key) : m_egl_context(0),
                                        m_egl_display(0),
                                        m_egl_surface(0),
                                        m_window(0) {
    pthread_mutex_init(&m_mutex, 0);
    sem_init(&m_SyncSem, 0, 0);
    sem_init(&m_dummy, 0, 0);
    this->mContextId = key;
}

GRenderer::~GRenderer() {
    LOG_D("~GRenderer.");
    pthread_mutex_destroy(&m_mutex);
}

void GRenderer::setNativeWindow(ANativeWindow *window) {
    m_window = window;
}

ANativeWindow *GRenderer::getNativeWindow() {
    return m_window;
}

bool GRenderer::initialize() {
    const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, 16,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_NONE
    };
    EGLDisplay display;
    EGLConfig config;
    EGLint numConfigs;
    EGLint format;
    EGLSurface surface;
    EGLContext context;
    EGLint width;
    EGLint height;
    GLfloat ratio;


    if (m_egl_display == EGL_NO_DISPLAY) {
        if ((display = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY) {
            destroy();
            LOG_D("getdisplay failed.");
            return false;
        }
        if (!eglInitialize(display, 0, 0)) {
            destroy();
            LOG_D("egl initialize failed.");
            return false;
        }

    } else {
        display = m_egl_display;
    }

    if (!eglChooseConfig(display, attribs, &config, 1, &numConfigs)) {
        LOG_D("choose config failed.");
        destroy();
        return false;
    }

    if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format)) {
        LOG_D("get config failed.");
        destroy();
        return false;
    }

    if (m_egl_surface == EGL_NO_SURFACE) {
        ANativeWindow_setBuffersGeometry(m_window, 0, 0, format);

        if (!(surface = eglCreateWindowSurface(display, config, m_window, 0))) {
            LOG_D("create surface failed.");
            destroy();
            return false;
        }

    } else {
        surface = m_egl_surface;
    }

    if (m_egl_context == EGL_NO_CONTEXT) {
        EGLint AttribList[] = {
                EGL_CONTEXT_CLIENT_VERSION, 2,
                EGL_NONE
        };
        if (!(context = eglCreateContext(display, config, 0, AttribList))) {
            LOG_D("create context failed.");
            destroy();
            return false;
        }
    } else {
        context = m_egl_context;
    }

    if (!eglMakeCurrent(display, surface, surface, context)) {
        LOG_D("make current failed.");
        destroy();
        return false;
    }

    if (!eglQuerySurface(display, surface, EGL_WIDTH, &width) ||
        !eglQuerySurface(display, surface, EGL_HEIGHT, &height)) {
        LOG_D("query surface failed.");
        destroy();
        return false;
    }

    m_egl_display = display;
    m_egl_surface = surface;
    m_egl_context = context;

    glViewport(0, 0, width, height);

    m_width = width;
    m_height = height;

    ratio = (GLfloat) width / height;

    LOG_D("width=%d,height=%d,ratio=%f\n", m_width, m_height, ratio);

    return true;
}

void GRenderer::start() {
    m_requestExit = false;
    pthread_create(&m_thread_id, 0, threadStartCallback, this);
}

void GRenderer::stop() {

    LOG_D("start to stop grenderer thread.");
    m_requestExit = true;
    pthread_cond_signal(&m_cond);

    LOG_D("nofity finished.");
    if (m_started) {
        LOG_D("join thread");
        pthread_join(m_thread_id, 0);
    } else {
        LOG_D("thread require exit on Stop");
        if (mProxy) {
            mProxy->setContextLost(true);
            mProxy->setThreadExit();
            mProxy->finishProc();
        }

        surfaceExit();
        m_requestSurfaceDestroy = false;
        m_initialized = false;

        destroy();
    }

    LOG_D("finish stop thread.");
}

void GRenderer::destroy() {
    GCanvasManager *m = GCanvasManager::GetManager();
    m->RemoveCanvas(mContextId);
    mProxy = nullptr;

    while (!mBitmapQueue.empty()) {
        struct BitmapCmd *p = reinterpret_cast<struct BitmapCmd *> (mBitmapQueue.front());
        mBitmapQueue.pop();
        delete p;
    }

    LOG_D("context destroy in thread.");

    if (m_egl_context != EGL_NO_CONTEXT && m_egl_display != EGL_NO_DISPLAY) {
        LOG_D("context destroy start in thread.");
        eglMakeCurrent(m_egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (m_egl_surface != EGL_NO_SURFACE) {
            eglDestroySurface(m_egl_display, m_egl_surface);
        }
        eglDestroyContext(m_egl_display, m_egl_context);
        eglTerminate(m_egl_display);
        m_egl_context = EGL_NO_CONTEXT;
        m_egl_display = EGL_NO_DISPLAY;
    }
}

void GRenderer::surfaceExit() {
    LOG_D("surface destroy in thread.");
    if (m_egl_display != EGL_NO_DISPLAY && m_egl_surface != EGL_NO_SURFACE) {
        LOG_D("surface destroy start in thread.");
        eglMakeCurrent(m_egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroySurface(m_egl_display, m_egl_surface);
        m_egl_surface = EGL_NO_SURFACE;
    }
}

void GRenderer::drawFrame() {
    if (mProxy != nullptr) {
        LOG_D("start to linkNativeGLProc.");
        mProxy->LinkNativeGLProc();
    }
}

void GRenderer::renderLoop() {
    while (!m_requestExit) {

        pthread_mutex_lock(&m_mutex);

        if (!mProxy || (mProxy && !mProxy->continueProcess() &&
                        !m_viewportchanged && !m_requestSurfaceDestroy)) {
            pthread_cond_wait(&m_cond, &m_mutex);
        }


        if (m_requestSurfaceDestroy && !m_requestExit) {
            pthread_cond_wait(&m_cond, &m_mutex);
        }

        if (mProxy != nullptr) {
            mProxy->finishProc();
        }

        if (m_viewportchanged) {
            if (!m_initialized) {
                bool initResult = initialize();
                m_initialized = initResult;
                if (!initResult) {
                    break;
                }
                if (mProxy) mProxy->setContextLost(false);
            }

            mProxy->OnSurfaceChanged(0, 0, m_width, m_height);
            mProxy->SetClearColor(mClearColor);
            mProxy->SetDevicePixelRatio(m_device_pixel_ratio);
            m_viewportchanged = false;

            if (!m_requestInitialize) {
                m_sendEvent = true;
                m_requestInitialize = true;
            }

            sem_post(&m_SyncSem);
        }


        if (m_bindtexture && m_egl_surface != EGL_NO_SURFACE) {
            while (!mBitmapQueue.empty()) {
                struct BitmapCmd *p = reinterpret_cast<struct BitmapCmd * >(mBitmapQueue.front());
                mProxy->bindTexture(*p);

                mBitmapQueue.pop();
                delete p;
            }

            m_bindtexture = false;
            sem_post(&m_SyncSem);
        }

        if (m_subImage2D && m_egl_surface != EGL_NO_SURFACE) {
            while (!mBitmapQueue.empty()) {
                struct BitmapCmd *p = reinterpret_cast<struct BitmapCmd * >(mBitmapQueue.front());
                mProxy->texSubImage2D(*p);

                mBitmapQueue.pop();
                delete p;
            }

            m_subImage2D = false;
            sem_post(&m_SyncSem);
        }

        if (m_egl_display) {
            drawFrame();
            if (m_refresh) {
                if (!eglSwapBuffers(m_egl_display, m_egl_surface)) {
                }
                m_refresh = false;
            }
        }

        pthread_mutex_unlock(&m_mutex);
    }

    if (m_requestExit) {
        LOG_D("thread require exit.");
        if (mProxy) {
            mProxy->setContextLost(true);
            mProxy->setThreadExit();
            mProxy->finishProc();
        }

        surfaceExit();
        m_requestSurfaceDestroy = false;
        m_initialized = false;

        destroy();
    }
}

void *GRenderer::threadStartCallback(void *myself) {
    GRenderer *renderer = (GRenderer *) myself;

    renderer->m_started = true;
    renderer->renderLoop();
    renderer->m_started = false;
    LOG_D("renderer thread exit.");
    pthread_exit(0);
}

void GRenderer::signalUpGLthread() {
    pthread_mutex_lock(&m_mutex);
    pthread_cond_signal(&m_cond);
    pthread_mutex_unlock(&m_mutex);
}

void GRenderer::setRefreshFlag(bool refresh) {
    m_refresh = refresh;
}

void GRenderer::requestCreateCanvas(const std::string contextid) {
    mContextId = contextid;

    if (!m_createCanvas) {
        LOG_D("not create canvas create");
        if (!mProxy) {
            mProxy = new GcanvasWeexAndroid(mContextId, this);
            mProxy->CreateContext();
            mProxy->SetContextType(m_context_type);
            GCanvasManager *m = GCanvasManager::GetManager();
            m->AddCanvas(mProxy);
            m_createCanvas = true;
            m_sendEvent = true;
        }
        pthread_cond_signal(&m_cond);
    } else {
        LOG_D("already create the canvas");
    }
}

void GRenderer::requestViewportChanged() {
    LOG_D("requestViewportChanged");
    m_viewportchanged = true;
    pthread_cond_signal(&m_cond);
    waitUtilTimeout(&m_SyncSem, GCANVAS_TIMEOUT);
}

void GRenderer::bindTexture(JNIEnv *env, jobject bitmap, int id, int target, int level,
                            int internalformat,
                            int format, int type) {

    AndroidBitmapInfo info;
    memset(&info, 0, sizeof(info));
    AndroidBitmap_getInfo(env, bitmap, &info);
    // Check format, only RGB565 & RGBA are supported
    if (info.width <= 0 || info.height <= 0 ||
        (info.format != ANDROID_BITMAP_FORMAT_RGB_565 &&
         info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) &&
        info.format != ANDROID_BITMAP_FORMAT_RGBA_4444) {
        return;
    } else {
        if (info.format == ANDROID_BITMAP_FORMAT_RGB_565) {
            LOG_D("the bitmap is rgb format.");
            format = GL_RGB;
            internalformat = GL_RGB;
        } else if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
                   info.format == ANDROID_BITMAP_FORMAT_RGBA_4444) {
            LOG_D("the bitmap is rgba format.");
            format = GL_RGBA;
            internalformat = GL_RGBA;
        } else {
            LOG_W("the bitmap format=%d not support.", info.format);
            return;
        }
    }

    // Lock the bitmap to get the buffer
    void *pixels = NULL;
    int res = AndroidBitmap_lockPixels(env, bitmap, &pixels);
    if (pixels == NULL) {
        return;
    }

    AndroidBitmap_unlockPixels(env, bitmap);

    struct BitmapCmd *p = new struct BitmapCmd();
    p->Bitmap = pixels;
    p->width = info.width;
    p->height = info.height;
    p->target = target;
    p->level = level;
    p->interformat = internalformat;
    p->format = format;
    p->type = type;
    p->id = id;

    if (this->mProxy != nullptr) {
        mBitmapQueue.push(p);
        m_bindtexture = true;

        LOG_D("start to require bindtexure,width=%d,height=%d,target=%d,level=%d,internalformat=%d,format=%d,type=%d\n",
              info.width, info.height, target, level, internalformat, format, type);
        pthread_cond_signal(&m_cond);

        gcanvas::waitUtilTimeout(&m_SyncSem, GCANVAS_TIMEOUT);

        LOG_D("finish wait in bindtexture.");
    } else {
        delete p;
        LOG_D("the proxy is null when binding texture.");
    }
}

void
GRenderer::texSubImage2D(JNIEnv *env, jobject bitmap, int id, int target, int level, int xoffset,
                         int yoffset,
                         int format, int type) {

    AndroidBitmapInfo info;
    memset(&info, 0, sizeof(info));
    AndroidBitmap_getInfo(env, bitmap, &info);
    // Check format, only RGB565 & RGBA are supported
    if (info.width <= 0 || info.height <= 0 ||
        (info.format != ANDROID_BITMAP_FORMAT_RGB_565 &&
         info.format != ANDROID_BITMAP_FORMAT_RGBA_8888)) {
        return;
    }

    // Lock the bitmap to get the buffer
    void *pixels = NULL;
    int res = AndroidBitmap_lockPixels(env, bitmap, &pixels);
    if (pixels == NULL) {
        return;
    }

    AndroidBitmap_unlockPixels(env, bitmap);

    struct BitmapCmd *p = new struct BitmapCmd();
    p->Bitmap = pixels;
    p->width = info.width;
    p->height = info.height;
    p->target = target;
    p->level = level;
    p->xoffset = xoffset;
    p->yoffset = yoffset;
    p->format = format;
    p->type = type;
    p->id = id;

    if (this->mProxy != nullptr) {
        mBitmapQueue.push(p);
        m_subImage2D = true;

        LOG_D("start to require texSubImage2D,width=%d,height=%d,target=%d,level=%d,xoffset=%d,yoffset=%d,format=%d,type=%d\n",
              info.width, info.height, target, level, xoffset, yoffset, format, type);
        pthread_cond_signal(&m_cond);

        gcanvas::waitUtilTimeout(&m_SyncSem, GCANVAS_TIMEOUT);

        LOG_D("finish wait in texSubImage2D.");
    } else {
        delete p;
        LOG_D("the proxy is null when texSubImage2D texture.");
    }
}

void GRenderer::surfaceDestroy() {
    LOG_D("surface destroy request in grenderer.");
    m_requestSurfaceDestroy = true;
    pthread_cond_signal(&m_cond);

}

bool GRenderer::sendEvent() {
    bool ret = m_sendEvent;
    m_sendEvent = false;
    return ret;
}

void GRenderer::setDevicePixelRatio(const float ratio) {
    this->m_device_pixel_ratio = ratio;
    if (mProxy) {
        mProxy->SetDevicePixelRatio(ratio);
    }
}
