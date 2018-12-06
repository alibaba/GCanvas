//
// Created by yuantong on 2018/4/21.
//

#ifndef GCANVAS_EXPORT_H
#define GCANVAS_EXPORT_H

#ifdef __GNUC__// it means the compiler is GCC version 4.0 or later
    #ifdef ANDROID
        #define API_EXPORT __attribute__((visibility ("default")))
        #define API_LOCAL __attribute__((visibility("hidden")))
    #else
        #define API_EXPORT
        #define API_LOCAL
    #endif

#else
    #error "##### requires gcc version >= 4.0 #####"
#endif

#endif //GCANVAS_EXPORT_H
