////
//// Created by 鸾萱 on 2017/9/3.
////
//
//#include <support/Log.h>
//#include "GCanvasWindvaneLinkNative.h"
//#include "GCanvas.hpp"
//#include "GCanvasManager.h"
//#include "GCanvasString.hpp"
//
//
//using namespace gcanvas;
//using namespace std;
//
//bool GCanvasWindvaneLinkNative::hasAttached;
//JavaVM *GCanvasWindvaneLinkNative::mJvm;
//JNIEnv *GCanvasWindvaneLinkNative::mJe;
//jclass GCanvasWindvaneLinkNative::mClass;
//
//__attribute__ ((visibility ("default"))) const IString *
//GCanvasWindvaneLinkNative::CallNative(void *callbackParam, const char *ContextId, int type,
//                                      const char *args) {
//    if (ContextId == nullptr || args == nullptr || !strcmp(args, "")) {
//        LOG_D("parameter error.\n");
//        return new GCanvasString("parameter error");
//    }
//    if (type == -1) {
//        LOG_D("type -1.\n");
//        if (mJvm == nullptr) {
//            LOG_D("type error.\n");
//            return new GCanvasString("null jvm");
//        }
//        if (!hasAttached) {
//            if (mJvm->AttachCurrentThread(&mJe, NULL) == JNI_OK) {
//                hasAttached = true;
//            } else {
//                LOG_D("attach fail");
//            }
//        }
//        jclass plugin_class = mClass;
//        if (plugin_class) {
//            jmethodID plugin_method;
//            plugin_method = mJe->GetStaticMethodID(plugin_class, "executeFromNative",
//                                                   "(Ljava/lang/String;Ljava/lang/String;)V");
//            if (plugin_method) {
//                jstring jcanvasId = mJe->NewStringUTF(ContextId);
//                jstring jargs = mJe->NewStringUTF(args);
//                mJe->CallStaticVoidMethod(plugin_class, plugin_method, jcanvasId, jargs);
//                mJe->DeleteLocalRef(jcanvasId);
//                mJe->DeleteLocalRef(jargs);
//                return new GCanvasString("");
//            } else {
//                LOG_D("find plugin method failed");
//                return new GCanvasString("");
//            }
//        } else {
//            LOG_D("find plugin failed");
//            return new GCanvasString("");
//        }
//
//    } else {
//        GCanvasManager *theManager = GCanvasManager::GetManager();
//        string sContextId = ContextId;
//        GCanvas *theCanvas = theManager->GetCanvas(sContextId);
//        string sArgs = args;
//
//        if (theCanvas) {
//            const char *result = theCanvas->CallNative(type, sArgs);
//            return new GCanvasString(result);
//        }
//        LOG_D("No Canvas");
//        return new GCanvasString("no canvas");
//    }
//}
//
//__attribute__ ((visibility ("default"))) void
//GCanvasWindvaneLinkNative::initJVMEnv(JavaVM *jvm, jclass plugin_class) {
//    hasAttached = false;
//    mJvm = jvm;
//    mClass = plugin_class;//todo 记得释放！！！
//}
//
//__attribute__ ((visibility ("default"))) void
//GCanvasWindvaneLinkNative::releaseGRef(JNIEnv *je) {
//    je->DeleteGlobalRef(mClass);
//}
