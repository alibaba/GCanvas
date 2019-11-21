//
// Created by yuantong on 2017/8/1.
//

#ifndef G_CANVAS_GRENDER_H
#define G_CANVAS_GRENDER_H

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android/native_window.h>
#include <pthread.h>

#include "../../GCanvas.hpp"
#include "GcanvasWeexAndroid.h"

using namespace gcanvas;

class GcanvasWeexAndroid;

class GRenderer {
public:
    GRenderer(std::string key);

    void start();

    void stop();

    void destroy();

    virtual ~GRenderer();

    void setNativeWindow(ANativeWindow *window);

    ANativeWindow *getNativeWindow();

    bool initialize();

    void signalUpGLthread();

    void setRefreshFlag(bool refresh);

    void requestCreateCanvas(std::string contextid);

    void requestViewportChanged();

    void waitResponse();

    void bindTexture(JNIEnv *env, jobject bitmap, int id, int target, int level, int internalformat,
                     int format, int type);

    void texSubImage2D(JNIEnv *env, jobject bitmap, int id, int target, int level, int xoffset,
                       int yoffset,
                       int format, int type);

    void surfaceDestroy();

    void surfaceExit();

    void setDevicePixelRatio(const float ratio);

public:
    pthread_t m_thread_id;
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond = PTHREAD_COND_INITIALIZER;
    ANativeWindow *m_window;
    EGLDisplay m_egl_display;
    EGLSurface m_egl_surface;
    EGLContext m_egl_context;
    int m_context_type = 0;
    bool m_requestExit = false;
    bool m_requestSurfaceDestroy = false;
    bool m_refresh = false;
    bool m_createCanvas = false;
    GcanvasWeexAndroid *mProxy = nullptr;
    sem_t m_SyncSem;
    sem_t m_dummy;
    std::string mContextId;
    int m_width = 0;
    int m_height = 0;
    bool m_started = false;
    bool m_viewportchanged = false;
    bool m_bindtexture = false;
    bool m_subImage2D = false;
    GColorRGBA mClearColor;
    float m_device_pixel_ratio = 1.0f;

    std::queue<struct BitmapCmd *> mBitmapQueue;
    bool m_initialized = false;
    bool m_requestInitialize = false;

    bool m_sendEvent = false;

    static void *threadStartCallback(void *myself);

    void drawFrame();

    void renderLoop();

    bool sendEvent();

};


#endif //G_CANVAS_GRENDER_H
