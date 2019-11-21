/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include <jni.h>
/* Header for class com_taobao_gcanvas_GCanvasJNI */

#ifndef _Included_com_taobao_gcanvas_GCanvasJNI
#define _Included_com_taobao_gcanvas_GCanvasJNI
#ifdef __cplusplus
extern "C" {
#endif

#ifdef GCANVAS_WEEX

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_newCanvas(JNIEnv *,
                                                                    jclass,
                                                                    jstring,
                                                                    jint,
                                                                    jstring);

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_freeCanvas(JNIEnv *,
                                                                     jclass,
                                                                     jstring);

/*
 * Class:     com_taobao_gcanvas_GCanvasJNI
 * Method:    setBackgroundColor
 * Signature: (III)V
 */

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_setClearColor(
        JNIEnv *, jclass, jstring, jstring);

/*
 * Class:     com_taobao_gcanvas_GCanvasJNI
 * Method:    setOrtho
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_setOrtho(JNIEnv *,
                                                                   jclass,
                                                                   jstring,
                                                                   jint, jint);

/*
 * Class:     com_taobao_gcanvas_GCanvasJNI
 * Method:    addTexture
 * Signature: (IIII)V
 */
JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_addTexture(
        JNIEnv *, jclass, jstring, jint, jint, jint, jint);

/*
 * Class:     com_taobao_gcanvas_GCanvasJNI
 * Method:    addPngTexture
 * Signature:
 * (Ljava/lang/Object;Ljava/lang/String;ILcom/taobao/gcanvas/GCanvasTextureDimension;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_taobao_gcanvas_GCanvasJNI_addPngTexture(
        JNIEnv *, jclass, jstring, jobject, jstring, jint, jobject);

/*
 * Class:     com_taobao_gcanvas_GCanvasJNI_addPngTextureByStream
 * Method:    addPngTexture
 * Signature:
 * (Ljava/lang/Object;Ljava/lang/String;ILcom/taobao/gcanvas/GCanvasTextureDimension;)Z
 */
JNIEXPORT jboolean JNICALL
Java_com_taobao_gcanvas_GCanvasJNI_addPngTextureByStream(JNIEnv *, jclass,
                                                         jstring, jbyteArray,
                                                         jint, jobject);

/*
 * Class:     com_taobao_gcanvas_GCanvasJNI
 * Method:    removeTexture
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_removeTexture(
        JNIEnv *, jclass, jstring, jint);

/*
 * Class:     com_taobao_gcanvas_GCanvasJNI
 * Method:    render
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_render(
        JNIEnv *je, jclass jc, jstring contextId,
        jstring renderCommands);

/*
 * Class:     com_taobao_gcanvas_GCanvasJNI
 * Method:    surfaceChanged
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_surfaceChanged(
        JNIEnv *, jclass, jstring, jint, jint);

/*
 * Class:     com_taobao_gcanvas_GCanvasJNI
 * Method:    captureGLLayer
 * Signature: (Ljava/lang/String;IIIILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_captureGLLayer(
        JNIEnv *, jclass, jstring, jstring, jint, jint, jint, jint, jstring);

/*
 * Class:     com_taobao_gcanvas_GCanvasJNI
 * Method:    release
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_release(JNIEnv *,
                                                                  jclass);

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_init(JNIEnv *,
                                                               jclass);

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_uninit(JNIEnv *,
                                                                 jclass);

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_setTyOffsetFlag(
        JNIEnv *je, jclass jc, jstring contextId, jboolean flag);

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_setHiQuality(
        JNIEnv *je, jclass jc, jstring contextId, jboolean isHiQuality);

JNIEXPORT jboolean JNICALL
Java_com_taobao_gcanvas_GCanvasJNI_isNeonSupport(JNIEnv *, jclass);

JNIEXPORT jboolean JNICALL
Java_com_taobao_gcanvas_GCanvasJNI_isFboSupport(JNIEnv *, jclass, jstring);

JNIEXPORT jstring JNICALL Java_com_taobao_gcanvas_GCanvasJNI_getImageData(
        JNIEnv *je, jclass jc, jstring contextId, jint x, jint y, jint width,
        jint height);

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_setDevicePixelRatio(
        JNIEnv *je, jclass jc, jstring contextId, jdouble ratio);
#endif

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_setFallbackFont(
        JNIEnv *je, jclass jc, jstring fallback_font_name,
        jstring system_font_location);

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_addFontFamily(
        JNIEnv *je, jclass jc, jobjectArray font_names, jobjectArray font_files);

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_addFallbackFontFamily(
        JNIEnv *je, jclass jc, jobjectArray font_files);

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_setLogLevel(
        JNIEnv *je, jclass jc, jstring logLevel);

#ifdef GCANVAS_WEEX

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_setPreCompilePath(
        JNIEnv *je, jclass jc, jstring path);

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_setContextType(
        JNIEnv *je, jclass jc, jstring contextId, jint type);

JNIEXPORT jstring JNICALL Java_com_taobao_gcanvas_GCanvasJNI_getAllParameter(
        JNIEnv *je, jclass jc, jstring contextId);

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_setConfig(JNIEnv *je,
                                                                    jclass jc,
                                                                    jstring key,
                                                                    jint value);
JNIEXPORT jstring JNICALL Java_com_taobao_gcanvas_GCanvasJNI_exeSyncCmd
        (JNIEnv *je, jclass jc, jstring ContextID, jint type, jstring args);

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_bindTexture(
        JNIEnv *je, jclass jc, jstring contextId, jobject bitmap, jint id,
        jint target, jint level, jint internalformat, jint format, jint type);

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_texSubImage2D(
        JNIEnv *je, jclass jc, jstring contextId, jobject bitmap, jint id,
        jint target, jint level, jint xoffset, jint yoffset, jint format, jint type);


JNIEXPORT bool JNICALL Java_com_taobao_gcanvas_GCanvasJNI_sendEvent(
		JNIEnv *je, jclass jc, jstring contextId);

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_registerCallback(JNIEnv *je, jclass jc, jstring soPath, jint version);

JNIEXPORT jint JNICALL Java_com_taobao_gcanvas_GCanvasJNI_getNativeFps(JNIEnv *je, jclass jc, jstring contextId);

#endif

#ifdef __cplusplus
}
#endif
#endif
