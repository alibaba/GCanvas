//
// Created by yuantong on 2017/8/1.
//

#include "g3d_jni.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "jsc/weexjsc.h"
#include <GCanvasLinkNative.h>
#include <dlfcn.h>
#include <gcanvas/GPoint.h>
#include "gmanager.h"

//using namespace taobao::g_engine::gcanvas;
using namespace std;
using namespace gcanvas;
using namespace WEEXJSC;


char *jstringToString_(JNIEnv *env, jstring jstr) {
    char *strReturn = nullptr;
    jclass stringCls = env->FindClass("java/lang/String");
    jstring utfString = env->NewStringUTF("utf-8");
    jmethodID methodId =
            env->GetMethodID(stringCls, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray byteArray = (jbyteArray) env->CallObjectMethod(jstr, methodId, utfString);
    jsize arrayLength = env->GetArrayLength(byteArray);
    jbyte *byte = env->GetByteArrayElements(byteArray, JNI_FALSE);
    if (arrayLength > 0) {
        strReturn = (char *) malloc((size_t) arrayLength + 1);
        memcpy(strReturn, byte, (size_t) arrayLength);
        strReturn[arrayLength] = 0;
    }
    env->ReleaseByteArrayElements(byteArray, byte, 0);
    env->DeleteLocalRef(utfString);

    return strReturn;
}

JNIEXPORT void JNICALL
Java_com_taobao_gcanvas_surface_GTextureViewCallback_onSurfaceCreated(JNIEnv *env, jobject instance,
                                                                      jstring key,
                                                                      jobject surface) {

}

JNIEXPORT void JNICALL
Java_com_taobao_gcanvas_surface_GTextureViewCallback_onSurfaceChanged(JNIEnv *env, jobject instance,
                                                                      jstring key,
                                                                      jobject surface, jint format,
                                                                      jint width, jint height,
                                                                      jstring clearColor) {
    if (!key || !surface) {
        return;
    }

    LOG_D("onSurfaceChanged. width=%d,height=%d\n", width, height);

    const char *str_chars = env->GetStringUTFChars(key, NULL);
    if (!str_chars) {
        env->ReleaseStringUTFChars(key, str_chars);
        return;
    }

    string cxx_string = string(str_chars);

    GRenderer *render = GManager::getSingleton()->findRenderer(cxx_string);
    LOG_D("onSurfaceChanged findRender : %s", str_chars);
    if (!render) {
        LOG_D("onSurfaceChanged new render : %s", str_chars);
        render = GManager::getSingleton()->newRenderer(cxx_string);
    }

    if (render) {
        ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
        LOG_D("onSurfaceChanged ANativeWindow_fromSurface");
        render->setNativeWindow(window);
        render->m_width = width;
        render->m_height = height;
        if (!render->m_started) {
            if (clearColor) {
                char *ccolor = jstringToString_(env, clearColor);
                render->mClearColor = StrValueToColorRGBA(ccolor);
                free(ccolor);
                LOG_D("parse color r:%f, g:%f, b:%f, a:%f", render->mClearColor.rgba.r,
                      render->mClearColor.rgba.g, render->mClearColor.rgba.b,
                      render->mClearColor.rgba.a);
            }

            render->start();
            render->requestViewportChanged();
        } else {
            render->requestViewportChanged();
        }

        if (render->mProxy) {
            render->mProxy->initWebglExt();
        }
    }

    env->ReleaseStringUTFChars(key, str_chars);
}

JNIEXPORT void JNICALL
Java_com_taobao_gcanvas_surface_GTextureViewCallback_onSurfaceDestroyed(JNIEnv *env,
                                                                        jobject instance,
                                                                        jstring key,
                                                                        jobject surface) {
    if (!key || !surface) {
        return;
    }

    LOG_D("onSurfaceDestroyed");

    const char *str_chars = env->GetStringUTFChars(key, NULL);
    if (!str_chars) {
        env->ReleaseStringUTFChars(key, str_chars);
        return;
    }

    string cxx_string = string(str_chars);

    GRenderer *render = GManager::getSingleton()->findRenderer(cxx_string);
    if (render) {
        render->surfaceDestroy();
    }

    env->ReleaseStringUTFChars(key, str_chars);
}

JNIEXPORT void JNICALL
Java_com_taobao_gcanvas_surface_GTextureViewCallback_onRenderExit(JNIEnv *env,
                                                                  jobject instance,
                                                                  jstring key) {
    if (!key) {
        return;
    }

    LOG_D("onRenderExit");

    const char *str_chars = env->GetStringUTFChars(key, NULL);
    if (!str_chars) {
        env->ReleaseStringUTFChars(key, str_chars);
        return;
    }

    string cxx_string = string(str_chars);

    GManager::getSingleton()->removeRenderer(cxx_string);

    env->ReleaseStringUTFChars(key, str_chars);
}