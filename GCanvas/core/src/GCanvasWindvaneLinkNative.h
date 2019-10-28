////
//// Created by 鸾萱 on 2017/9/3.
////
//
//#ifndef GCANVAS_GCANVASWINDVANELINKNATIVE_H
//#define GCANVAS_GCANVASWINDVANELINKNATIVE_H
//
//#include <jni.h>
//#include "IString.h"
//
//namespace gcanvas {
//    class GCanvasWindvaneLinkNative {
//    public:
//        static const IString *
//        CallNative(void *callbackParam, const char *ContextId, int type, const char *args);
//
//        static void initJVMEnv(JavaVM *jvm, jclass plugin_class);
//
//        static void releaseGRef(JNIEnv *je);
//
//        static bool hasAttached;
//        static JavaVM *mJvm;
//        static JNIEnv *mJe;
//        static jclass mClass; //todo 记得释放！！！
//    };
//
//
//}
//
//#endif //GCANVAS_GCANVASWINDVANELINKNATIVE_H
