/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include <GLES/gl.h>
#include <stdlib.h>
#include <list>

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <string.h>

#include <dlfcn.h>

#include "GCanvasJNI.h"
#include "support/Util.h"
#include "support/Log.h"

#include "GSystemFontInformation.h"


#ifdef GCANVAS_WEEX

#include <3d/gmanager.h>
#include "GCanvasManager.h"
#include "GCanvasLinkNative.h"
#include "elf_sym_reader.h"

#endif


#ifdef ENABLE_GPROF
#include "prof.h"
#endif

using namespace std;
using namespace gcanvas;

// -----------------------------------------------------------
// --                     JNI interface                     --
// -----------------------------------------------------------


char *jstringToString(JNIEnv *env, jstring jstr) {
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


#ifdef GCANVAS_WEEX

typedef const char *(*FunType)(const char *, int, const char *);

typedef void (*RegisterFunc)(FunType fp);


RegisterFunc dlopen_weex_so_above_android_7(const char *soPath) {
    RegisterFunc result = (RegisterFunc) ali::getSymbolAddr((char *) "Inject_GCanvasFunc",
                                                            (char *) soPath, "libweexcore.so");
    LOG_E("load libweexcore.so result is 0x%x", result);
    return result;
}

void RegisterCallNativeCallback_belowN() {
    void *handle = dlopen("libweexcore.so", RTLD_NOW);
    if (!handle) {
        LOG_D("load libweexcore.so failed,error=%s\n", dlerror());
        dlclose(handle);
        return;
    }

    RegisterFunc func = (RegisterFunc) dlsym(handle, "Inject_GCanvasFunc");
    if (!func) {
        LOG_D("load Inject_GCanvasFunc failed,error=%s\n", dlerror());
        dlclose(handle);
        return;
    }

    func(GCanvasLinkNative::CallNative);

    dlclose(handle);
}


void executeCallbacks(JNIEnv *je, jstring contextId) {
    GCanvasManager *theManager = GCanvasManager::GetManager();
    char *cid = jstringToString(je, contextId);
    string canvasId = cid;
    free(cid);
    GCanvasWeex *theCanvas = (GCanvasWeex *) theManager->GetCanvas(canvasId);

    Callback *callback = theCanvas ? theCanvas->GetNextCallback() : nullptr;
    if (callback) {
        jclass cls = je->FindClass("com/taobao/gcanvas/GCanvas");
        if (je->ExceptionCheck()) {
            je->DeleteLocalRef(cls);
            return;
        }

        jmethodID callbackMethodId = je->GetStaticMethodID(cls, "executeCallback",
                                                           "(Ljava/lang/String;ZLjava/lang/String;)V");
        if (je->ExceptionCheck()) {
            return;
        }

        while (callback) {
            jstring callbackIdStr = je->NewStringUTF(callback->callbackId);
            jstring result = je->NewStringUTF(callback->result);
            je->CallStaticVoidMethod(cls, callbackMethodId, callbackIdStr,
                                     callback->isError, result);
            theCanvas->PopCallbacks();
            callback = theCanvas->GetNextCallback();
            je->DeleteLocalRef(callbackIdStr);
            je->DeleteLocalRef(result);
        }
        je->DeleteLocalRef(cls);
    }
}

extern int g_js_version;

#ifdef GCANVAS_WEEX

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_newCanvas(
        JNIEnv *je, jclass jc, jstring contextId, jint jsVersion, jstring clearColor) {
    LOG_E("Canvas JNI::newCanvas. jsVer=%d", jsVersion);
    GCanvasManager *theManager = GCanvasManager::GetManager();
    char *cid = jstringToString(je, contextId);
    string canvasId = cid;
    free(cid);
    GCanvasWeex *theCanvas = theManager->NewCanvasWeex(canvasId);
    if (theCanvas) {
        char *ccolor = jstringToString(je, clearColor);
        string colorId = ccolor;
        free(ccolor);
        theCanvas->SetClearColor(StrValueToColorRGBA(colorId.c_str()));
    }
}

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_freeCanvas(
        JNIEnv *je, jclass jc, jstring contextId) {
    LOG_D("Canvas JNI::freeCanvas.");
    GCanvasManager *theManager = GCanvasManager::GetManager();
    char *cid = jstringToString(je, contextId);
    string canvasId = cid;
    free(cid);
    theManager->RemoveCanvas(canvasId);
}


JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_setClearColor(
        JNIEnv *je, jclass jc, jstring contextId, jstring color) {
    GCanvasManager *theManager = GCanvasManager::GetManager();
    char *cid = jstringToString(je, contextId);
    string canvasId = cid;
    free(cid);
    char *colorStr = jstringToString(je, color);
    string colorId = colorStr;
    free(colorStr);
    GCanvasWeex *theCanvas = (GCanvasWeex *) theManager->GetCanvas(canvasId);
    if (theCanvas) {
        LOG_D("Canvas JNI::setClearColor. %s", colorId.c_str());
        theCanvas->SetClearColor(StrValueToColorRGBA(colorId.c_str()));
    }
}

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_setOrtho(
        JNIEnv *je, jclass jc, jstring contextId, jint width, jint height) {
    GCanvasManager *theManager = GCanvasManager::GetManager();
    char *cid = jstringToString(je, contextId);
    string canvasId = cid;
    free(cid);
    GCanvasWeex *theCanvas = (GCanvasWeex *) theManager->GetCanvas(canvasId);
    if (theCanvas) {
        LOG_D("Canvas JNI::SetOrtho.");
        theCanvas->SetOrtho(width, height);
    }
}

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_addTexture(
        JNIEnv *je, jclass jc, jstring contextId, jint id, jint glID, jint width,
        jint height) {
    GCanvasManager *theManager = GCanvasManager::GetManager();
    char *cid = jstringToString(je, contextId);
    string canvasId = cid;
    free(cid);
    GCanvasWeex *theCanvas = (GCanvasWeex *) theManager->GetCanvas(canvasId);
    if (theCanvas) {
        LOG_D("Canvas JNI::AddTexture");
        theCanvas->AddTexture(id, glID, width, height);
    }
}

JNIEXPORT jboolean JNICALL Java_com_taobao_gcanvas_GCanvasJNI_addPngTexture(
        JNIEnv *je, jclass jc, jstring contextId, jobject assetManager,
        jstring path, jint textureId, jobject dimension) {
    bool success = false;
    GCanvasManager *theManager = GCanvasManager::GetManager();
    char *cid = jstringToString(je, contextId);
    string canvasId = cid;
    free(cid);
    GCanvasWeex *theCanvas = (GCanvasWeex *) theManager->GetCanvas(canvasId);
    if (theCanvas) {
        LOG_D("Canvas JNI::addPngTexture");

        AAssetManager *mgr = AAssetManager_fromJava(je, assetManager);
        if (mgr == nullptr)
            return (jboolean) false;

        const char *p = je->GetStringUTFChars(path, 0);
        AAsset *asset = AAssetManager_open(mgr, p, AASSET_MODE_UNKNOWN);
        je->ReleaseStringUTFChars(path, p);
        if (asset == nullptr)
            return (jboolean) false;

        size_t size = (size_t) AAsset_getLength(asset);
        unsigned char *buffer = (unsigned char *) malloc(size);
        if (buffer == nullptr)
            return (jboolean) false;

        AAsset_read(asset, buffer, size);
        AAsset_close(asset);

        unsigned int width;
        unsigned int height;
        success = theCanvas->AddPngTexture(buffer, (unsigned int) size, textureId,
                                           &width, &height);
        free(buffer);

        if (success) {
            LOG_D("Canvas JNI::addPngTexture success ");
            jclass cls = je->GetObjectClass(dimension);
            jfieldID wID = je->GetFieldID(cls, "width", "I");
            je->SetIntField(dimension, wID, (int) width);
            jfieldID hID = je->GetFieldID(cls, "height", "I");
            je->SetIntField(dimension, hID, (int) height);
        }
    }
    return (jboolean) success;
}

JNIEXPORT jboolean JNICALL
Java_com_taobao_gcanvas_GCanvasJNI_addPngTextureByStream(JNIEnv *je, jclass jc,
                                                         jstring contextId,
                                                         jbyteArray array,
                                                         jint id, jobject dim) {
    bool success = false;
    GCanvasManager *theManager = GCanvasManager::GetManager();
    char *cid = jstringToString(je, contextId);
    string canvasId = cid;
    free(cid);
    GCanvasWeex *theCanvas = (GCanvasWeex *) theManager->GetCanvas(canvasId);
    if (theCanvas) {
        LOG_D("Canvas JNI::addPngTextureByStream");
        jboolean isCopy;
        jbyte *buffer = je->GetByteArrayElements(array, &isCopy);
        jsize length = je->GetArrayLength(array);
        unsigned int width;
        unsigned int height;
        success = theCanvas->AddPngTexture((unsigned char *) buffer, (unsigned int) length, id,
                                           &width, &height);
        je->ReleaseByteArrayElements(array, buffer, 0);
        if (success) {
            LOG_D("Canvas JNI::addPngTextureByStream success ");
            jclass cls = je->GetObjectClass(dim);
            jfieldID widthId = je->GetFieldID(cls, "width", "I");
            je->SetIntField(dim, widthId, (int) width);
            jfieldID heightId = je->GetFieldID(cls, "height", "I");
            je->SetIntField(dim, heightId, (int) height);
        }
    }
    return (jboolean) success;
}

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_removeTexture(
        JNIEnv *je, jclass jc, jstring contextId, jint id) {
    GCanvasManager *theManager = GCanvasManager::GetManager();
    char *cid = jstringToString(je, contextId);
    string canvasId = cid;
    free(cid);
    GCanvasWeex *theCanvas = (GCanvasWeex *) theManager->GetCanvas(canvasId);
    if (theCanvas) {
        LOG_D("Canvas JNI::removeTexture");
        theCanvas->RemoveTexture(id);
    }
}

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_render(
        JNIEnv *je, jclass jc, jstring contextId, jstring renderCommands) {
    GCanvasManager *theManager = GCanvasManager::GetManager();
    char *canvasId = jstringToString(je, contextId);
    GCanvasWeex *theCanvas = (GCanvasWeex *) theManager->GetCanvas(canvasId);
    free(canvasId);
    if (theCanvas) {
        const char *rc = je->GetStringUTFChars(renderCommands, 0);
        LOG_D("Java_com_taobao_gcanvas_GCanvasJNI_render, cmd=%s", rc);
        int length = je->GetStringUTFLength(renderCommands);
        if (0 != length) {
            const char *result = theCanvas->CallNative(0x60000001, rc);
            if (result) {
                delete result;
            }
            je->ReleaseStringUTFChars(renderCommands, rc);
//        } else {
//            theCanvas->LinkNativeGLProc();
        }
        executeCallbacks(je, contextId);
    }
}

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_surfaceChanged(
        JNIEnv *je, jclass jc, jstring contextId, jint width, jint height) {
    GCanvasManager *theManager = GCanvasManager::GetManager();
    char *cid = jstringToString(je, contextId);
    string canvasId = cid;
    free(cid);
    GCanvasWeex *theCanvas = (GCanvasWeex *) theManager->GetCanvas(canvasId);
    if (theCanvas) {
        LOG_D("Canvas JNI::OnSurfaceChanged (%d, %d)", width, height);

        theCanvas->OnSurfaceChanged(0, 0, width, height);
    }
}

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_captureGLLayer(
        JNIEnv *je, jclass jc, jstring contextId, jstring callbackId, jint x,
        jint y, jint w, jint h, jstring fileName) {
//    LOG_D("Canvas JNI::captureGLLayer");
//
//    GCanvasManager *theManager = GCanvasManager::GetManager();
//    char *cid = jstringToString(je, contextId);
//    string canvasId = cid;
//    free(cid);
//    GCanvas *theCanvas = theManager->GetCanvas(canvasId);
//    if (theCanvas) {
//        const char *callback = je->GetStringUTFChars(callbackId, 0);
//        const char *fn = je->GetStringUTFChars(fileName, 0);
//        theCanvas->QueueCaptureGLLayer(x, y, w, h, callback, fn);
//        je->ReleaseStringUTFChars(callbackId, callback);
//        je->ReleaseStringUTFChars(fileName, fn);
//    }
}

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_contextLost(
        JNIEnv *je, jclass jc, jstring contextId) {
    LOG_D("Canvas JNI::contextLost");
    GCanvasManager *theManager = GCanvasManager::GetManager();
    char *cid = jstringToString(je, contextId);
    string canvasId = cid;
    free(cid);
    GCanvasWeex *theCanvas = (GCanvasWeex *) theManager->GetCanvas(canvasId);
    if (theCanvas) {
        theCanvas->Clear();
    }
}

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_release(JNIEnv *je,
                                                                  jclass jc) {
    LOG_D("Canvas JNI::Release");
    GCanvasManager::Release();
}

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_setTyOffsetFlag(
        JNIEnv *je, jclass jc, jstring contextId, jboolean flag) {
    LOG_D("Canvas JNI::setTyOffsetFlag");
    char *cid = jstringToString(je, contextId);
    string canvasId = cid;
    free(cid);
    GCanvasManager *theManager = GCanvasManager::GetManager();
    GCanvasWeex *theCanvas = (GCanvasWeex *) theManager->GetCanvas(canvasId);
    if (theCanvas) {
        theCanvas->SetTyOffsetFlag(flag);
    }
}

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_setHiQuality(
        JNIEnv *je, jclass jc, jstring contextId, jboolean isHiQuality) {
    LOG_D("Canvas JNI::setHiQuality");
    char *cid = jstringToString(je, contextId);
    string canvasId = cid;
    free(cid);
    GCanvasManager *theManager = GCanvasManager::GetManager();
    GCanvasWeex *theCanvas = (GCanvasWeex *) theManager->GetCanvas(canvasId);
    if (theCanvas) {
        theCanvas->GetGCanvasContext()->SetHiQuality(isHiQuality);
    }
}

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_setDevicePixelRatio(
        JNIEnv *je, jclass jc, jstring contextId, jdouble ratio) {
    LOG_D("Canvas JNI::setDevicePixelRatio");
    char *canvasId = jstringToString(je, contextId);

    LOG_D("Canvas JNI::setDevicePixelRatio");
    GRenderer *render = GManager::getSingleton()->findRenderer(canvasId);
    if (render) {
        render->setDevicePixelRatio((const float) ratio);
    }
    free(canvasId);
}

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_init(JNIEnv *je, jclass jc) {
#ifdef ENABLE_GPROF
    monstartup("libgcanvas.so");
#endif
}

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_uninit(JNIEnv *,
                                                                 jclass) {
#ifdef ENABLE_GPROF
    moncleanup();
#endif
}

JNIEXPORT jboolean JNICALL
Java_com_taobao_gcanvas_GCanvasJNI_isNeonSupport(JNIEnv *, jclass) {
    return JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_com_taobao_gcanvas_GCanvasJNI_isFboSupport(
        JNIEnv *je, jclass jc, jstring contextId) {
    LOG_D("Canvas JNI::isFboSupport");
    char *cid = jstringToString(je, contextId);
    string canvasId = cid;
    free(cid);
    GCanvasManager *theManager = GCanvasManager::GetManager();
    GCanvasWeex *theCanvas = (GCanvasWeex *) theManager->GetCanvas(canvasId);
    if (theCanvas) {
        return (jboolean) theCanvas->GetGCanvasContext()->IsFboSupport();
    }

    return (jboolean) false;
}

JNIEXPORT jstring JNICALL Java_com_taobao_gcanvas_GCanvasJNI_getImageData(
        JNIEnv *je, jclass jc, jstring contextId, jint x, jint y, jint width,
        jint height) {
    LOG_D("Canvas JNI::getImageData xy=(%d, %d), wh=(%d, %d)", x, y, width,
          height);
    GCanvasManager *theManager = GCanvasManager::GetManager();
    char *cid = jstringToString(je, contextId);
    string canvasId = cid;
    free(cid);
    GCanvasWeex *theCanvas = (GCanvasWeex *) theManager->GetCanvas(canvasId);
    if (NULL == theCanvas) {
        return je->NewStringUTF("");
    }
    std::string strRet;
    theCanvas->GetImageData(x, y, width, height, true, strRet);
    return je->NewStringUTF(strRet.c_str());
}

#endif


extern bool g_use_pre_compile;
extern std::string g_shader_cache_path;

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_setPreCompilePath(
        JNIEnv *je, jclass jc, jstring path) {
    const char *pathPreCompile = je->GetStringUTFChars(path, 0);

    LOG_D("Canvas JNI::setPreCompilePath = %s", pathPreCompile);

    g_use_pre_compile = true;
    g_shader_cache_path = pathPreCompile;

    je->ReleaseStringUTFChars(path, pathPreCompile);
}

JNIEXPORT jstring JNICALL Java_com_taobao_gcanvas_GCanvasJNI_getAllParameter(
        JNIEnv *je, jclass jc, jstring contextId) {
    LOG_D("Canvas JNI::getAllParameter");

    GCanvasManager *theManager = GCanvasManager::GetManager();
    char *cid = jstringToString(je, contextId);
    string canvasId = cid;
    free(cid);
    GCanvasWeex *theCanvas = (GCanvasWeex *) theManager->GetCanvas(canvasId);
    if (NULL == theCanvas) {
        return je->NewStringUTF("");
    }
    std::string strRet;
    theCanvas->GetAllParameter(strRet);
    return je->NewStringUTF(strRet.c_str());
}


JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_setContextType(
        JNIEnv *je, jclass jc, jstring contextId, jint type) {
    LOG_D("Canvas JNI::setContextType %d", type);

    char *canvasId = jstringToString(je, contextId);

    GRenderer *render = GManager::getSingleton()->findRenderer(canvasId);
    if (!render) {
        LOG_D("start to create renderer.id=%s\n", canvasId);
        render = GManager::getSingleton()->newRenderer(canvasId);
    } else {
        LOG_D("render is not null,id=%s\n", render->mContextId.c_str());
    }

    render->m_context_type = type;
    LOG_D("request create canvas start");
    render->requestCreateCanvas(canvasId);
    LOG_D("request create canvas end");
    free(canvasId);
    if (render->mProxy) {
        render->mProxy->initWebglExt();
    }

}

JNIEXPORT jstring JNICALL Java_com_taobao_gcanvas_GCanvasJNI_exeSyncCmd
        (JNIEnv *je, jclass jc, jstring ContextID, jint type, jstring args) {
    char *cid = jstringToString(je, ContextID);
    string contextID = cid;
    free(cid);

    GCanvasManager *theManager = GCanvasManager::GetManager();
    GCanvasWeex *theCanvas = (GCanvasWeex *) theManager->GetCanvas(contextID);
    if (theCanvas) {
        const char *cargs = nullptr;
        if (args != NULL) {
            cargs = (const char *) jstringToString(je, args);
            LOG_D("Canvas JNI::exeSyncCmd type is %d,args is %s\n", type, cargs);
        }

        std::string result = theCanvas->exeSyncCmd(type, cargs);

        LOG_D("Canvas JNI::execSyncCmd result is %s", result.c_str());

        return je->NewStringUTF(result.c_str());
    }

    return nullptr;
}

//extern int g_encode_type;
//extern int g_clear_color_time;

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_setConfig(
        JNIEnv *je, jclass jc, jstring key, jint value) {
    char *configKey = jstringToString(je, key);
    LOG_D("Canvas JNI::setConfig %s=%d", configKey, value);
    if (0 == strcmp(configKey, "encode_type")) {
//        g_encode_type = value;
    } else if (0 == strcmp(configKey, "clear_color_time")) {
//        g_clear_color_time = value;
    }
    free(configKey);
}

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_bindTexture(
        JNIEnv *je, jclass jc, jstring contextId, jobject bitmap, jint id,
        jint target, jint level, jint internalformat, jint format, jint type) {

    LOG_D("bindtexture in gcanvasjni.");

    if (!contextId) {
        return;
    }
    const char *str_chars = je->GetStringUTFChars(contextId, NULL);
    if (!str_chars) {
        je->ReleaseStringUTFChars(contextId, str_chars);
        return;
    }

    string cxx_string = string(str_chars);

    GRenderer *render = GManager::getSingleton()->findRenderer(cxx_string);
    if (render != nullptr) {
        render->bindTexture(je, bitmap, id, target, level, internalformat, format, type);
    }
}

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_texSubImage2D(
        JNIEnv *je, jclass jc, jstring contextId, jobject bitmap,
        jint id, jint target, jint level, jint xoffset, jint yoffset, jint format, jint type) {

    LOG_D("texSubImage2D in gcanvasjni.");

    if (!contextId) {
        return;
    }
    const char *str_chars = je->GetStringUTFChars(contextId, NULL);
    if (!str_chars) {
        je->ReleaseStringUTFChars(contextId, str_chars);
        return;
    }

    string cxx_string = string(str_chars);

    GRenderer *render = GManager::getSingleton()->findRenderer(cxx_string);
    if (render != nullptr) {
        render->texSubImage2D(je, bitmap, id, target, level, xoffset, yoffset, format, type);
    }
}


JNIEXPORT bool JNICALL Java_com_taobao_gcanvas_GCanvasJNI_sendEvent(
        JNIEnv *je, jclass jc, jstring contextId) {
    if (!contextId) {
        return false;
    }
    const char *str_chars = je->GetStringUTFChars(contextId, NULL);
    if (!str_chars) {
        je->ReleaseStringUTFChars(contextId, str_chars);
        return false;
    }

    string cxx_string = string(str_chars);

    GRenderer *render = GManager::getSingleton()->findRenderer(cxx_string);
    if (render != nullptr) {
        return render->sendEvent();
    }

    return false;
}

JNIEXPORT void JNICALL
Java_com_taobao_gcanvas_GCanvasJNI_registerCallback(JNIEnv *je, jclass jc, jstring soPath,
                                                    jint version) {
    LOG_D("start to register jsc callback.");
    if (version >= 24) {
        LOG_D("Register for Android N");
        if (!soPath) {
            LOG_E("so path is empty");
            return;
        }
        const char *cSoPath = je->GetStringUTFChars(soPath, NULL);
        if (!cSoPath) {
            LOG_E("so path is empty");
            je->ReleaseStringUTFChars(soPath, cSoPath);
            return;
        }
        RegisterFunc func = dlopen_weex_so_above_android_7(cSoPath);
        if (func == nullptr) {
            LOG_E("can not find Inject_GCanvasFunc address.");
        } else {
            LOG_D("call Inject_GCanvasFunc success.");
            func(GCanvasLinkNative::CallNative);
        }
    } else {
        LOG_D("Register for Android N Below");
        RegisterCallNativeCallback_belowN();
    }
}


JNIEXPORT jint JNICALL
Java_com_taobao_gcanvas_GCanvasJNI_getNativeFps(JNIEnv *je, jclass jc, jstring contextId) {
    if (!contextId) {
        return false;
    }
    const char *str_chars = je->GetStringUTFChars(contextId, NULL);
    if (!str_chars) {
        je->ReleaseStringUTFChars(contextId, str_chars);
        return false;
    }

    string cxx_string = string(str_chars);

    GCanvasManager *theManager = GCanvasManager::GetManager();
    GCanvasWeex *theCanvas = (GCanvasWeex *) theManager->GetCanvas(cxx_string);
    if (theCanvas) {
        return theCanvas->mFps;
    }

    return 0;
}

#endif


JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_setFallbackFont(
        JNIEnv *je, jclass jc, jstring fallback_font_file,
        jstring system_font_location) {
    // fallback_font_file_jvm is managed by JVM.
    const char *fallback_font_file_jvm = je->GetStringUTFChars(fallback_font_file, 0);
    if (fallback_font_file_jvm == nullptr)
        return; // return if memory overflows.

    // Set the default font file.
    SystemFontInformation::GetSystemFontInformation()->SetDefaultFontFile(
            fallback_font_file_jvm);
    // Release the string managed by JVM.
    je->ReleaseStringUTFChars(fallback_font_file, fallback_font_file_jvm);

    const char *system_font_location_jvm = je->GetStringUTFChars(
            system_font_location, 0);
    if (system_font_location_jvm == nullptr)
        return;

    SystemFontInformation::GetSystemFontInformation()->SetSystemFontLocation(
            system_font_location_jvm);
    je->ReleaseStringUTFChars(system_font_location, system_font_location_jvm);
}

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_addFontFamily(
        JNIEnv *je, jclass jc, jobjectArray font_names, jobjectArray font_files) {
    int length = je->GetArrayLength(font_files);
    std::list<const char *> font_file_list;
    std::list<jstring> font_file_list_jstring;
    for (int i = 0; i < length; ++i) {
        jstring s = (jstring) je->GetObjectArrayElement(font_files, i);
        const char *font_file_jvm = je->GetStringUTFChars(s, 0);

        font_file_list_jstring.push_back(s);
        font_file_list.push_back(font_file_jvm);
    }

    length = je->GetArrayLength(font_names);
    for (int i = 0; i < length; ++i) {
        jstring s = (jstring) je->GetObjectArrayElement(font_names, i);
        const char *font_name_jvm = je->GetStringUTFChars(s, 0);

        auto font_family_ptr =
                SystemFontInformation::GetSystemFontInformation()->FindFontFamily(
                        font_name_jvm);
        if (font_family_ptr == nullptr) {
            // Insert the element if the font is not found.
            SystemFontInformation::GetSystemFontInformation()->InsertFontFamily(
                    font_name_jvm, font_file_list);
        }

        // Release the string managed by JVM.
        je->ReleaseStringUTFChars(s, font_name_jvm);
    }

    // Release the string managed by JVM.
    std::list<const char *>::iterator it_font_file_list =
            font_file_list.begin();
    std::list<jstring>::iterator it_font_file_list_jstring =
            font_file_list_jstring.begin();
    for (; it_font_file_list != font_file_list.end();
           ++it_font_file_list, ++it_font_file_list_jstring) {
        je->ReleaseStringUTFChars(*it_font_file_list_jstring,
                                  *it_font_file_list);
    }
}

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_addFallbackFontFamily(
        JNIEnv *je, jclass jc, jobjectArray font_files) {
    LOG_D("Canvas JNI::addFallbackFontFamily");

    int length = je->GetArrayLength(font_files);
    std::list<const char *> font_file_list;
    std::list<jstring> font_file_list_jstring;
    for (int i = 0; i < length; ++i) {
        jstring s = (jstring) je->GetObjectArrayElement(font_files, i);
        const char *font_file_jvm = je->GetStringUTFChars(s, 0);

        font_file_list_jstring.push_back(s);
        font_file_list.push_back(font_file_jvm);
    }

    SystemFontInformation::GetSystemFontInformation()->InsertFallbackFontFamily(
            font_file_list);

    std::list<const char *>::iterator it_font_file_list =
            font_file_list.begin();
    std::list<jstring>::iterator it_font_file_list_jstring =
            font_file_list_jstring.begin();
    for (; it_font_file_list != font_file_list.end();
           ++it_font_file_list, ++it_font_file_list_jstring) {
        je->ReleaseStringUTFChars(*it_font_file_list_jstring,
                                  *it_font_file_list);
    }

    LOG_D("finish to insert fallbackfont.");
}

JNIEXPORT void JNICALL Java_com_taobao_gcanvas_GCanvasJNI_setLogLevel(
        JNIEnv *je, jclass jc, jstring logLevel) {
    char *level = jstringToString(je, logLevel);

    if (strcmp(level, "debug") == 0)
        SetLogLevel(LOG_LEVEL_DEBUG);
    else if (strcmp(level, "info") == 0)
        SetLogLevel(LOG_LEVEL_INFO);
    else if (strcmp(level, "warn") == 0)
        SetLogLevel(LOG_LEVEL_WARN);
    else if (strcmp(level, "error") == 0)
        SetLogLevel(LOG_LEVEL_ERROR);
    else if (strcmp(level, "fatal") == 0)
        SetLogLevel(LOG_LEVEL_FATAL);

    free(level);
    return;
}


