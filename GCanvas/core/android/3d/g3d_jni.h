//
// Created by yuantong on 2017/8/1.
//



#ifndef G_CANVAS_G3D_JNI_H
#define G_CANVAS_G3D_JNI_H

#include <jni.h>

extern "C" {

JNIEXPORT void JNICALL
Java_com_taobao_gcanvas_surface_GTextureViewCallback_onSurfaceCreated(JNIEnv *, jobject, jstring,
                                                                      jobject);

JNIEXPORT void JNICALL
Java_com_taobao_gcanvas_surface_GTextureViewCallback_onSurfaceChanged(JNIEnv *, jobject, jstring,
                                                                      jobject, jint, jint, jint, jstring);


JNIEXPORT void JNICALL
Java_com_taobao_gcanvas_surface_GTextureViewCallback_onSurfaceDestroyed(JNIEnv *, jobject, jstring,
                                                                        jobject);


JNIEXPORT void JNICALL
Java_com_taobao_gcanvas_surface_GTextureViewCallback_onRenderExit(JNIEnv *env,
                                                                  jobject instance,
                                                                  jstring key);
}

#endif //G_CANVAS_G3D_JNI_H
